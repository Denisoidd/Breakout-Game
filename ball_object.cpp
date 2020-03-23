#include "ball_object.h"

BallObject::BallObject():GameObject(), Radius(12.5f), Stuck(true) {}

BallObject::BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite)
	: GameObject(pos, glm::vec2(radius * 2, radius * 2), sprite, glm::vec3(1.0f), velocity), Radius(radius), Stuck(true) { }

glm::vec2 BallObject::Move(GLfloat dt, GLuint window_width)
{
	//not stuck to player board
	if (!this->Stuck)
	{
		//Move the ball
		this->Position += this->Velocity * dt;

		//Check if outside the window
		if (this->Position.x <= 0.f)
		{
			this->Velocity.x = -(this->Velocity.x);
			this->Position.x = 0.f;
		}
		else if (this->Position.x + this->Size.x >= window_width)
		{
			this->Velocity.x = -this->Velocity.x;
			this->Position.x = window_width - this->Size.x;
		}
		if (this->Position.y <= 0.f)
		{
			this->Velocity.y = -(this->Velocity.y);
			this->Position.y = 0.f;
		}
	}
	return this->Position;
}

//Reset the ball to initial Stuck Position
void BallObject::Reset(glm::vec2 position, glm::vec2 velocity)
{
	this->Position = position;
	this->Velocity = velocity;
	this->Stuck = true;
}

