#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE

#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Player.h"
#include "PlayerState.h"
#include "Audio.h"


// Scene contains all the entities of our game.
// It is responsible for updating and render them.
// L'escena es pinta en 3 capes (i player).
// 1. Background
// 2. Mapa (tiles)
// 3. Player
// 4. Mapa (tiles) front, per donar sensació "3D" al girar la gravetat i en colisions a dalt o la dreta.


class Scene
{

public:
	Scene();
	~Scene();

	void init();
	void update(int deltaTime);
	void render();

	bool changeMap;
	int timer;

private:
	void calculateProjectionMatrix();
	void initShaders();

private:
	int currentMap;
	int transitionMap;
	TileMap *map[3];
	char* soundMap[3]; // musica lligada a cada mapa
	audio *actualSound;
	Player *player;
	ShaderProgram texProgram;
	float currentTime;
	float V, A;
	glm::mat4 projection;
	glm::ivec2 sceneSize;
	int checkpointMap;

};


#endif // _SCENE_INCLUDE

