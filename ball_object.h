#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "sprite_renderer.h"
#include "game_object.h"
class BallObject :
	public GameObject
{
public:
	//Ball state
	GLfloat Radius;
	GLboolean Stuck;

	//Constructor
	BallObject();
	BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite);
	
	//Move
	glm::vec2 Move(GLfloat dt, GLuint window_width);

	//Reset
	void Reset(glm::vec2 position, glm::vec2 velocity);
};

