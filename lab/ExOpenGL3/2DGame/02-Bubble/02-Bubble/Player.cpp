#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Player.h"
#include "Game.h"


#define JUMP_ANGLE_STEP 5
#define JUMP_HEIGHT 96
#define FALL_STEP 10
#define MOVEMENT_STEP 4
#define GRAVITY_STEP 20


#define FPS 60
#define STAND_KFPS 6
#define MOVE_KFPS 18
#define JUMP_KFPS 8
#define FALL_KFPS 6
#define GRAVITY_KFPS 14
#define DEATH_KFPS 6

#define INFINITE_LOOP -1

#define STAND_SOUND_VOLUME .1f
#define MOVE_SOUND_VOLUME .1f
#define JUMP_SOUND_VOLUME .1f
#define FALL_SOUND_VOLUME .1f
#define GRAVITY_SOUND_VOLUME .1f
#define DEATH_SOUND_VOLUME .1f


enum PlayerAnimations {
	STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT, JUMP_LEFT, JUMP_RIGHT,
	FALL_LEFT, FALL_RIGHT, GRAVITY_LEFT, GRAVITY_RIGHT, DEATH_LEFT, DEATH_RIGHT
};

enum PlayerAnimationSounds {
	STAND, MOVE, JUMP, FALL, CHANGE_GRAVITY_EFFECT, DEATH
};

void Player::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram) {

	actualSound = NULL;

	tileMapDispl = tileMapPos;

	int xcols = 8;
	int ycols = 8;

	spritesheet[0].loadFromFile("images/HarukoSpritesUp.png", TEXTURE_PIXEL_FORMAT_RGBA);
	spritesheet[1].loadFromFile("images/HarukoSpritesDown.png", TEXTURE_PIXEL_FORMAT_RGBA);
	//spritesheet[0].loadFromFile("images/HarukoSpritesUp-test.png", TEXTURE_PIXEL_FORMAT_RGBA);
	//spritesheet[1].loadFromFile("images/HarukoSpritesDown-test.png", TEXTURE_PIXEL_FORMAT_RGBA);

	for (int k = 0; k < 2; k++) {
		startCollision[k] = glm::ivec3(15, 5, 16);
		sprite[k] = Sprite::createSprite(glm::ivec2(64, 64), glm::vec2(1.f / xcols, 1.f / ycols), &spritesheet[k], &shaderProgram);
		sprite[k]->setNumberAnimations(12);

		sprite[k]->setAnimationSpeed(STAND_LEFT, STAND_KFPS);
		for (int i = 0; i < 4; i++) {
			sprite[k]->addKeyframe(STAND_LEFT, glm::vec2(float(i) / xcols, 0.f));
		}
		sprite[k]->addKeyframe(STAND_LEFT, glm::vec2(2.f / xcols, 0.f));
		sprite[k]->addKeyframe(STAND_LEFT, glm::vec2(1.f / xcols, 0.f));

		sprite[k]->setAnimationSpeed(STAND_RIGHT, STAND_KFPS);
		for (int i = 4; i < 8; i++) {
			sprite[k]->addKeyframe(STAND_RIGHT, glm::vec2(float(i) / xcols, 0.f));
		}
		sprite[k]->addKeyframe(STAND_RIGHT, glm::vec2(6.f / xcols, 0.f));
		sprite[k]->addKeyframe(STAND_RIGHT, glm::vec2(5.f / xcols, 0.f));

		sprite[k]->setAnimationSpeed(MOVE_LEFT, MOVE_KFPS);
		for (int i = 0; i < 8; i++) {
			sprite[k]->addKeyframe(MOVE_LEFT, glm::vec2(float(i) / xcols, 1.f / ycols));
		}

		sprite[k]->setAnimationSpeed(MOVE_RIGHT, MOVE_KFPS);
		for (int i = 0; i < 8; i++) {
			sprite[k]->addKeyframe(MOVE_RIGHT, glm::vec2(float(i) / xcols, 2.f / ycols));
		}

		sprite[k]->setAnimationSpeed(JUMP_LEFT, JUMP_KFPS);
		for (int i = 0; i < 8; i++) {
			sprite[k]->addKeyframe(JUMP_LEFT, glm::vec2(float(i) / xcols, 3.f / ycols));
		}

		sprite[k]->setAnimationSpeed(JUMP_RIGHT, JUMP_KFPS);
		for (int i = 0; i < 8; i++) {
			sprite[k]->addKeyframe(JUMP_RIGHT, glm::vec2(float(i) / xcols, 4.f / ycols));
		}

		sprite[k]->setAnimationSpeed(FALL_LEFT, FALL_KFPS);
		sprite[k]->addKeyframe(FALL_LEFT, glm::vec2(5.f / xcols, 3.f / ycols));
		sprite[k]->addKeyframe(FALL_LEFT, glm::vec2(6.f / xcols, 3.f / ycols));
		sprite[k]->addKeyframe(FALL_LEFT, glm::vec2(0.f / xcols, 3.f / ycols));
		sprite[k]->addKeyframe(FALL_LEFT, glm::vec2(1.f / xcols, 3.f / ycols));

		sprite[k]->setAnimationSpeed(FALL_RIGHT, FALL_KFPS);
		sprite[k]->addKeyframe(FALL_RIGHT, glm::vec2(5.f / xcols, 4.f / ycols));
		sprite[k]->addKeyframe(FALL_RIGHT, glm::vec2(6.f / xcols, 4.f / ycols));
		sprite[k]->addKeyframe(FALL_RIGHT, glm::vec2(0.f / xcols, 4.f / ycols));
		sprite[k]->addKeyframe(FALL_RIGHT, glm::vec2(1.f / xcols, 4.f / ycols));

		sprite[k]->setAnimationSpeed(GRAVITY_LEFT, GRAVITY_KFPS);
		for (int i = 0; i < 4; i++) {
			sprite[k]->addKeyframe(GRAVITY_LEFT, glm::vec2(float(i) / xcols, 5.f / ycols));
		}

		sprite[k]->setAnimationSpeed(GRAVITY_RIGHT, GRAVITY_KFPS);
		for (int i = 4; i < 8; i++) {
			sprite[k]->addKeyframe(GRAVITY_RIGHT, glm::vec2(float(i) / xcols, 5.f / ycols));
		}

		sprite[k]->setAnimationSpeed(DEATH_LEFT, DEATH_KFPS);
		for (int i = 0; i < 8; i++) {
			sprite[k]->addKeyframe(DEATH_LEFT, glm::vec2(float(i) / xcols, 6.f / ycols));
		}

		sprite[k]->setAnimationSpeed(DEATH_RIGHT, DEATH_KFPS);
		for (int i = 0; i < 8; i++) {
			sprite[k]->addKeyframe(DEATH_RIGHT, glm::vec2(float(i) / xcols, 7.f / ycols));
		}

		sprite[k]->changeAnimation(0);
		sprite[k]->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
	}

	soundAction[STAND] = "sound/Actions/silence.wav";
	soundAction[MOVE] = "sound/Actions/run.wav";
	soundAction[JUMP] = "sound/Actions/silence.wav";
	soundAction[FALL] = "sound/Actions/silence.wav";
	soundAction[CHANGE_GRAVITY_EFFECT] = "sound/Actions/silence.wav";
	soundAction[DEATH] = "sound/Actions/silence.wav";

	resetVariables();
}

void Player::resetVariables() {
	Game::instance().gravity = true;
	g = 1;
	currentSpriteSheet = 0;
	gravityStep = 0;
	jumping = false;
	left = true;
	sprite[currentSpriteSheet]->changeAnimation(0);
	changeAnimationSound(STAND, STAND_SOUND_VOLUME);

	animationTimer = 0;
	playerState = NONE;
}

void Player::restart() {
	resetVariables();
	setPosition(checkpoint);
}

void Player::changeMap(const glm::vec2 &newPosition) {
	// resetVariables();
	setPosition(newPosition);
}

void Player::changeAnimationSound(int animation, float volume) {
	if (actualSound) Audio::instance().release(actualSound);
	actualSound = new audio(Audio::instance().createAudio(soundAction[animation]));
	Audio::instance().play(actualSound, INFINITE_LOOP, volume);
}

void Player::update(int deltaTime) {

	sprite[currentSpriteSheet]->update(deltaTime);

	// RESTART
	if (playerState == RESTART) return;

	// Death
	if (playerState == DEAD) {
		if (left && sprite[currentSpriteSheet]->animation() != DEATH_LEFT) {
			sprite[currentSpriteSheet]->changeAnimation(DEATH_LEFT);
			changeAnimationSound(DEATH, DEATH_SOUND_VOLUME);
		}
		else if (!left && sprite[currentSpriteSheet]->animation() != DEATH_RIGHT) {
			sprite[currentSpriteSheet]->changeAnimation(DEATH_RIGHT);
			changeAnimationSound(DEATH, DEATH_SOUND_VOLUME);
		}

		animationTimer++;
		// frames animation * fps update / keyframes ps animation
		if (animationTimer > 8 * FPS / DEATH_KFPS) {
			playerState = RESTART;
		}

		sprite[currentSpriteSheet]->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
		return;
	}

	if (playerState == CHANGE_GRAVITY) {
		Game::instance().gravity = !Game::instance().gravity;
		playerState = NONE;
	}

	if (Game::instance().gravity) {
		if (g != 1) {
			Game::instance().ground = false;
			playerState = CHANGING_GRAVITY;
			animationTimer = 0;
		}
		g = 1;
		currentSpriteSheet = 0;
	}
	else {
		if (g != -1) {
			Game::instance().ground = false;
			playerState = CHANGING_GRAVITY;
			animationTimer = 0;
		}
		g = -1;
		currentSpriteSheet = 1;
	}

	glm::vec2 playerSize = sprite[currentSpriteSheet]->size;
	playerSize.x -= (startCollision[currentSpriteSheet].x + startCollision[currentSpriteSheet].p);
	playerSize.y -= startCollision[currentSpriteSheet].y;

	if (g == 1) posPlayer.y += startCollision[currentSpriteSheet].y;

	// Changing gravity
	if (playerState == CHANGING_GRAVITY) {
		if (g == 1) posPlayer.y -= g * startCollision[currentSpriteSheet].y;
		if (left && sprite[currentSpriteSheet]->animation() != GRAVITY_LEFT) {
			sprite[currentSpriteSheet]->changeAnimation(GRAVITY_LEFT);
			changeAnimationSound(CHANGE_GRAVITY_EFFECT, GRAVITY_SOUND_VOLUME);
		}
		else if (!left && sprite[currentSpriteSheet]->animation() != GRAVITY_RIGHT) {
			sprite[currentSpriteSheet]->changeAnimation(GRAVITY_RIGHT);
			changeAnimationSound(CHANGE_GRAVITY_EFFECT, GRAVITY_SOUND_VOLUME);
		}

		animationTimer++;
		// frames animation * fps update / keyframes ps animation
		if (animationTimer > 4 * FPS / GRAVITY_KFPS) {
			playerState = NONE;
			animationTimer = 0;
		}

		sprite[currentSpriteSheet]->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
		return;
	}

	if (gravityStep < GRAVITY_STEP) gravityStep += 0.4f;

	if (Game::instance().getSpecialKey(GLUT_KEY_LEFT)) {
		mv = true;
		left = true;
		posPlayer.x -= MOVEMENT_STEP;
		if (Game::instance().ground) {
			if (sprite[currentSpriteSheet]->animation() != MOVE_LEFT) {
				sprite[currentSpriteSheet]->changeAnimation(MOVE_LEFT);
				changeAnimationSound(MOVE, MOVE_SOUND_VOLUME);
			}
		}
	}
	else if (Game::instance().getSpecialKey(GLUT_KEY_RIGHT)) {
		mv = true;
		left = false;
		posPlayer.x += MOVEMENT_STEP;
		if (Game::instance().ground) {
			if (sprite[currentSpriteSheet]->animation() != MOVE_RIGHT) {
				sprite[currentSpriteSheet]->changeAnimation(MOVE_RIGHT);
				changeAnimationSound(MOVE, MOVE_SOUND_VOLUME);
			}
		}
	}
	else if (!mv) {
		if (left && Game::instance().ground && sprite[currentSpriteSheet]->animation() != STAND_LEFT) {
			sprite[currentSpriteSheet]->changeAnimation(STAND_LEFT);
			changeAnimationSound(STAND, STAND_SOUND_VOLUME);
		}
		else if (!left && Game::instance().ground && sprite[currentSpriteSheet]->animation() != STAND_RIGHT) {
			sprite[currentSpriteSheet]->changeAnimation(STAND_RIGHT);
			changeAnimationSound(STAND, STAND_SOUND_VOLUME);
		}
	}

	if (left) posPlayer.x += startCollision[currentSpriteSheet].x;
	else posPlayer.x += startCollision[currentSpriteSheet].p;
	// Si s'esta movent, mirem si hi ha colisio esquerra o dreta (per corretgir) i reiniciem mv
	if (mv) {
		if (left) map->collisionMoveLeft(posPlayer, playerSize, &posPlayer, playerState);
		else map->collisionMoveRight(posPlayer, playerSize, &posPlayer, playerState);
		mv = false;
	}

	if (Game::instance().getSpecialKey(GLUT_KEY_UP) && Game::instance().ground) {
		mv = true;
		jumping = true;
		jumpAngle = 0;
		startY = posPlayer.y;
		Game::instance().ground = false;
		if (left && sprite[currentSpriteSheet]->animation() != JUMP_LEFT) {
			sprite[currentSpriteSheet]->changeAnimation(JUMP_LEFT);
			changeAnimationSound(JUMP, JUMP_SOUND_VOLUME);
		}
		else if (!left && sprite[currentSpriteSheet]->animation() != JUMP_RIGHT) {
			sprite[currentSpriteSheet]->changeAnimation(JUMP_RIGHT);
			changeAnimationSound(JUMP, JUMP_SOUND_VOLUME);
		}
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
				if (map->collisionMoveUp(posPlayer, playerSize, &posPlayer, playerState)) {
					jumping = false;
					gravityStep = 0;
				}
			}
			else {
				if (map->collisionMoveDown(posPlayer, playerSize, &posPlayer, playerState)) {
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
		if (map->collisionMoveDown(posPlayer, playerSize, &posPlayer, playerState)) {
			Game::instance().ground = true;
			jumping = false;
			gravityStep = 0;
		}
	}
	else {
		if (map->collisionMoveUp(posPlayer, playerSize, &posPlayer, playerState)) {
			Game::instance().ground = true;
			jumping = false;
			gravityStep = 0;
		}
	}
	if (!Game::instance().ground && !jumping) {
		mv = true;
		if (left && sprite[currentSpriteSheet]->animation() != FALL_LEFT) {
			sprite[currentSpriteSheet]->changeAnimation(FALL_LEFT);
			changeAnimationSound(FALL, FALL_SOUND_VOLUME);
		}
		else if (!left && sprite[currentSpriteSheet]->animation() != FALL_RIGHT) {
			sprite[currentSpriteSheet]->changeAnimation(FALL_RIGHT);
			changeAnimationSound(FALL, FALL_SOUND_VOLUME);
		}
	}

	if (left) posPlayer.x -= startCollision[currentSpriteSheet].x;
	else posPlayer.x -= startCollision[currentSpriteSheet].p;

	if (g == 1)posPlayer.y -= startCollision[currentSpriteSheet].y;

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




