#pragma once
#define MUNCHIECOUNT 50
#define GHOSTCOUNT 4
// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
	#ifndef _DEBUG
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif

// Just need to include main header file
#include "S2D/S2D.h"

// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;

// Declares the Pacman class which inherits from the Game class.
// This allows us to overload the Game class methods to help us
// load content, draw and update our game.


// Definitions for Enemy and munhies
struct Enemy {
	int frameCount;
	Rect* sourceRect;
	Texture2D* texture;
	Vector2* position;
	int currentFrameTime;
	int frametime;
};

// Definition for ghosts
struct MovingEnemy {
	Vector2* position;
	Texture2D* texture;
	Rect* sourceRect;
	int direction;
	float speed;
};

// Definitions for Player
struct Player {
	bool dead;
	Vector2* position;
	Rect* sourceRect;
	Texture2D* texture;
	int direction;
	int frame;
	int frameTime;
	int currentFrameTime;
	float speedMultiplier;
};

// Definitions for MENU
struct Menu {
	Texture2D* _menuBackground;
	Rect* _menuRectangle;
	Vector2* _menuStringPosition;
	bool _paused;
	bool _pKeyDown;
	bool _start;
	bool _stKeyDown;
};

// Class Definition
class Pacman : public Game
{
private:
	// Data to represent Pacman
	Player* _pacman;
	const float _cPacmanSpeed;
	const int _cPacmanFrameTime;

	// Data to represent Ghosts
	MovingEnemy* _ghosts[GHOSTCOUNT];

	// Data to represent Munchie
	Enemy* _munchies[MUNCHIECOUNT];
	const int _cMunchieFrameTime;
	
	// Data to represent Cherry
	Enemy* _cherry;

	// Data for Menu
	Menu* _menu;

	// Position for String
	Vector2* _stringPosition;

	// Data for sound
	SoundEffect* _pop;

private:
	// Input Methods
	void Input(int elapsedTime, Input::KeyboardState* state,Input::MouseState* mouseState);

	// Check Methods
	void CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey);
	void CheckViewportCollision();
	void CheckStart(Input::KeyboardState* state, Input::Keys startKey);
	void CheckGhostCollisions();
	bool collisionCheckMunchies(int x1,int y1,int width1, int height1,int x2,int y2,int width2,int height2);

	// Update methods
	void UpdatePacman(int elapsedTime);
	void UpdateMunchies(Enemy*,int elapsedTime);
	void UpdateCherry(int elapsedTime);
	void UpdateGhost(MovingEnemy*, int elapsedTime);

public:
	/// <summary> Constructs the Pacman class. </summary>
	Pacman(int argc, char* argv[]);

	/// <summary> Destroys any data associated with Pacman class. </summary>
	virtual ~Pacman();

	/// <summary> All content should be loaded in this method. </summary>
	void virtual LoadContent();

	/// <summary> Called every frame - update game logic here. </summary>
	void virtual Update(int elapsedTime);

	/// <summary> Called every frame - draw game here. </summary>
	void virtual Draw(int elapsedTime);
};