#include "Enemy.h"


#define FPS 60
#define STAND_KFPS 6
#define MOVE_KFPS 18


enum EnemyAnimations {
	MOVE_LEFT, MOVE_RIGHT
};

void Enemy::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram) {
	tileMapDispl = tileMapPos;

	int xcols = 8;
	int ycols = 8;

	spritesheet[0].loadFromFile("images/HarukoSpritesUp.png", TEXTURE_PIXEL_FORMAT_RGBA);
	spritesheet[1].loadFromFile("images/HarukoSpritesDown.png", TEXTURE_PIXEL_FORMAT_RGBA);
	//spritesheet[0].loadFromFile("images/HarukoSpritesUp-test.png", TEXTURE_PIXEL_FORMAT_RGBA);
	//spritesheet[1].loadFromFile("images/HarukoSpritesDown-test.png", TEXTURE_PIXEL_FORMAT_RGBA);

	for (int k = 0; k < 2; k++) {
		startColision[k] = glm::ivec3(15, 5, 16);
		sprite[k] = Sprite::createSprite(glm::ivec2(64, 64), glm::vec2(1.f / xcols, 1.f / ycols), &spritesheet[k], &shaderProgram);
		sprite[k]->setNumberAnimations(12);

		sprite[k]->setAnimationSpeed(MOVE_LEFT, MOVE_KFPS);
		for (int i = 0; i < 8; i++) {
			sprite[k]->addKeyframe(MOVE_LEFT, glm::vec2(float(i) / xcols, 1.f / ycols));
		}

		sprite[k]->setAnimationSpeed(MOVE_RIGHT, MOVE_KFPS);
		for (int i = 0; i < 8; i++) {
			sprite[k]->addKeyframe(MOVE_RIGHT, glm::vec2(float(i) / xcols, 2.f / ycols));
		}

		sprite[k]->changeAnimation(0);
		sprite[k]->setPosition(glm::vec2(float(tileMapDispl.x + posEnemy.x), float(tileMapDispl.y + posEnemy.y)));
	}

	currentSpriteSheet = 0;
	left = true;
	sprite[currentSpriteSheet]->changeAnimation(0);
}

void Enemy::update(int deltaTime) {
	sprite[currentSpriteSheet]->update(deltaTime);
}

void Enemy::render() {
	sprite[currentSpriteSheet]->render();
}

void Enemy::setPosition(const glm::vec2 &pos) {
	posEnemy = pos;
	sprite[currentSpriteSheet]->setPosition(glm::vec2(float(tileMapDispl.x + posEnemy.x), float(tileMapDispl.y + posEnemy.y)));
}
