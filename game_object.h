#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "sprite_renderer.h"

class GameObject
{
public:
	//Object state
	glm::vec2 Position, Size, Velocity;
	glm::vec3 Color;
	GLfloat Rotation;
	GLboolean IsSolid;
	GLboolean Destroyed;

	//Render State
	Texture2D Sprite;

	//Constructor
	GameObject();
	GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.f), glm::vec2 velocity = glm::vec2(0.f, 0.f));

	//Draw sprite
	virtual void Draw(SpriteRenderer& renderer);

};

