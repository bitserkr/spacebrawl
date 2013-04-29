#include "bbk.h"
#include "gamestates/Sandbox.h"
#include <crtdbg.h>

int main(int, char *[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); // Memleak dump
	
	bbk::InitFramework();

	bbk::GameStateMgr::RegisterGameState(new Sandbox);
	bbk::GameStateMgr::SetInitState(0);

	bbk::appwindow::OpenWindow(1024, 768, bbk::appwindow::OPENGL, false);

	bbk::RunFramework();

	return 0;
}
