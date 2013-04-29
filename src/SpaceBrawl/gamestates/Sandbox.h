#ifndef _SANDBOX_H
#define _SANDBOX_H

#include "framework/gamestate.h"

class Sandbox : public bbk::GameState
{
public:
	virtual bool Load();
	virtual void Init();
	virtual void Update(float deltatime);
	virtual void Draw();
	virtual void Cleanup();
	virtual void Unload();
}; // class Sandbox

#endif /* _SANDBOX_H */
