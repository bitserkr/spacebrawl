#include "bbk.h"

bool bUpdateLinear = true;
bool bUpdateAngular = true;

namespace
{
void UpdateInput();

const float frametime = 1000.0f / 60.0f;
} // anon namespace

namespace bbk
{
bool InitFramework()
{
	if (!bbk::InitPlatform())
		return false;
	if (!bbk::gfx::Init())
		return false;

	return true;
}

void RunFramework()
{
	while (bbk::GameStateMgr::OuterCheckPoint()) // Application keeps running within this loop
	{
		uint32_t ts_frameStart = 0;
		uint32_t ts_prevFrame  = 0;

		while (bbk::GameStateMgr::InnerCheckPoint()) // Main loop (game frame)
		{
			ts_frameStart      = bbk::clock::GetTicks();
			uint32_t deltatime = ts_frameStart - ts_prevFrame;
			ts_prevFrame       = ts_frameStart;

			::UpdateInput();

			bbk::GameStateMgr::Update(static_cast<float>(deltatime) * 0.001f);

			bbk::gfx::Render();
			bbk::appwindow::SwapFramebuffers();
			
			/*while ((bbk::SysClock::GetTicks() - ts_frameStart) < ::frametime)
				;*/
		}
	}

	bbk::GameStateMgr::Halt();
	bbk::gfx::Halt();
	bbk::HaltPlatform();
}
} // namespace bbk

namespace
{
void UpdateInput()
{
	// Clear status of input devices prior to polling for this frame
	bbk::mouse::ClearStatus();

	// Poll for system events
	bbk::pollster::Poll();
	const std::vector<bbk::SysEvent>& evVec = bbk::pollster::GetEventsVec();

	// Sort and handle events
	for (size_t i = 0, size = evVec.size(); i < size; ++i)
	{
		switch (evVec[i].type)
		{
		case bbk::SYS_KB_KEYDOWN:
		case bbk::SYS_KB_KEYUP:
			bbk::keyboard::HandleEvent(evVec[i]);
			break;

		case bbk::SYS_MOUSE_MOVE:
		case bbk::SYS_MOUSE_BDOWN:
		case bbk::SYS_MOUSE_BUP:
			bbk::mouse::HandleEvent(evVec[i]);
			break;

		case bbk::SYS_APP_QUIT:
			bbk::GameStateMgr::EndGame();
			break;
		}
	}

	bbk::keyboard::Update();
	bbk::mouse::Update();
}
} // anon namespace
