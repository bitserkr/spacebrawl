#include "gamestatemgr.h"
#include "gamestate.h"

namespace
{
bool bFirstFrame = true;
} // anon namespace
namespace bbk
{
/**
 * \name
 * Static member data definitions
 *///\{
GameStateMgr::GSMStatus GameStateMgr::gsmStatus_ = GSM_CHANGED;
std::vector<GameState*> GameStateMgr::statesVec_;
GameState*              GameStateMgr::pCurrState_ = nullptr;
unsigned                GameStateMgr::prev_ = 0;
unsigned                GameStateMgr::curr_ = 0;
unsigned                GameStateMgr::next_ = 0;

std::vector<GameStateMgr::GameStateContext> GameStateMgr::gsStack_;

int  GameStateMgr::gsStackTopInd_ = -1;
bool GameStateMgr::toPush_   = false;
bool GameStateMgr::toPop_    = false;
bool GameStateMgr::toSwitch_ = false;

int   GameStateMgr::numCycles_   = 0;
float GameStateMgr::timeElapsed_ = 0.0f;

//\}

bool GameStateMgr::Init() {return true;}

void GameStateMgr::Halt()
{
	for (size_t i = 0, size = statesVec_.size(); i < size; ++i)
		delete statesVec_[i];
	statesVec_.clear();
	gsStack_.clear();
}

void GameStateMgr::Update(float deltatime)
{
	if (!::bFirstFrame)
	{
		UpdateStates(deltatime);
		DrawStates();
	}
	else
		::bFirstFrame = false;
	
	if (toPop_)
		ExecutePop();
	if (toPush_)
		ExecutePush();
	if (toSwitch_)
		ExecuteSwitch();
}

unsigned GameStateMgr::RegisterGameState(GameState * const pState)
{
	statesVec_.push_back(pState);
	return (statesVec_.size() - 1);
}

void GameStateMgr::SetInitState(unsigned index)
{
	prev_ = next_ = curr_ = index;
}

void GameStateMgr::SwitchGameState(unsigned index)
{
	// Raise state-change flag
	gsmStatus_ = GSM_CHANGED;
	toSwitch_ = true;
	// Save index of state to change to
	next_ = index;
}

void GameStateMgr::RestartState()
{
	// Raise state-change flag
	gsmStatus_ = GSM_RESTART;
}

void GameStateMgr::EndGame()
{
	// Raise state-change flag
	gsmStatus_ = GSM_END;
}

void GameStateMgr::PushState(unsigned ind, bool updateBelow, bool drawBelow)
{
	if (gsStackTopInd_ < 0)
		SetInitState(ind);
	else
	{
		gsStack_.push_back(GameStateContext(statesVec_[ind], updateBelow, drawBelow));
		toPush_ = true;
	}
}

void GameStateMgr::PopState()
{
	toPop_ = true;
}

GameState* GameStateMgr::GetTopState()
{
	return statesVec_[gsStackTopInd_];
}

GameState* GameStateMgr::GetCurrentState()
{
	return pCurrState_;
}

bool GameStateMgr::OuterCheckPoint()
{
	if (gsmStatus_ == GSM_END)
		return false;

	SetupBaseState();
	return true;
}

bool GameStateMgr::InnerCheckPoint()
{
	if (gsmStatus_ == GSM_STATQUO)
		return true;

	CloseBaseState();
	return false;
}

void GameStateMgr::UpdateStates(float deltatime)
{
	for (int i = gsStackTopInd_; i >= 0; --i)
	{
		// Save pointer to current state
		pCurrState_ = gsStack_[i].pState;
		pCurrState_->Update(deltatime);
		if (!gsStack_[i].updateBelow)
			return;
	}
}

void GameStateMgr::DrawStates()
{
	for (int i = gsStackTopInd_; i >= 0; --i)
	{
		gsStack_[i].pState->Draw();
		if (!gsStack_[i].drawBelow)
			return;
	}
}

void GameStateMgr::SetupBaseState()
{
	if (gsmStatus_ == GSM_RESTART)
		AdjustStateVarsRestart();
	else
	{
		ClearStack();
		pCurrState_ = statesVec_[curr_];
		gsStack_.push_back(GameStateContext(statesVec_[curr_], false, false));
		++gsStackTopInd_;
		//gsStack_[++gsStackTopInd_].pState->Load();
		//pCurrState_ = statesVec_[gsStackTopInd_];
		pCurrState_->Load();
	}

	//statesVec_[gsStackTopInd_]->Init();
	pCurrState_->Init();
	gsmStatus_ = GSM_STATQUO;
	::bFirstFrame = true;
}

void GameStateMgr::CloseBaseState()
{
	// Call cleanup for all states in stack
	for (int i = gsStackTopInd_; i >= 0; --i)
		gsStack_[i].pState->Cleanup();

	// Unload all states but base if restarting
	if (gsmStatus_ == GSM_RESTART)
	{
		for (int i = gsStackTopInd_; i > 0; --i)
			gsStack_[i].pState->Unload();
	}
	else // Unload all states including base
	{
		for (int i = gsStackTopInd_; i >= 0; --i)
			gsStack_[i].pState->Unload();
	}
	AdjustStateVarsEnd();
}

void GameStateMgr::AdjustStateVarsRestart()
{
	curr_ = next_ = prev_;
}

void GameStateMgr::AdjustStateVarsEnd()
{
	prev_ = curr_;
	curr_ = next_;
}

void GameStateMgr::ExecutePush()
{
	++gsStackTopInd_;
	gsStack_[gsStackTopInd_].pState->Load();
	gsStack_[gsStackTopInd_].pState->Init();
	toPush_ = false;
}

void GameStateMgr::ExecutePop()
{
	gsStack_[gsStackTopInd_].pState->Cleanup();
	gsStack_[gsStackTopInd_].pState->Unload();
	gsStack_.erase(gsStack_.begin() + gsStackTopInd_--);
	toPop_ = false;
	
	if (gsStackTopInd_ < 0)
		EndGame();
}

void GameStateMgr::ExecuteSwitch()
{
	gsmStatus_ = GSM_CHANGED;
	toSwitch_ = false;
}

void GameStateMgr::ClearStack()
{
	gsStack_.clear();
	gsStackTopInd_ = -1;
}

GameStateMgr::GameStateMgr() {}
} // namespace bbk
