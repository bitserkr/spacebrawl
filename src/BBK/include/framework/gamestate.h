#ifndef _GAMESTATE_H
#define _GAMESTATE_H

namespace bbk
{
class GameState
{
public:
	GameState() {}
	virtual ~GameState() {}

	virtual bool Load()                  = 0;
	virtual void Init()                  = 0;
	virtual void Update(float deltatime) = 0;
	virtual void Draw()                  = 0;
	virtual void Cleanup()               = 0;
	virtual void Unload()                = 0;
}; // class GameState
} // namespace bbk

#endif /* _GAMESTATE_H */
