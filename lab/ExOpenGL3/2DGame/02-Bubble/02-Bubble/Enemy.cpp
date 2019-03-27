#include "Enemy.h"


#define FPS 60
#define STAND_KFPS 6
#define MOVE_KFPS 18


enum EnemyAnimations {
	MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN
};


enum EnemyType {
	ARM
};


void Enemy::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram, const int &type, const bool &direction, const glm::ivec2 &posStart, const glm::ivec2 &posEnd, const int &upDown) {

	tileMapDispl = tileMapPos;
	s = 1;

	int xcols = 8;
	int ycols = 8;

	if (type == ARM) {
		velocity = 2;
		enemySize = glm::ivec2(64, 64);
		spritesheet[0].loadFromFile("images/HarukoSpritesUp.png", TEXTURE_PIXEL_FORMAT_RGBA);
		spritesheet[1].loadFromFile("images/HarukoSpritesDown.png", TEXTURE_PIXEL_FORMAT_RGBA);
	}

	horizontal = direction;
	startPosition = posStart - enemySize;
	endPosition = posEnd - enemySize;

	posEnemy = startPosition;

	for (int k = 0; k < 2; k++) {
		startCollision[k] = glm::ivec3(15, 5, 16);
		sprite[k] = Sprite::createSprite(enemySize, glm::vec2(1.f / xcols, 1.f / ycols), &spritesheet[k], &shaderProgram);
		sprite[k]->setNumberAnimations(12);

		sprite[k]->setAnimationSpeed(MOVE_LEFT, MOVE_KFPS);
		for (int i = 0; i < 8; i++) {
			sprite[k]->addKeyframe(MOVE_LEFT, glm::vec2(float(i) / xcols, 1.f / ycols));
		}

		sprite[k]->setAnimationSpeed(MOVE_RIGHT, MOVE_KFPS);
		for (int i = 0; i < 8; i++) {
			sprite[k]->addKeyframe(MOVE_RIGHT, glm::vec2(float(i) / xcols, 2.f / ycols));
		}

		sprite[k]->setAnimationSpeed(MOVE_UP, MOVE_KFPS);
		for (int i = 0; i < 8; i++) {
			sprite[k]->addKeyframe(MOVE_UP, glm::vec2(float(i) / xcols, 3.f / ycols));
		}

		sprite[k]->setAnimationSpeed(MOVE_DOWN, MOVE_KFPS);
		for (int i = 0; i < 8; i++) {
			sprite[k]->addKeyframe(MOVE_DOWN, glm::vec2(float(i) / xcols, 4.f / ycols));
		}

		sprite[k]->changeAnimation(0);
		sprite[k]->setPosition(glm::vec2(float(tileMapDispl.x + posEnemy.x), float(tileMapDispl.y + posEnemy.y)));
	}

	this->upDown = upDown;
	sprite[upDown]->changeAnimation(0);
}

void Enemy::update(int deltaTime) {
	sprite[upDown]->update(deltaTime);

	int collision;
	if (horizontal) {
		posEnemy.x += s * velocity;
		// si s < 0, left
		if (s < 0) {
			collision = startCollision[upDown].x;
			if ((posEnemy.x + collision) < startPosition.x) {
				posEnemy -= s * velocity;
				s *= -1;
				if (sprite[upDown]->animation() != MOVE_RIGHT) {
					sprite[upDown]->changeAnimation(MOVE_RIGHT);
				}
			}
		}
		else {
			collision = startCollision[upDown].z;
			if ((posEnemy.x - collision) > endPosition.x) {
				posEnemy -= s * velocity;
				s *= -1;
				if (sprite[upDown]->animation() != MOVE_LEFT) {
					sprite[upDown]->changeAnimation(MOVE_LEFT);
				}
			}
		}
	}
	else {

	}
	sprite[upDown]->setPosition(glm::vec2(float(tileMapDispl.x + posEnemy.x), float(tileMapDispl.y + posEnemy.y)));
}

void Enemy::render() {
	sprite[upDown]->render();
}

void Enemy::setPosition(const glm::vec2 &pos) {
	posEnemy = pos;
	sprite[upDown]->setPosition(glm::vec2(float(tileMapDispl.x + posEnemy.x), float(tileMapDispl.y + posEnemy.y)));
}
