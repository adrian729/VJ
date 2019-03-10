#ifndef _PLAYER_INCLUDE
#define _PLAYER_INCLUDE


#include "Sprite.h"
#include "TileMap.h"


// Player is basically a Sprite that represents the player. As such it has
// all properties it needs to track its movement, jumping, and collisions.


class Player
{

public:
	void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram);
	void update(int deltaTime);
	void render();
	
	void setTileMap(TileMap *tileMap);
	void setPosition(const glm::vec2 &pos);
	
private:
	bool jumping;
	bool left, mv; // direccio mira pj i si s'ha mogut
	glm::ivec2 tileMapDispl, posPlayer;
	int jumpAngle, startY, currentSpriteSheet;
	int g; // direccio gravetat (1 normal -1 invertida)
	glm::ivec2 spriteSize[2]; // tamany dels sprites del sheet
	Texture spritesheet[2];
	Sprite *sprite[2];
	TileMap *map;

};


#endif // _PLAYER_INCLUDE


