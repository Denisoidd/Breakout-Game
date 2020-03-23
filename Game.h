#ifndef GAME_H
#define GAME_H
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "game_level.h"

//Direction of aabb collision
enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

// Represents the current state of the game
enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

//Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(100, 20);

//Initial velocity of the player paddle
const GLfloat PLAYER_VELOCITY(500.f);

//Initial velocity of the ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.f, -350.f);

//Radius of the ball
const GLfloat BALL_RADIUS = 12.5f;

//Tuple for directions
typedef std::tuple<GLboolean, Direction, glm::vec2> Collision;

class Game
{
public:
	// Game state
	GameState              State;
	GLboolean              Keys[1024];
	GLuint                 Width, Height;
	std::vector<GameLevel> Levels;
	GLuint				   Level;
	// Constructor/Destructor
	Game(GLuint width, GLuint height);
	~Game();
	// Initialize game state (load all shaders/textures/levels)
	void Init();
	// GameLoop
	void ProcessInput(GLfloat dt);
	void Update(GLfloat dt);
	void Render();
	void DoCollisions();
	//Reset
	void ResetLevel();
	void ResetPlayer();
};

#endif