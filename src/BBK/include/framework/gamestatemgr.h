#ifndef _GAMESTATEMGR_H
#define _GAMESTATEMGR_H

#include <vector>
#include "compiler.h"

namespace bbk
{
class GameState;

/**
 * \class GameStateMgr
 * \brief GameState Manager
 */
class GameStateMgr
{
public:
	static bool Init();
	static void Halt();
	static void Update(float deltatime);

	/**
	 * \name
	 * GameState-registry and GameState-stack methods.
	 *///\{
	/// Gives the GSM a gamestate. GSM will handle its dealloc
	static unsigned RegisterGameState(GameState * const pState);
	/// Sets first gamestate to run
	static void SetInitState(unsigned index);
	/// Clears gamestate stack and pushes arg state to stack
	static void SwitchGameState(unsigned index);
	/// Restarts current state
	static void RestartState();
	/// Exits current state and shuts down game engine
	static void EndGame();

	/// Pushes gamestate onto stack. If stack is empty, operation is the same as calling SetGameState
	static void PushState(unsigned index, bool updateBelow, bool drawBelow);
	/// Removes gamestate from stack. If operation leaves stack empty, same as calling EndGame
	static void PopState();
	//\}
	
	/**
	 * \name
	 * Accessors
	 *///\{
	/// Gets the gamestate at the top of the stack
	static GameState* GetTopState();
	/// Returns index of gamestate currently being updated. If param is a valid pointer to pointer, it will store ptr to state
	static GameState* GetCurrentState();
	//\}

	/**
	 * \name
	 * Methods to control gameloop.
	 *///\{
	/// Returns whether to continue running gamestates (the application)
	static bool OuterCheckPoint();
	/// Returns whether to continue running inner, main game-frame loop
	static bool InnerCheckPoint();
	//\}

private:
	/**
	 * \name
	 * GameState registry and associated helper functions.
	 *///\{
	enum GSMStatus         {GSM_STATQUO, GSM_CHANGED, GSM_RESTART, GSM_END};
	static GSMStatus               gsmStatus_;  ///< Status of gamestate controller
	static std::vector<GameState*> statesVec_;  ///< Registered gamestates
	static GameState*              pCurrState_; ///< Pointer to current state
	static unsigned                prev_;       ///< Index of previous state
	static unsigned                curr_;       ///< Index of current state
	static unsigned                next_;       ///< Index of next state

	static void UpdateStates(float deltatime);
	static void DrawStates();
	static void SetupBaseState();
	static void CloseBaseState();
	static void AdjustStateVarsRestart();
	static void AdjustStateVarsEnd();
	//\}
	
	/**
	 * \name
	 * GameState stack members and helper functions
	 *///\{
	struct GameStateContext
	{
		GameStateContext(GameState *ptr, bool updateB, bool drawB) :
			pState(ptr), updateBelow(updateB), drawBelow(drawB) {}
		
		GameState* pState;
		bool       updateBelow;
		bool       drawBelow;
	}; // struct GameStateContext
	
	static std::vector<GameStateContext> gsStack_;       ///< Gamestate stack
	static int                           gsStackTopInd_; ///< Index to topmost gamestate in stack
	static bool                          toPush_;
	static bool                          toPop_;
	static bool                          toSwitch_;
	
	static void ExecutePush();
	static void ExecutePop();
	static void ExecuteSwitch();
	static void ClearStack();
	//\}

	/**
	 * \name
	 * Gameloop
	 *///\{
	static int   numCycles_;
	static float timeElapsed_;
	//\}

	GameStateMgr();
}; // class GameStateMgr
} // namespace bbk

#endif /* _GAMESTATEMGR_H */
