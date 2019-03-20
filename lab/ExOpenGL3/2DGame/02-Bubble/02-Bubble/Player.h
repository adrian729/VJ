#ifndef _PLAYER_INCLUDE
#define _PLAYER_INCLUDE


#include "Sprite.h"
#include "TileMap.h"


// Player is basically a Sprite that represents the player. As such it has
// all properties it needs to track its movement, jumping, and collisions.


class Player {

public:
	Player() {}
	static Player &instance() {
		static Player P;

		return P;
	}

	void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram);
	void Player::resetVariables();
	void update(int deltaTime);
	void render();

	void setTileMap(TileMap *tileMap);
	void setPosition(const glm::vec2 &pos);
	glm::ivec2 getPlayerSize() { return sprite[currentSpriteSheet]->size; }

	bool left;

	int animationTimer;
	int playerState;

private:
	bool jumping;
	bool mv; // direccio mira pj i si s'ha mogut
	glm::ivec2 tileMapDispl, posPlayer;
	int jumpAngle, startY, currentSpriteSheet;
	int g; // direccio gravetat (1 normal -1 invertida)
	float gravityStep;
	glm::ivec3 startColision[2]; // left, up, right (x, y, t)
	Texture spritesheet[2];
	Sprite *sprite[2];
	TileMap *map;
};


#endif // _PLAYER_INCLUDE


