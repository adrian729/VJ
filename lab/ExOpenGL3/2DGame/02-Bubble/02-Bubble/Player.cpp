#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Player.h"
#include "Game.h"


#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 96
#define FALL_STEP 4

int oldG = 1;

enum PlayerAnims
{
	STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT, JUMP_LEFT, JUMP_RIGHT
};


void Player::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram)
{
	if (Game::instance().gravity) g = 1;
	else g = -1;
	bJumping = false;
	int xcols = 8;
	int ycols = 5;
	spritesheet.loadFromFile("images/HarukoSprites-prova.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(glm::ivec2(64, 64), glm::vec2(1.f / xcols, 1.f / ycols), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(6);

	sprite->setAnimationSpeed(STAND_LEFT, 6);
	for (int i = 0; i < 4; i++)
	{
		sprite->addKeyframe(STAND_LEFT, glm::vec2(float(i) / xcols, 0.f));
	}
	sprite->addKeyframe(STAND_LEFT, glm::vec2(2.f / xcols, 0.f));
	sprite->addKeyframe(STAND_LEFT, glm::vec2(1.f / xcols, 0.f));

	sprite->setAnimationSpeed(STAND_RIGHT, 8);
	for (int i = 4; i < 8; i++)
	{
		sprite->addKeyframe(STAND_RIGHT, glm::vec2(float(i) / xcols, 0.f));
	}
	sprite->addKeyframe(STAND_RIGHT, glm::vec2(6.f / xcols, 0.f));
	sprite->addKeyframe(STAND_RIGHT, glm::vec2(5.f / xcols, 0.f));

	sprite->setAnimationSpeed(MOVE_LEFT, 16);
	for (int i = 0; i < 8; i++)
	{
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(float(i) / xcols, 1.f / ycols));
	}

	sprite->setAnimationSpeed(MOVE_RIGHT, 16);
	for (int i = 0; i < 8; i++)
	{
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(float(i) / xcols, 2.f / ycols));
	}

	sprite->setAnimationSpeed(JUMP_LEFT, 8);
	for (int i = 0; i < 8; i++)
	{
		sprite->addKeyframe(JUMP_LEFT, glm::vec2(float(i) / xcols, 3.f / ycols));
	}
	
	sprite->changeAnimation(0);
	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}

void Player::update(int deltaTime)
{
	if (Game::instance().gravity) g = 1;
	else g = -1;
	if (g != oldG) {
		cout << "G " << g << endl;
		oldG = g;
	}

	sprite->update(deltaTime);
	if (Game::instance().getSpecialKey(GLUT_KEY_LEFT))
	{
		// Si l'animacio no era la que toca, canvi animacio
		if (sprite->animation() != MOVE_LEFT && !bJumping)
			sprite->changeAnimation(MOVE_LEFT);
		// Si no hi ha colisio, ens movem
		if (!map->collisionMoveLeft(posPlayer, glm::ivec2(64, 64)))
		{
			posPlayer.x -= 2;
		}
	}
	else if (Game::instance().getSpecialKey(GLUT_KEY_RIGHT))
	{
		// Si l'animacio no era la que toca, canvi animacio
		if (sprite->animation() != MOVE_RIGHT && !bJumping)
			sprite->changeAnimation(MOVE_RIGHT);
		// Si no hi ha colisio, ens movem
		if (!map->collisionMoveRight(posPlayer, glm::ivec2(64, 64)))
		{
			posPlayer.x += 2;
		}
	}
	else
	{
		if (sprite->animation() == MOVE_LEFT || sprite->animation() == JUMP_LEFT)
			sprite->changeAnimation(STAND_LEFT);
		else if (sprite->animation() == MOVE_RIGHT || sprite->animation() == JUMP_RIGHT)
			sprite->changeAnimation(STAND_RIGHT);
	}

	if (bJumping)
	{
		if (sprite->animation() != JUMP_LEFT)
			sprite->changeAnimation(JUMP_LEFT);
		jumpAngle += JUMP_ANGLE_STEP;
		if (jumpAngle == 180)
		{
			bJumping = false;
			posPlayer.y = startY;
		}
		else
		{
			posPlayer.y = g * int(startY - 96 * sin(3.14159f * jumpAngle / 180.f));
			if (map->collisionMoveUp(posPlayer, glm::ivec2(64, 64), &posPlayer.y))
				bJumping = false;
			else if (jumpAngle > 90)
				bJumping = !map->collisionMoveDown(posPlayer, glm::ivec2(64, 64), &posPlayer.y);
		}
	}
	else
	{
		posPlayer.y += g * FALL_STEP;
		if (map->collisionMoveDown(posPlayer, glm::ivec2(64, 64), &posPlayer.y))
		{
			if (Game::instance().getSpecialKey(GLUT_KEY_UP))
			{
				bJumping = true;
				jumpAngle = 0;
				startY = posPlayer.y;
			}
		}
		if (map->collisionMoveUp(posPlayer, glm::ivec2(64, 64), &posPlayer.y))
			bJumping = false;
	}

	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}

void Player::render()
{
	sprite->render();
}

void Player::setTileMap(TileMap *tileMap)
{
	map = tileMap;
}

void Player::setPosition(const glm::vec2 &pos)
{
	posPlayer = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}




