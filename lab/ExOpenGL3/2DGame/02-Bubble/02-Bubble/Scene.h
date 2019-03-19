#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Player.h"


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
	void Scene::setMap(const int &mapId);

private:
	void calculateProjectionMatrix();
	void initShaders();

private:
	int currentMap;
	glm::ivec2 initPlayerTiles[1];
	TileMap *map[1];
	Player *player;
	ShaderProgram texProgram;
	float currentTime;
	float V, A;
	glm::mat4 projection;
	glm::ivec2 sceneSize;
};


#endif // _SCENE_INCLUDE

