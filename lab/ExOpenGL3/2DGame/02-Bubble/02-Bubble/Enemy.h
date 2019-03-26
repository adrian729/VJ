#ifndef _ENEMY_INCLUDE
#define _ENEMY_INCLUDE

#include "Sprite.h"
#include "PlayerState.h"

class Enemy {

	public:
		Enemy() {};
		~Enemy();

		void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram);
		void update(int deltaTime);
		void render();

		void setPosition(const glm::vec2 &pos);

		bool left;
		glm::ivec2 posEnemy;

	private:
		glm::ivec2 tileMapDispl;
		int g; // direccio gravetat (1 normal -1 invertida)
		int currentSpriteSheet;
		glm::ivec3 startColision[2]; // left, up, right (x, y, t)
		Texture spritesheet[2];
		Sprite *sprite[2];
};

#endif // _ENEMY_INCLUDE
