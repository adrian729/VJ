#include "Enemy.h"
#include "Globals.h"


/*
	- MOVE_1: left/up
	- MOVE_2: right/down
*/
enum EnemyAnimations {
	MOVE_1, MOVE_2
};


enum EnemyType {
	ARM, BIG_ARM, ROBOT_1, ROBOT_2, ROBOT_3
};


void Enemy::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram, const int &type, const bool &direction, const glm::ivec2 &posStart, const glm::ivec2 &posEnd, const int &upDown) {

	tileMapDispl = tileMapPos;
	s = -1;

	int xcols = 8;
	int ycols = 10;

	int kfps;

	spritesheet[0].loadFromFile("images/EnemiesUp.png", TEXTURE_PIXEL_FORMAT_RGBA);
	spritesheet[1].loadFromFile("images/EnemiesDown.png", TEXTURE_PIXEL_FORMAT_RGBA);

	float col = 0.f;

	int numEnemies = 5;
	if (type == ARM) {
		velocity = 2;
		enemySize = glm::ivec2(64, 64);
		startCollision[0] = glm::ivec3(20, 18, 20);
		startCollision[1] = glm::ivec3(20, 18, 20);
		col = 0.f;
		kfps = 6;
	}
	else if (type == BIG_ARM) {
		velocity = 2;
		enemySize = glm::ivec2(128, 128);
		startCollision[0] = glm::ivec3(40, 36, 40);
		startCollision[1] = glm::ivec3(40, 36, 40);
		col = 0.f;
		kfps = 6;
	}
	else if (type == ROBOT_1) {
		velocity = 2;
		enemySize = glm::ivec2(64, 64);
		startCollision[0] = glm::ivec3(9, 14, 9);
		startCollision[1] = glm::ivec3(9, 14, 9);
		col = 1.f;
		kfps = 10;
	}
	else if (type == ROBOT_2) {
		velocity = 5;
		enemySize = glm::ivec2(64, 64);
		startCollision[0] = glm::ivec3(7, 14, 7);
		startCollision[1] = glm::ivec3(7, 14, 7);
		col = 2.f;
		kfps = 6;
	}
	else if (type == ROBOT_3) {
		velocity = 1;
		enemySize = glm::ivec2(64, 64);
		startCollision[0] = glm::ivec3(6, 34, 6);
		startCollision[1] = glm::ivec3(6, 34, 6);
		col = 3.f;
		kfps = 6;
	}


	horizontal = direction;
	startPosition = posStart - enemySize;
	endPosition = posEnd - enemySize;

	posEnemy = startPosition;

	for (int k = 0; k < 2; k++) {
		sprite[k] = Sprite::createSprite(enemySize, glm::vec2(1.f / xcols, 1.f / ycols), &spritesheet[k], &shaderProgram);
		sprite[k]->setNumberAnimations(2);

		sprite[k]->setAnimationSpeed(MOVE_1, kfps);
		for (int i = 0; i < 4; i++) {
			sprite[k]->addKeyframe(MOVE_1, glm::vec2(float(i) / xcols, col / ycols));
		}

		sprite[k]->setAnimationSpeed(MOVE_2, kfps);
		for (int i = 4; i < 8; i++) {
			sprite[k]->addKeyframe(MOVE_2, glm::vec2(float(i) / xcols, col / ycols));
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
				if (sprite[upDown]->animation() != MOVE_2) {
					sprite[upDown]->changeAnimation(MOVE_2);
				}
			}
		}
		else {
			collision = startCollision[upDown].z;
			if ((posEnemy.x - collision) > endPosition.x) {
				posEnemy -= s * velocity;
				s *= -1;
				if (sprite[upDown]->animation() != MOVE_1) {
					sprite[upDown]->changeAnimation(MOVE_1);
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
