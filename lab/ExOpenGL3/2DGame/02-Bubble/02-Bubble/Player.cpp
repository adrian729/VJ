#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Player.h"
#include "Game.h"


#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 96
#define FALL_STEP 8
#define MOVEMENT_STEP 3
#define GRAVITY_STEP 16


enum PlayerAnims {
	STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT, JUMP_LEFT, JUMP_RIGHT,
	FALL_LEFT, FALL_RIGHT, GRAVITY_LEFT, GRAVITY_RIGHT
};

void Player::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram) {
	if (Game::instance().gravity) {
		g = 1;
		currentSpriteSheet = 0;
	}
	else {
		g = -1;
		currentSpriteSheet = 1;
	}
	gravityStep = 0;
	jumping = false;
	left = true;
	tileMapDispl = tileMapPos;

	int xcols = 8;
	int ycols = 6;

	spritesheet[0].loadFromFile("images/HarukoSpritesUp.png", TEXTURE_PIXEL_FORMAT_RGBA);
	spritesheet[1].loadFromFile("images/HarukoSpritesDown.png", TEXTURE_PIXEL_FORMAT_RGBA);
	//spritesheet[0].loadFromFile("images/HarukoSpritesUp-test.png", TEXTURE_PIXEL_FORMAT_RGBA);
	//spritesheet[1].loadFromFile("images/HarukoSpritesDown-test.png", TEXTURE_PIXEL_FORMAT_RGBA);

	startColision = glm::ivec2(14, 0);

	for (int k = 0; k < 2; k++) {
		sprite[k] = Sprite::createSprite(glm::ivec2(64, 64), glm::vec2(1.f / xcols, 1.f / ycols), &spritesheet[k], &shaderProgram);
		sprite[k]->setNumberAnimations(10);

		sprite[k]->setAnimationSpeed(STAND_LEFT, 6);
		for (int i = 0; i < 4; i++) {
			sprite[k]->addKeyframe(STAND_LEFT, glm::vec2(float(i) / xcols, 0.f));
		}
		sprite[k]->addKeyframe(STAND_LEFT, glm::vec2(2.f / xcols, 0.f));
		sprite[k]->addKeyframe(STAND_LEFT, glm::vec2(1.f / xcols, 0.f));

		sprite[k]->setAnimationSpeed(STAND_RIGHT, 6);
		for (int i = 4; i < 8; i++) {
			sprite[k]->addKeyframe(STAND_RIGHT, glm::vec2(float(i) / xcols, 0.f));
		}
		sprite[k]->addKeyframe(STAND_RIGHT, glm::vec2(6.f / xcols, 0.f));
		sprite[k]->addKeyframe(STAND_RIGHT, glm::vec2(5.f / xcols, 0.f));

		sprite[k]->setAnimationSpeed(MOVE_LEFT, 16);
		for (int i = 0; i < 8; i++) {
			sprite[k]->addKeyframe(MOVE_LEFT, glm::vec2(float(i) / xcols, 1.f / ycols));
		}

		sprite[k]->setAnimationSpeed(MOVE_RIGHT, 16);
		for (int i = 0; i < 8; i++) {
			sprite[k]->addKeyframe(MOVE_RIGHT, glm::vec2(float(i) / xcols, 2.f / ycols));
		}

		sprite[k]->setAnimationSpeed(JUMP_LEFT, 8);
		for (int i = 0; i < 8; i++) {
			sprite[k]->addKeyframe(JUMP_LEFT, glm::vec2(float(i) / xcols, 3.f / ycols));
		}

		sprite[k]->setAnimationSpeed(JUMP_RIGHT, 8);
		for (int i = 0; i < 8; i++) {
			sprite[k]->addKeyframe(JUMP_RIGHT, glm::vec2(float(i) / xcols, 4.f / ycols));
		}

		sprite[k]->setAnimationSpeed(FALL_LEFT, 6);
		sprite[k]->addKeyframe(FALL_LEFT, glm::vec2(5.f / xcols, 3.f / ycols));
		sprite[k]->addKeyframe(FALL_LEFT, glm::vec2(6.f / xcols, 3.f / ycols));
		sprite[k]->addKeyframe(FALL_LEFT, glm::vec2(0.f / xcols, 3.f / ycols));
		sprite[k]->addKeyframe(FALL_LEFT, glm::vec2(1.f / xcols, 3.f / ycols));

		sprite[k]->setAnimationSpeed(FALL_RIGHT, 6);
		sprite[k]->addKeyframe(FALL_RIGHT, glm::vec2(5.f / xcols, 4.f / ycols));
		sprite[k]->addKeyframe(FALL_RIGHT, glm::vec2(6.f / xcols, 4.f / ycols));
		sprite[k]->addKeyframe(FALL_RIGHT, glm::vec2(0.f / xcols, 4.f / ycols));
		sprite[k]->addKeyframe(FALL_RIGHT, glm::vec2(1.f / xcols, 4.f / ycols));

		sprite[k]->setAnimationSpeed(GRAVITY_LEFT, 8);
		for (int i = 0; i < 4; i++) {
			sprite[k]->addKeyframe(GRAVITY_LEFT, glm::vec2(float(i) / xcols, 5.f / ycols));
		}

		sprite[k]->setAnimationSpeed(GRAVITY_RIGHT, 8);
		for (int i = 4; i < 8; i++) {
			sprite[k]->addKeyframe(GRAVITY_RIGHT, glm::vec2(float(i) / xcols, 5.f / ycols));
		}

		sprite[k]->changeAnimation(0);
		sprite[k]->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
	}
}

void Player::update(int deltaTime) {

	if (Game::instance().gravity) {
		g = 1;
		currentSpriteSheet = 0;
	}
	else {
		g = -1;
		currentSpriteSheet = 1;
	}
	
	if (gravityStep < GRAVITY_STEP) gravityStep += 0.3;

	sprite[currentSpriteSheet]->update(deltaTime);
	if (Game::instance().getSpecialKey(GLUT_KEY_LEFT)) {
		mv = true;
		left = true;
		posPlayer.x -= MOVEMENT_STEP;
		if (Game::instance().ground) {
			if (sprite[currentSpriteSheet]->animation() != MOVE_LEFT)
				sprite[currentSpriteSheet]->changeAnimation(MOVE_LEFT);
		}
	}
	else if (Game::instance().getSpecialKey(GLUT_KEY_RIGHT)) {
		mv = true;
		left = false;
		posPlayer.x += MOVEMENT_STEP;
		if (Game::instance().ground) {
			if (sprite[currentSpriteSheet]->animation() != MOVE_RIGHT)
				sprite[currentSpriteSheet]->changeAnimation(MOVE_RIGHT);
		}
	}
	else if (!mv) {
		if (left && Game::instance().ground && sprite[currentSpriteSheet]->animation() != STAND_LEFT)
			sprite[currentSpriteSheet]->changeAnimation(STAND_LEFT);
		else if (!left && Game::instance().ground && sprite[currentSpriteSheet]->animation() != STAND_RIGHT)
			sprite[currentSpriteSheet]->changeAnimation(STAND_RIGHT);
	}
	// Si s'esta movent, mirem si hi ha colisio esquerra o dreta (per corretgir) i reiniciem mv
	if (mv) {
		if (left) map->collisionMoveLeft(posPlayer, sprite[currentSpriteSheet]->size, &posPlayer.x);
		else map->collisionMoveRight(posPlayer, sprite[currentSpriteSheet]->size, &posPlayer.x);
		mv = false;
	}

	if (Game::instance().getSpecialKey(GLUT_KEY_UP) && Game::instance().ground) {
		mv = true;
		jumping = true;
		jumpAngle = 0;
		startY = posPlayer.y;
		Game::instance().ground = false;
		if (left && sprite[currentSpriteSheet]->animation() != JUMP_LEFT)
			sprite[currentSpriteSheet]->changeAnimation(JUMP_LEFT);
		else if (!left && sprite[currentSpriteSheet]->animation() != JUMP_RIGHT)
			sprite[currentSpriteSheet]->changeAnimation(JUMP_RIGHT);
	}
	if (jumping) {
		jumpAngle += JUMP_ANGLE_STEP;
		if (jumpAngle == 180) {
			jumping = false;
			gravityStep = 0;
		}
		else  posPlayer.y = int(startY - JUMP_HEIGHT * g * sin(3.14159f * jumpAngle / 180.f));
		if (jumpAngle <= 90) {
			if (Game::instance().gravity) {
				if (map->collisionMoveUp(posPlayer, sprite[currentSpriteSheet]->size, &posPlayer.y)) {
					jumping = false;
					gravityStep = 0;
				}
			}
			else {
				if (map->collisionMoveDown(posPlayer, sprite[currentSpriteSheet]->size, &posPlayer.y)) {
					jumping = false;
					gravityStep = 0;
				}
			}
		}
	}

	// Supossem que no esta tocant ground, false.
	Game::instance().ground = false;
	// Fem que caigui sempre, a no ser que estigui saltant. Comprovem colisions amb el terra sempre i aixo ja ho corretgeix.
	if (!jumping) posPlayer.y += g * (FALL_STEP + int(gravityStep));
	// Si esta tocant ground, true.
	if (Game::instance().gravity) {
		if (map->collisionMoveDown(posPlayer, sprite[currentSpriteSheet]->size, &posPlayer.y)) {
			Game::instance().ground = true;
			jumping = false;
			gravityStep = 0;
		}
	}
	else {
		if (map->collisionMoveUp(posPlayer, sprite[currentSpriteSheet]->size, &posPlayer.y)) {
			Game::instance().ground = true;
			jumping = false;
			gravityStep = 0;
		}
	}
	if (!Game::instance().ground && !jumping) {
		mv = true;
		if (left && sprite[currentSpriteSheet]->animation() != FALL_LEFT)
			sprite[currentSpriteSheet]->changeAnimation(FALL_LEFT);
		else if (!left && sprite[currentSpriteSheet]->animation() != FALL_RIGHT)
			sprite[currentSpriteSheet]->changeAnimation(FALL_RIGHT);
	}

	sprite[currentSpriteSheet]->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}

void Player::render() {
	sprite[currentSpriteSheet]->render();
}

void Player::setTileMap(TileMap *tileMap) {
	map = tileMap;
}

void Player::setPosition(const glm::vec2 &pos) {
	posPlayer = pos;
	sprite[currentSpriteSheet]->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}




