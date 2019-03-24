#include <iostream>
#include <cmath>
#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"


#define SCREEN_X 0
#define SCREEN_Y 0


Scene::Scene() {
	for (int i = 0; i < sizeof(map) / sizeof(int); i++) {
		map[i] = NULL;
	}
	player = NULL;
}

Scene::~Scene() {
	if (map != NULL)
		delete[] map;
	if (player != NULL)
		delete player;
}

void Scene::init() {
	initShaders();

	currentMap = 1;
	checkpointMap = 1;

	// PRUEBAS SONIDO! Retocar la classe (es la del Sergio....), com a minim funciona <3
	sound_prova = new sound_t(SoundPlayer::instance().createSound("sound/Little_Busters.wav"));
	SoundPlayer::instance().playSound(sound_prova);
	float v;
	sound_prova->channel->setVolume(0.05f);
	sound_prova->channel->getVolume(&v);
	cout << "INFO: " << v << endl;

	map[0] = TileMap::createTileMap("level00", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	map[1] = TileMap::createTileMap("level01", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	map[2] = TileMap::createTileMap("level02", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);

	player = new Player();
	sceneSize = map[currentMap]->getSceneSize();
	player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	player->setPosition(map[currentMap]->getPlayerInitPosition() - glm::vec2(player->getPlayerSize()));
	player->checkpoint = map[currentMap]->getPlayerInitPosition() - glm::vec2(player->getPlayerSize());
	player->setTileMap(map[currentMap]);
	calculateProjectionMatrix();
	currentTime = 0.0f;
}

void Scene::update(int deltaTime) {
	if (changeMap && timer < 30) {
		timer++;
		if (timer == 1) {
			// Mapa "transicio"
			transitionMap = currentMap; // guardem a quin mapa estavem
			currentMap = 0; // mapa "transicio"
		}
		else if (timer >= 30) {
			currentMap = transitionMap;
			transitionMap = 0;
		}
	}
	else if (player->playerState == RESTART) {
		int newMap = checkpointMap;
		if (!changeMap && currentMap != 0 && newMap != currentMap) {
			changeMap = true;
		}
		else {
			changeMap = false;
			timer = 0;
			player->restart();
			currentMap = checkpointMap;
			player->setTileMap(map[currentMap]);
			player->playerState = NONE;
		}
	}
	else if (player->playerState == CHANGE_MAP) {
		int newMap = map[currentMap]->changeMapInfo[map[currentMap]->changeMapInfoId].r;
		if (!changeMap && currentMap != 0 && newMap != currentMap) {
			changeMap = true;
		}
		else {
			changeMap = false;
			timer = 0;
			int changeMapInfoId = map[currentMap]->changeMapInfoId;
			glm::ivec3 newMapInfo = map[currentMap]->changeMapInfo[changeMapInfoId];
			player->changeMap(glm::vec2(newMapInfo.y, newMapInfo.z) - glm::vec2(player->getPlayerSize()));
			currentMap = newMapInfo.x;
			player->setTileMap(map[currentMap]);
			player->playerState = NONE;
		}
	}
	else if (player->playerState == CHECKPOINT) {
		if(currentMap != checkpointMap) map[checkpointMap]->activatedCheckpoint = glm::ivec2(-1, -1);
		checkpointMap = currentMap;
		player->checkpoint = player->posPlayer;
		player->playerState = NONE;
	}

	currentTime += deltaTime;
	player->update(deltaTime);
	map[currentMap]->update(deltaTime, texProgram);
}

void Scene::render() {
	glm::mat4 modelview = glm::mat4(1.0f);
	calculateProjectionMatrix();
	texProgram.use();
	texProgram.setUniformMatrix4f("projection", projection);
	texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
	texProgram.setUniformMatrix4f("modelview", modelview);
	texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
	map[currentMap]->renderBackground();
	if (currentMap != 0) {
		map[currentMap]->render();
		player->render();
		// Tornar a fer set de modelview i texCoordDispl ja que player->render() les modifica.
		texProgram.setUniformMatrix4f("modelview", modelview);
		texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
		map[currentMap]->renderFront();
		//map[currentMap]->renderLights();
	}
}

void Scene::calculateProjectionMatrix() {
	V = A = 1.f;
	if (glutGet(GLUT_WINDOW_HEIGHT) > 0) V = float(glutGet(GLUT_WINDOW_WIDTH)) / glutGet(GLUT_WINDOW_HEIGHT);
	if (sceneSize.y != 0) A = float(sceneSize.x) / sceneSize.y;
	if (V >= A) {
		projection = glm::ortho(0.f, (V / A) * float(sceneSize.x), float(sceneSize.y), 0.f);
		projection *= glm::translate(glm::mat4(), glm::vec3(float(sceneSize.x) * ((V / A) - 1.f) / 2, 0, 0));
	}
	else {
		projection = glm::ortho(0.5f, float(sceneSize.x), (A / V) * float(sceneSize.y), 0.f);
		projection *= glm::translate(glm::mat4(), glm::vec3(0, float(sceneSize.y) * ((A / V) - 1.f) / 2, 0));
	}
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
	if (!texProgram.isLinked()) {
		cout << "Shader Linking Error" << endl;
		cout << "" << texProgram.log() << endl << endl;
	}
	texProgram.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();
}



