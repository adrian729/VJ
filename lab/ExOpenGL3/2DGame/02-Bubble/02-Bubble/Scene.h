#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "Background.h"
#include "TileMap.h"
#include "Player.h"


// Scene contains all the entities of our game.
// It is responsible for updating and render them.


class Scene
{

public:
	Scene();
	~Scene();

	void init();
	void update(int deltaTime);
	void render();

private:
	void initShaders();

private:
	Background *background;
	TileMap *map, *frontMap;
	Player *player;
	ShaderProgram texProgram;
	float currentTime;
	float ra;
	glm::mat4 projection;
};


#endif // _SCENE_INCLUDE

