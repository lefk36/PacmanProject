#include "Pacman.h"
#include <time.h>
#include <iostream>
#include <sstream>


Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv), _cPacmanSpeed(0.1f),_cPacmanFrameTime(250), _cMunchieFrameTime(500)
{
	// Initialize Member Variables
	_pacman = new Player();
	_pacman->dead = false;
	int i;
	for (i = 0; i < MUNCHIECOUNT; i++) {
		_munchies[i] = new Enemy();
		_munchies[i]->frameCount = 0;
		_munchies[i]->currentFrameTime = 0;
		_munchies[i]->frametime = rand() % (500 + 50);
	}
	_cherry = new Enemy();
	_menu = new Menu();
	// Initialize ghost Character
	for (int i = 0; i < GHOSTCOUNT; i++) {
		_ghosts[i] = new MovingEnemy();
		_ghosts[i]->direction = 0;
		_ghosts[i]->speed = 0.2f;
	}
	// Remaining Variable Initianilizations
	
	_menu->_paused = false;
	_menu->_pKeyDown = false;
	_menu->_start = false;
	_menu->_stKeyDown = false;
	_pacman->direction = 0;
	_pacman->frameTime = 0; 
	_pacman->frame = 0;
	_pacman->speedMultiplier = 1.0f;
	_cherry->frameCount = 0;
	_cherry->currentFrameTime = 0;
	_pop = new SoundEffect();
	

	//Initialise important Game aspects
	Audio::Initialise();
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60);
	Input::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

Pacman::~Pacman()
{
	// Clean up the texture for munchies
	delete _munchies[0]->texture;

	int nCount = 0;
	for (nCount = 0; nCount < MUNCHIECOUNT; nCount++) {
		delete _munchies[nCount]->position;
		delete _munchies[nCount]->sourceRect;
		delete _munchies[nCount];
	}
	delete _munchies;

	// Remaining clean up
	delete _pacman->texture;
	delete _pacman->sourceRect;
	delete _pacman->position;
	delete _pacman;

	delete _menu->_menuBackground;
	delete _menu->_menuRectangle;
	delete _menu->_menuStringPosition;
	delete _menu;

	delete _cherry->texture;
	delete _cherry->position;
	delete _cherry->sourceRect;
	delete _cherry;

	delete _ghosts[0]->texture;


	int gCount = 0;
	for (gCount = 0; gCount < GHOSTCOUNT; gCount++) {
		delete _ghosts[gCount]->sourceRect;
		delete _ghosts[gCount]->position;
	}
	delete _ghosts;
	
	delete _pop;
}

void Pacman::LoadContent()
{
	// Load Pacman
	_pacman->texture = new Texture2D();
	_pacman->texture->Load("Textures/Pacman.tga", false);
	_pacman->position = new Vector2(350.0f, 350.0f);
	_pacman->sourceRect = new Rect(0.0f, 0.0f, 32, 32);

	// Load Ghosts
	Texture2D* ghostTex = new Texture2D();
	ghostTex->Load("Textures/GhostBlue.png", true);
	for (int i = 0; i < GHOSTCOUNT; i++) {
		_ghosts[i]->texture = new Texture2D();
		_ghosts[i]->texture->Load("Textures/GhostBlue.png", true);
		_ghosts[i]->sourceRect = new Rect(0.0f, 0.0f, 20, 20);
		_ghosts[i]->position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
	}
	// Load Munchies
	Texture2D* munchieTex = new Texture2D();
	munchieTex->Load("Textures/Munchie.png",false);
	for (int i = 0; i < MUNCHIECOUNT; i++) {
		_munchies[i]->texture = munchieTex;
		_munchies[i]->sourceRect = new Rect(0.0f, 0.0f, 12, 12);
		_munchies[i]->position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
	}

	// Load cherry
	_cherry->texture = new Texture2D();
	_cherry->texture->Load("Textures/cherries.png",true);
	_cherry->position = new Vector2(300.0f, 560.0f);
	_cherry->sourceRect = new Rect(0.0f, 0.0f, 32, 32);


	// Set Menu Parameters
	_menu->_menuBackground = new Texture2D();
	_menu->_menuBackground->Load("Textures/Transparency.png",false);
	_menu->_menuRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_menu->_menuStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);

	// Load Sounds
	_pop->Load("Sounds/pop.wav");

	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);
}

void Pacman::Update(int elapsedTime) {



	// Gets the current state of the keyboard and mouse
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();
	Input::MouseState* mouseState = Input::Mouse::GetState();

	// Check for start
	CheckStart(keyboardState, Input::Keys::SPACE);
	

	if (_menu->_start) {
		// Calling method for Pause Button
		CheckPaused(keyboardState,Input::Keys::P);

		if (!_menu->_paused) {

			// Called methods
			Input(elapsedTime, keyboardState,mouseState);
			CheckViewportCollision();
			UpdatePacman(elapsedTime);
			for (int i = 0; i < GHOSTCOUNT; i++) {
				UpdateGhost(_ghosts[i], elapsedTime);
			}
			CheckGhostCollisions();
			// Collisions for munchies
			for (int i = 0; i < MUNCHIECOUNT; i++) {
				if (collisionCheckMunchies(_pacman->position->X, _pacman->position->Y, _pacman->sourceRect->Width, _pacman->sourceRect->Height, _munchies[i]->position->X, _munchies[i]->position->Y, _munchies[i]->sourceRect->Width, _munchies[i]->sourceRect->Height)) {
					_munchies[i]->position = new Vector2(-100.0f, -100.0f);
					Audio::Play(_pop);
				}
			}
			// Updates Munchies
			for (int i = 0; i < MUNCHIECOUNT; i++) {
				UpdateMunchies(_munchies[i], elapsedTime);
			}
			UpdateCherry(elapsedTime);
		}
	}
}

void Pacman::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	std::stringstream stream;
	stream << "Pacman X: " << _pacman->position->X << " Y: " << _pacman->position->Y;

	SpriteBatch::BeginDraw(); // Starts Drawing
	// Draws Pacman
	if (!_pacman->dead) {
		SpriteBatch::Draw(_pacman->texture, _pacman->position, _pacman->sourceRect);
	}
	// Draws Ghosts
	for (int i = 0; i < GHOSTCOUNT; i++) {
		SpriteBatch::Draw(_ghosts[i]->texture, _ghosts[i]->position, _ghosts[i]->sourceRect);
	}
	// Draws Munchies
	for (int i = 0; i < MUNCHIECOUNT; i++) {
		SpriteBatch::Draw(_munchies[i]->texture, _munchies[i]->position, _munchies[i]->sourceRect);
	}
	// Draws Cherry
	SpriteBatch::Draw(_cherry->texture, _cherry->position, _cherry->sourceRect);
	// Draws String
	SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);

	// Draws Start Screen
	if (!_menu->_start) {
		std::stringstream menuStream;
		menuStream << "PRESS SPACE TO START";
	SpriteBatch::DrawString(menuStream.str().c_str(),_menu->_menuStringPosition, Color::White);
	}
	// Draws pause menu
	if (_menu->_paused) {
		std::stringstream menuStream;
		menuStream << "PAUSED!";
		SpriteBatch::DrawString(menuStream.str().c_str(), _menu->_menuStringPosition, Color::Red);
	}

	// Draw Game Over
	if (_pacman->dead) {
		std::stringstream menuStream;
		menuStream << "Game Over!";
		SpriteBatch::DrawString(menuStream.str().c_str(),_menu->_menuStringPosition, Color::Red);
	}

	SpriteBatch::EndDraw(); // Ends Drawing
}

// Inputs for Movement
void Pacman::Input(int elapsedTime, Input::KeyboardState* state,Input::MouseState* mouseState) {

	float pacmanSpeed = _cPacmanSpeed * elapsedTime * _pacman->speedMultiplier;

	// Keyboard Movements for pacman
	if (state->IsKeyDown(Input::Keys::D)) {
		_pacman->position->X += pacmanSpeed;
		_pacman->direction = 0;
	}
	else if (state->IsKeyDown(Input::Keys::A)) {
		_pacman->position->X -= pacmanSpeed;
		_pacman->direction = 2;
	}
	else if (state->IsKeyDown(Input::Keys::S)) {
		_pacman->position->Y += pacmanSpeed;
		_pacman->direction = 1;
	}
	else if (state->IsKeyDown(Input::Keys::W)) {
		_pacman->position->Y -= pacmanSpeed;
		_pacman->direction = 3;
	}

	// Speed Multiplier
	if (state->IsKeyDown(Input::Keys::LEFTSHIFT) || state->IsKeyDown(Input::Keys::RIGHTSHIFT)) {
		// Apply multiplier
		_pacman->speedMultiplier = 3.0f;
	}
	else {
		// Reset Multiplier
		_pacman->speedMultiplier = 1.0f;
	}
	// Mouse movements for cherry
	if (mouseState->LeftButton == Input::ButtonState::PRESSED) {
		_cherry->position->X = mouseState->X;
		_cherry->position->Y = mouseState->Y;
	}
}

// Collision Detection 
void Pacman::CheckViewportCollision() {
	// Checks if Pacman is trying to dissappear
	if (_pacman->position->X + _pacman->sourceRect->Width > Graphics::GetViewportWidth()) {
		// Pacman hit right wall teleports left
		_pacman->position->X = 32 - _pacman->sourceRect->Width;
	}
		// Pacman hit left wall teleports right
	else if (_pacman->position->X + _pacman->sourceRect->Width < 32) {
		_pacman->position->X = Graphics::GetViewportWidth() - _pacman->sourceRect->Width;
	}
	// Pacman hit top Wall teleports bottom
	else if (_pacman->position->Y + _pacman->sourceRect->Height > Graphics::GetViewportHeight()) {
		_pacman->position->Y = 32 - _pacman->sourceRect->Height;
	}
	// Pacman hit bottom wall teleport left
	else if (_pacman->position->Y + _pacman->sourceRect->Height < 32) {
		_pacman->position->Y = Graphics::GetViewportHeight() - _pacman->sourceRect->Height;
	}
}

// Pause Button
void Pacman::CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey) {
	if (state->IsKeyDown(Input::Keys::P) && !_menu->_pKeyDown) {
		_menu->_paused = !_menu->_paused;
		_menu->_pKeyDown = true;
	}
	else if (state->IsKeyUp(Input::Keys::P)) {
		_menu->_pKeyDown = false;
	}
}

// Start the game button
void Pacman::CheckStart(Input::KeyboardState* state, Input::Keys startKey) {
	if (state->IsKeyDown(Input::Keys::SPACE) && _menu->_stKeyDown) {
		_menu->_start = true;
		_menu->_stKeyDown = false;
	}
	else if (state->IsKeyUp(Input::Keys::SPACE)) {
		_menu->_stKeyDown = true;
	}
}

// Updates Pacman Animation  
void Pacman::UpdatePacman(int elapsedTime) {

	_pacman->currentFrameTime += elapsedTime;
	_pacman->sourceRect->Y = _pacman->sourceRect->Height * _pacman->direction;
	_pacman->sourceRect->X = _pacman->sourceRect->Width * _pacman->frame;

	if (_pacman->currentFrameTime > _cPacmanFrameTime) {
		_pacman->frame++;

		if (_pacman->frame >= 2)
			_pacman->frame = 0;

		_pacman->currentFrameTime = 0;
	}
}

// Updates Munchie Animation
void Pacman::UpdateMunchies(Enemy*,int elapsedTime) {
	for (int i = 0; i < MUNCHIECOUNT; i++) {
		_munchies[i]->currentFrameTime += elapsedTime;
		_munchies[i]->sourceRect->X = _munchies[i]->sourceRect->Width * _munchies[i]->frameCount;

		if (_munchies[i]->currentFrameTime > _cMunchieFrameTime) {
			_munchies[i]->frameCount++;

			if (_munchies[i]->frameCount >= 2)
				_munchies[i]->frameCount = 0;

				_munchies[i]->currentFrameTime = 0;
		}
	}
}


void Pacman::UpdateCherry(int elapsedTime) {

	_cherry->currentFrameTime += elapsedTime;
	_cherry->sourceRect->X = _cherry->sourceRect->Width * _cherry->frameCount;

	if (_cherry->currentFrameTime > _cMunchieFrameTime) {
		_cherry->frameCount++;

		if (_cherry->frameCount >= 2)
			_cherry->frameCount = 0;

		_cherry->currentFrameTime = 0;
	}
}

// Update Ghost method
void Pacman::UpdateGhost(MovingEnemy* ghost, int elapsedTime) {
	///// X Axis///////////////////////////////
	if (ghost->direction == 0) {
		// Move Right
		ghost->position->X += ghost->speed * elapsedTime;
	}
	else if (ghost->direction == 1) {
		// Moves Left
		ghost->position->X -= ghost->speed * elapsedTime;
	}
	
	// hits Right Edge
	if (ghost->position->X + ghost->sourceRect->X >= Graphics::GetViewportWidth()) {
		// Change Direction
		ghost->direction = 1;
	}
	// hits Left edge
	else if (ghost->position->X <= 0) {
		// Change Direction
		ghost->direction = 0; 
	}

	


	///// Y Axis///////////////////////////////
	if (ghost->direction == 0) {
		// Move top
		ghost->position->Y += ghost->speed * elapsedTime;
	}
	else if (ghost->direction == 1) {
		ghost->position->Y -= ghost->speed * elapsedTime;
	}
	if (ghost->position->Y + ghost->sourceRect->Y >= Graphics::GetViewportHeight()) {
		ghost->direction = 1;
	}
	else if (ghost->position->Y <= 0) {
		ghost->direction = 0;
	}
	
	 

	
}

void Pacman::CheckGhostCollisions() {
	// Local Variables
	
	int bottom1 = _pacman->position->Y + _pacman->sourceRect->Height;
	int bottom2 = 0;
	int left1 = _pacman->position->X;
	int left2 = 0;
	int right1 = _pacman->position->X + _pacman->sourceRect->Width;
	int right2 = 0;
	int top1 = _pacman->position->Y;
	int top2 = 0;

	int i = 0;
	for (i = 0; i < GHOSTCOUNT; i++) {
		// Populate variables with Ghost Data
		bottom2 = _ghosts[i]->position->Y + _ghosts[i]->sourceRect->Height;
		left2 = _ghosts[i]->position->X;
		right2 = _ghosts[i]->position->X + _ghosts[i]->sourceRect->Width;
		top2 = _ghosts[i]->position->Y;

		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2)) {
			_pacman->dead = true;
			i = GHOSTCOUNT;

		}
	}

}

bool Pacman::collisionCheckMunchies(int x1, int y1, int width1, int height1, int x2,int y2,int width2, int height2) {
	int left1 = x1;

	int left2 = x2;

	int right1 = x1 + width1;

	int right2 = x2 + width2;

	int top1 = y1;

	int top2 = y2;

	int bottom1 = y1 + height1;

	int bottom2 = y2 + height2;

	if (bottom1 < top2) 
		return false;
	

	if (top1 > bottom2) 
		return false;
	
	if (right1 < left2) 
		return false;
	
	if (left1 > right2) 
		return false;
	
	return true;

	
}
