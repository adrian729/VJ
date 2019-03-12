#include <iostream>
#include <cmath>
#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"


#define SCREEN_X 64
#define SCREEN_Y 64

#define INIT_PLAYER_X_TILES 17
#define INIT_PLAYER_Y_TILES 17


Scene::Scene() {
	background = NULL;
	map = NULL;
	frontMap = NULL;
	player = NULL;
}

Scene::~Scene() {
	if (background != NULL)
		delete background;
	if (map != NULL)
		delete map;
	if (frontMap != NULL)
		delete frontMap;
	if (player != NULL)
		delete player;
}

void Scene::init() {
	initShaders();
	map = TileMap::createTileMap("levels/lvl.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	// -4 i +4 per donar "efecte 2.5D" una mica en els tiles, aixi que ho afegim tambe al background per corretgor la posicio.
	background = Background::createBackground("images/background-prova.png", glm::vec2(SCREEN_X - 4, SCREEN_Y + 4), map->getMapSize()*map->getTileSize(), texProgram);
	frontMap = TileMap::createTileMap("levels/lvl-front.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	player = new Player();
	player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize(), INIT_PLAYER_Y_TILES * map->getTileSize()));
	player->setTileMap(map);
	ra = float(SCREEN_WIDTH - 1) / float(SCREEN_HEIGHT - 1);
	projection = glm::ortho(0.f, float(SCREEN_WIDTH - 1)*ra, float(SCREEN_HEIGHT - 1), 0.f);
	currentTime = 0.0f;
}

void Scene::update(int deltaTime) {
	currentTime += deltaTime;
	player->update(deltaTime);
}

void Scene::render() {
	glm::mat4 modelview;

	cout << "WSize " << glutGet(GLUT_WINDOW_WIDTH) << " " << glutGet(GLUT_WINDOW_HEIGHT) << endl;
	if (glutGet(GLUT_WINDOW_HEIGHT) != 0.f) ra = float(glutGet(GLUT_WINDOW_WIDTH)) / glutGet(GLUT_WINDOW_HEIGHT);

	projection = glm::ortho(0.f, float(SCREEN_WIDTH - 1)*ra, float(SCREEN_HEIGHT - 1), 0.f);

	texProgram.use();
	texProgram.setUniformMatrix4f("projection", projection);
	texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
	modelview = glm::mat4(1.0f);
	texProgram.setUniformMatrix4f("modelview", modelview);
	texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
	background->render();
	map->render();
	player->render();
	// Redeclare things to render map or static things after player again or it wouldn't work
	texProgram.use();
	texProgram.setUniformMatrix4f("projection", projection);
	texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
	modelview = glm::mat4(1.0f);
	texProgram.setUniformMatrix4f("modelview", modelview);
	texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
	frontMap->render();
}

void Scene::initShaders() {
	Shader vShader, fShader;

	vShader.initFromFile(VERTEX_SHADER, "shaders/texture.vert");
	if (!vShader.isCompiled()) {
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "shaders/texture.frag");
	if (!fShader.isCompiled()) {
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	texProgram.init();
	texProgram.addShader(vShader);
	texProgram.addShader(fShader);
	texProgram.link();
	if (!texProgram.isLinked())	{
		cout << "Shader Linking Error" << endl;
		cout << "" << texProgram.log() << endl << endl;
	}
	texProgram.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();
}



