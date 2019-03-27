#ifndef _ENEMY_INCLUDE
#define _ENEMY_INCLUDE

#include "Sprite.h"
#include "PlayerState.h"

class Enemy {

public:
	Enemy() {};
	~Enemy();

	void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram,
		const int &type, const bool &direction,
		const glm::ivec2 &posStart, const glm::ivec2 &posEnd, const int &upDown);
	void update(int deltaTime);
	void render();

	void getSize() const;
	void setPosition(const glm::vec2 &pos);

	bool horizontal;

	int upDown; // spritesheet for looking up or down
	int s;
	glm::ivec2 posEnemy;
	glm::ivec2 enemySize;

	glm::ivec3 getStartCollision() const { return startCollision[upDown]; };

private:
	glm::ivec2 tileMapDispl;
	int velocity;
	glm::ivec2 startPosition, endPosition;
	glm::ivec3 startCollision[2]; // left, up, right (x, y, t)
	Texture spritesheet[2];
	Sprite *sprite[2];
};

#endif // _ENEMY_INCLUDE
