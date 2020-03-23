#include "Game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "ball_object.h"
#include <map>
#include <string>

using namespace std;


//Intial characteristics

// Game-related State data
SpriteRenderer* Renderer;
GameObject* Player;
BallObject* Ball;


Game::Game(GLuint width, GLuint height)
	: State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{

}

Game::~Game()
{
	delete Renderer;
	delete Player;
}

void Game::Init()
{
	// Load shaders
	ResourceManager::LoadShader("Shader/sprite.vs", "Shader/sprite.frag", nullptr, "sprite");

	// Configure shaders
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width), static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
	
	// Load textures
	ResourceManager::LoadTexture("Texture/paddle.png", true, "paddle");
	ResourceManager::LoadTexture("Texture/background.jpg", GL_FALSE, "background");
	ResourceManager::LoadTexture("Texture/awesome.png", GL_TRUE, "face");
	ResourceManager::LoadTexture("Texture/block.png", GL_FALSE, "block");
	ResourceManager::LoadTexture("Texture/block_solid.png", GL_FALSE, "block_solid");
	
	// Set render-specific controls
	Shader myShader = ResourceManager::GetShader("sprite");
	Renderer = new SpriteRenderer(myShader);

	//Load levels
	GameLevel one;
	one.Load("Levels/one.lvl", this -> Width, this->Height * 0.5);
	GameLevel two;
	two.Load("Levels/two.lvl", this->Width, this->Height * 0.5);
	GameLevel three;
	two.Load("Levels/three.lvl", this->Width, this->Height * 0.5);
	GameLevel four;
	two.Load("Levels/four.lvl", this->Width, this->Height * 0.5);
	this->Levels.push_back(one);
	this->Levels.push_back(two);
	this->Levels.push_back(three);
	this->Levels.push_back(four);

	this->Level = 0;

	//Load player
	glm::vec2 playerPos = glm::vec2(
		this->Width / 2 - PLAYER_SIZE.x / 2,
		this->Height - PLAYER_SIZE.y
	);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

	//Load ball
	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS / 2);
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY,
		ResourceManager::GetTexture("face"));
}

void Game::Update(GLfloat dt)
{
	//Move ball
	Ball->Move(dt, this->Width);

	//Collisions check
	this->DoCollisions();

	//Reset
	if (Ball->Position.y >= this->Height)
	{
		ResetLevel();
		ResetPlayer();
	}
}


void Game::ProcessInput(GLfloat dt)
{
	if (this->State == GAME_ACTIVE)
	{
		GLfloat velocity = PLAYER_VELOCITY * dt;
		
		//Move board
		if (this->Keys[GLFW_KEY_A])
		{
			if (Player->Position.x >= 0)
			{
				Player->Position.x -= velocity;
				if (Ball->Stuck)
					Ball->Position.x -= velocity;
			}
		}
		if (this->Keys[GLFW_KEY_D])
		{
			if (Player->Position.x <= this->Width - Player->Size.x)
			{
				Player->Position.x += velocity;
				if (Ball->Stuck)
					Ball->Position.x += velocity;
			}
		}
		if (this->Keys[GLFW_KEY_SPACE])
			Ball->Stuck = false;
	}
}

void Game::Render()
{
	if (this->State == GAME_ACTIVE)
	{
		// Draw background
		Texture2D myText = ResourceManager::GetTexture("background");
		Renderer->DrawSprite(myText,
			glm::vec2(0, 0), glm::vec2(this->Width, this->Height), 0.0f
		);
		// Draw level
		this->Levels[this->Level].Draw(*Renderer);
		// Draw player
		Player->Draw(*Renderer);

		//Draw ball
		Ball->Draw(*Renderer);
	}
}

Direction VectorDirection(glm::vec2 target)
{
	glm::vec2 compass[] =
	{
		glm::vec2(0.f, 1.f),
		glm::vec2(1.f, 0.f),
		glm::vec2(0.f, -1.f),
		glm::vec2(-1.f, 0.f)
	};
	GLfloat max = 0.f;
	GLuint best_match = -1;
	for (GLuint i = 0; i < 4; i++)
	{
		GLfloat dot_product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_product > max)
		{
			max = dot_product;
			best_match = i;
		}
	}
	return (Direction)best_match;
}

Collision CheckCollision(BallObject& one, GameObject& two)
{
	//Get center point of the ball
	glm::vec2 center(one.Position + one.Radius);

	//Calculate AABB info
	glm::vec2 aabb_half_extents(two.Size.x / 2, two.Size.y / 2);
	glm::vec2 aabb_center(
		two.Position.x + aabb_half_extents.x,
		two.Position.y + aabb_half_extents.y
	);

	//Difference between two centers
	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);

	//Add clamped value to AABB_center and get the the point closest to circle
	glm::vec2 closest = aabb_center + clamped;

	//Distance between them
	difference = closest - center;
	
	if (glm::length(difference) <= one.Radius)
	{
		Direction direction = VectorDirection(difference);
		return std::make_tuple(GL_TRUE, direction, difference);
	}
	else
		return std::make_tuple(GL_FALSE, UP, glm::vec2(0, 0));
}

void Game::DoCollisions()
{
	for (GameObject& box : this->Levels[this->Level].Bricks)
	{
		if (!box.Destroyed)
		{
			Collision collision = CheckCollision(*Ball, box);
			if (std::get<0>(collision))
			{
				//Destroy the block if not solid
				if (!box.IsSolid)
					box.Destroyed = GL_TRUE;

				//Collision
				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);
				//Horizontal collision
				if (dir == LEFT || dir == RIGHT)
				{
					//Reverse the speed
					Ball->Velocity.x = -(Ball->Velocity.x);

					//Replace after penetration
					GLfloat penetration = Ball->Radius - std::abs(diff_vector.x);
					if (dir == LEFT)
						//Move the ball to the right
						Ball->Position.x += penetration;
					else
						//Move the ball to the left
						Ball->Position.x -= penetration;
				}
				//Vertical collision
				else
				{
					Ball->Velocity.y = -(Ball->Velocity.y);

					GLfloat penetration = Ball->Radius - std::abs(diff_vector.y);
					if (dir == UP)
					{
						Ball->Position.y -= penetration;
					}
					else
					{
						Ball->Position.y += penetration;
					}
				}
			}
		}
	}
	Collision result = CheckCollision(*Ball, *Player);
	if (!Ball->Stuck && std::get<0>(result))
	{
		//Where ball hit the board
		GLfloat centerBoard = Player->Position.x + Player->Size.x / 2;
		GLfloat distance = (Ball->Position.x + Ball->Radius) - centerBoard;
		GLfloat percentage = distance / (Player->Size.x / 2);

		//Move the ball
		GLfloat strength = 2.f;
		glm::vec2 oldVelocity = Ball->Velocity;
		Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
		Ball->Velocity.y = -abs(Ball->Velocity.y);
		Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
	}
}

void Game::ResetLevel()
{
	if (this->Level == 0) this->Levels[0].Load("Levels/one.lvl", this->Width, this->Height * 0.5f);
	else if (this->Level == 1)
		this->Levels[1].Load("Levels/two.lvl", this->Width, this->Height * 0.5f);
	else if (this->Level == 2)
		this->Levels[2].Load("Levels/three.lvl", this->Width, this->Height * 0.5f);
	else if (this->Level == 3)
		this->Levels[3].Load("Levels/four.lvl", this->Width, this->Height * 0.5f);
}

void Game::ResetPlayer()
{
	Player->Size = PLAYER_SIZE;
	Player->Position = glm::vec2(this->Width / 2 - PLAYER_SIZE.x / 2, this->Height - PLAYER_SIZE.y);
	Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -(BALL_RADIUS * 2)), INITIAL_BALL_VELOCITY);
}
	