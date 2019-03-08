#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Player.h"
#include "Game.h"


#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 96
#define FALL_STEP 6


enum PlayerAnims
{
	STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT, JUMP_LEFT, JUMP_RIGHT,
	FALL_LEFT, FALL_RIGHT, GRAVITY_LEFT, GRAVITY_RIGHT
};

void Player::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram)
{
	if (Game::instance().gravity) {
		g = 1;
		currentSprite = 0;
	}
	else {
		g = -1;
		currentSprite = 1;
	}
	bJumping = false;
	left = true;
	tileMapDispl = tileMapPos;

	int xcols = 8;
	int ycols = 6;

	spritesheet[0].loadFromFile("images/HarukoSpritesUp.png", TEXTURE_PIXEL_FORMAT_RGBA);
	spritesheet[1].loadFromFile("images/HarukoSpritesDown.png", TEXTURE_PIXEL_FORMAT_RGBA);
	for (int k = 0; k < 2; k++) {
		sprite[k] = Sprite::createSprite(glm::ivec2(64, 64), glm::vec2(1.f / xcols, 1.f / ycols), &spritesheet[k], &shaderProgram);
		sprite[k]->setNumberAnimations(10);

		sprite[k]->setAnimationSpeed(STAND_LEFT, 6);
		for (int i = 0; i < 4; i++)
		{
			sprite[k]->addKeyframe(STAND_LEFT, glm::vec2(float(i) / xcols, 0.f));
		}
		sprite[k]->addKeyframe(STAND_LEFT, glm::vec2(2.f / xcols, 0.f));
		sprite[k]->addKeyframe(STAND_LEFT, glm::vec2(1.f / xcols, 0.f));

		sprite[k]->setAnimationSpeed(STAND_RIGHT, 6);
		for (int i = 4; i < 8; i++)
		{
			sprite[k]->addKeyframe(STAND_RIGHT, glm::vec2(float(i) / xcols, 0.f));
		}
		sprite[k]->addKeyframe(STAND_RIGHT, glm::vec2(6.f / xcols, 0.f));
		sprite[k]->addKeyframe(STAND_RIGHT, glm::vec2(5.f / xcols, 0.f));

		sprite[k]->setAnimationSpeed(MOVE_LEFT, 14);
		for (int i = 0; i < 8; i++)
		{
			sprite[k]->addKeyframe(MOVE_LEFT, glm::vec2(float(i) / xcols, 1.f / ycols));
		}

		sprite[k]->setAnimationSpeed(MOVE_RIGHT, 14);
		for (int i = 0; i < 8; i++)
		{
			sprite[k]->addKeyframe(MOVE_RIGHT, glm::vec2(float(i) / xcols, 2.f / ycols));
		}

		sprite[k]->setAnimationSpeed(JUMP_LEFT, 8);
		for (int i = 0; i < 8; i++)
		{
			sprite[k]->addKeyframe(JUMP_LEFT, glm::vec2(float(i) / xcols, 3.f / ycols));
		}

		sprite[k]->setAnimationSpeed(JUMP_RIGHT, 8);
		for (int i = 0; i < 8; i++)
		{
			sprite[k]->addKeyframe(JUMP_RIGHT, glm::vec2(float(i) / xcols, 4.f / ycols));
		}

		sprite[k]->setAnimationSpeed(FALL_LEFT, 4);
		sprite[k]->addKeyframe(FALL_LEFT, glm::vec2(5.f / xcols, 3.f / ycols));
		sprite[k]->addKeyframe(FALL_LEFT, glm::vec2(6.f / xcols, 3.f / ycols));
		sprite[k]->addKeyframe(FALL_LEFT, glm::vec2(0.f / xcols, 3.f / ycols));
		sprite[k]->addKeyframe(FALL_LEFT, glm::vec2(1.f / xcols, 3.f / ycols));

		sprite[k]->setAnimationSpeed(FALL_RIGHT, 4);
		sprite[k]->addKeyframe(FALL_RIGHT, glm::vec2(5.f / xcols, 4.f / ycols));
		sprite[k]->addKeyframe(FALL_RIGHT, glm::vec2(6.f / xcols, 4.f / ycols));
		sprite[k]->addKeyframe(FALL_RIGHT, glm::vec2(0.f / xcols, 4.f / ycols));
		sprite[k]->addKeyframe(FALL_RIGHT, glm::vec2(1.f / xcols, 4.f / ycols));

		sprite[k]->setAnimationSpeed(GRAVITY_LEFT, 8);
		for (int i = 0; i < 4; i++)
		{
			sprite[k]->addKeyframe(GRAVITY_LEFT, glm::vec2(float(i) / xcols, 5.f / ycols));
		}

		sprite[k]->setAnimationSpeed(GRAVITY_RIGHT, 8);
		for (int i = 4; i < 8; i++)
		{
			sprite[k]->addKeyframe(GRAVITY_RIGHT, glm::vec2(float(i) / xcols, 5.f / ycols));
		}

		sprite[k]->changeAnimation(0);
		sprite[k]->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
	}
}

void Player::update(int deltaTime)
{
	if (Game::instance().gravity) {
		g = 1;
		currentSprite = 0;
	}
	else {
		g = -1;
		currentSprite = 1;
	}

	sprite[currentSprite]->update(deltaTime);
	if (Game::instance().getSpecialKey(GLUT_KEY_LEFT))
	{
		// Si l'animacio no era la que toca, canvi animacio
		if (Game::instance().ground) {
			if (sprite[currentSprite]->animation() != MOVE_LEFT)
				sprite[currentSprite]->changeAnimation(MOVE_LEFT);
		}
		// Si no hi ha colisio, ens movem
		posPlayer.x -= 2;
		map->collisionMoveLeft(posPlayer, sprite[currentSprite]->size, &posPlayer.x);
		left = true;
	}
	else if (Game::instance().getSpecialKey(GLUT_KEY_RIGHT))
	{
		if (Game::instance().ground) {
			if (sprite[currentSprite]->animation() != MOVE_RIGHT)
				sprite[currentSprite]->changeAnimation(MOVE_RIGHT);
		}
		posPlayer.x += 2;
		map->collisionMoveRight(posPlayer, sprite[currentSprite]->size, &posPlayer.x);
		left = false;
	}
	else if (left && Game::instance().ground && sprite[currentSprite]->animation() != STAND_LEFT)
		sprite[currentSprite]->changeAnimation(STAND_LEFT);
	else if (!left && Game::instance().ground && sprite[currentSprite]->animation() != STAND_RIGHT)
		sprite[currentSprite]->changeAnimation(STAND_RIGHT);

	posPlayer.y += g * FALL_STEP;
	// Supossem que no esta tocant ground, false.
	Game::instance().ground = false;
	// Si esta tocant ground, true.
	if (Game::instance().gravity) {
		if (map->collisionMoveDown(posPlayer, sprite[currentSprite]->size, &posPlayer.y)) {
			Game::instance().ground = true;
		}
	}
	else {
		if (map->collisionMoveUp(posPlayer, sprite[currentSprite]->size, &posPlayer.y)) {
			Game::instance().ground = true;
		}
	}

	if (!Game::instance().ground) {
		if (left && sprite[currentSprite]->animation() != FALL_LEFT)
			sprite[currentSprite]->changeAnimation(FALL_LEFT);
		else if (!left && sprite[currentSprite]->animation() != FALL_RIGHT)
			sprite[currentSprite]->changeAnimation(FALL_RIGHT);
	}

	//if (bJumping)
	//{
	//	if (sprite[currentSprite]->animation() != JUMP_LEFT)
	//		sprite[currentSprite]->changeAnimation(JUMP_LEFT);
	//	jumpAngle += JUMP_ANGLE_STEP;
	//	if (jumpAngle == 180)
	//	{
	//		bJumping = false;
	//		posPlayer.y = startY;
	//	}
	//	else
	//	{
	//		posPlayer.y = g * int(startY - 96 * sin(3.14159f * jumpAngle / 180.f));
	//		if (map->collisionMoveUp(posPlayer, glm::ivec2(64, 64), &posPlayer.y))
	//			bJumping = false;
	//		else if (jumpAngle > 90)
	//			bJumping = !map->collisionMoveDown(posPlayer, glm::ivec2(64, 64), &posPlayer.y);
	//	}
	//}
	//else
	//{
	//	posPlayer.y += g * FALL_STEP;
	//	if (map->collisionMoveDown(posPlayer, glm::ivec2(64, 64), &posPlayer.y))
	//	{
	//		if (Game::instance().getSpecialKey(GLUT_KEY_UP))
	//		{
	//			bJumping = true;
	//			jumpAngle = 0;
	//			startY = posPlayer.y;
	//		}
	//	}
	//	/*if (map->collisionMoveUp(posPlayer, glm::ivec2(64, 64), &posPlayer.y))
	//		bJumping = false;*/
	//}

	sprite[currentSprite]->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}

void Player::render()
{
	sprite[currentSprite]->render();
}

void Player::setTileMap(TileMap *tileMap)
{
	map = tileMap;
}

void Player::setPosition(const glm::vec2 &pos)
{
	posPlayer = pos;
	sprite[currentSprite]->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}




