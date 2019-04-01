#include <iostream>
#include <cmath>
#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"


#define TRANSITION_TIME 60

#define INFINITE_LOOP -1
#define MAP_SOUND_VOLUME .06f

#define CANTI_TV 0


Scene::Scene() {
	for (int i = 0; i < sizeof(map) / sizeof(int); i++) {
		map[i] = NULL;
	}
	for (int i = 0; i < sizeof(soundMap) / sizeof(int); i++) {
		soundMap[i] = NULL;
	}
	player = NULL;
}

Scene::~Scene() {
	//if (map != NULL)
	//	delete[] map;
	//if (soundMap != NULL)
	//	delete[] soundMap;
	if (player != NULL)
		delete player;
}

void Scene::init() {
	initShaders();

	currentMap = 1;
	selection = 0;
	menu = true;
	instructions = false;
	canPress = vector<bool>(256, false);
	canPressSpace = false;
	timer = 0;

	// Canti TV
	map[0] = TileMap::createTileMap("CantiTV", glm::vec2(0, 0), texProgram);
	sceneSize = map[CANTI_TV]->getSceneSize();
	tileSize = map[CANTI_TV]->getTileSize();
	soundMap[0] = "sound/Songs/INTRO-Thank_You_My_Twilight.wav";

	actualSound = new audio(Audio::instance().createAudio(soundMap[0]));
	Audio::instance().play(actualSound, INFINITE_LOOP, MAP_SOUND_VOLUME);

	// Menu
	// Select Start
	map[1] = TileMap::createTileMap("MenuStart", glm::vec2(0, 0), texProgram);
	// Select Instructions
	map[2] = TileMap::createTileMap("MenuInstructions", glm::vec2(0, 0), texProgram);

	// Instructions
	map[3] = TileMap::createTileMap("Instructions", glm::vec2(0, 0), texProgram);


	checkpointMap = 4;
	// Level 1
	map[4] = TileMap::createTileMap("level00", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	soundMap[4] = "sound/Songs/Little_Busters.wav";

	//map[1] = TileMap::createTileMap("level01", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	//soundMap[1] = "sound/Songs/Little_Busters.wav";
	//map[2] = TileMap::createTileMap("level02", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	//soundMap[2] = "sound/Songs/Thank_You_My_Twilight.wav";

	player = new Player();
	player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	player->setPosition(map[4]->getPlayerInitPosition() - glm::vec2(player->getPlayerSize()));
	player->checkpoint = map[4]->getPlayerInitPosition() - glm::vec2(player->getPlayerSize());
	player->oldCheckpoint = map[4]->getPlayerInitPosition() - glm::vec2(player->getPlayerSize());
	player->setTileMap(map[4]);
	calculateProjectionMatrix();
	currentTime = 0.0f;
}

void Scene::update(int deltaTime) {

	currentTime += deltaTime;
	// Menus
	if (menu) {
		if (Game::instance().releaseSpecialKeys[GLUT_KEY_UP]) canPress[GLUT_KEY_UP] = true;
		if (Game::instance().releaseSpecialKeys[GLUT_KEY_RIGHT]) canPress[GLUT_KEY_RIGHT] = true;
		if (Game::instance().releaseSpecialKeys[GLUT_KEY_DOWN]) canPress[GLUT_KEY_DOWN] = true;
		if (Game::instance().releaseSpecialKeys[GLUT_KEY_LEFT]) canPress[GLUT_KEY_LEFT] = true;
		if (Game::instance().releaseKey[32]) canPressSpace = true;

		if (Game::instance().getKey(32) && canPressSpace) {
			canPressSpace = false;
			menu = false;
			if (selection == 0) {
				currentMap = 4;
				Audio::instance().release(actualSound);
				actualSound = new audio(Audio::instance().createAudio(soundMap[currentMap]));
				Audio::instance().play(actualSound, INFINITE_LOOP, MAP_SOUND_VOLUME);
			}
			else {
				instructions = true;
				currentMap = 3;
			}
			return;
		}

		if ((Game::instance().getSpecialKey(GLUT_KEY_UP) && canPress[GLUT_KEY_UP])
			|| (Game::instance().getSpecialKey(GLUT_KEY_RIGHT) && canPress[GLUT_KEY_RIGHT])) {

			canPress[GLUT_KEY_UP] = false;
			canPress[GLUT_KEY_RIGHT] = false;
			selection = (selection + 1) % 2;
			currentMap = selection + 1;
		}
		else if ((Game::instance().getSpecialKey(GLUT_KEY_DOWN) && canPress[GLUT_KEY_DOWN])
			|| (Game::instance().getSpecialKey(GLUT_KEY_LEFT) && canPress[GLUT_KEY_LEFT])) {

			canPress[GLUT_KEY_DOWN] = false;
			canPress[GLUT_KEY_LEFT] = false;
			selection = (selection - 1 + 2) % 2;
			currentMap = selection + 1;
		}
	}
	// Instructions
	else if (instructions) {
		if (Game::instance().releaseKey[32]) canPressSpace = true;

		if (Game::instance().getKey(32) && canPressSpace) {
			canPressSpace = false;
			menu = true;
			instructions = false;
			selection = 0;
			currentMap = 1;
		}
	}
	// Canviar de mapa
	else if (changeMap && timer < TRANSITION_TIME) {
		timer++;
		if (timer == 1) {
			int nextMap = currentMap;
			if (player->playerState == RESTART) {
				nextMap = checkpointMap;
			}
			else if (player->playerState == CHANGE_MAP) {
				nextMap = map[currentMap]->changeMapInfo[map[currentMap]->changeMapInfoId].r;
			}
			cout << "HEI " << nextMap << endl;
			Audio::instance().release(actualSound);
			actualSound = new audio(Audio::instance().createAudio(soundMap[nextMap]));
			Audio::instance().play(actualSound, INFINITE_LOOP, MAP_SOUND_VOLUME);
			// Mapa "transicio"
			transitionMap = currentMap; // guardem a quin mapa estavem
			currentMap = 0; // mapa "transicio"
		}
		else if (timer >= TRANSITION_TIME) {
			currentMap = transitionMap;
			transitionMap = 0;
		}
	}
	// resta accions segons estat del player o accions
	else {
		if (player->playerState == RESTART) {
			int newMap = checkpointMap;
			if (!changeMap && currentMap != 0 && newMap != currentMap) {
				changeMap = true;
				timer = 0;
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
				timer = 0;
			}
			else {
				timer = 0;
				int changeMapInfoId = map[currentMap]->changeMapInfoId;
				glm::ivec3 newMapInfo = map[currentMap]->changeMapInfo[changeMapInfoId];
				player->changeMap(glm::vec2(newMapInfo.y, newMapInfo.z) - glm::vec2(player->getPlayerSize()));
				currentMap = newMapInfo.x;
				player->setTileMap(map[currentMap]);
				player->playerState = NONE;
				changeMap = false;
			}
		}
		else if (player->playerState == CHECKPOINT) {
			if (currentMap != checkpointMap) map[checkpointMap]->activatedCheckpoint = glm::ivec2(-1, -1);
			checkpointMap = currentMap;
			player->checkpoint = (map[checkpointMap]->activatedCheckpoint*tileSize - player->getPlayerSize()) + tileSize;
			player->playerState = NONE;
		}
		player->update(deltaTime);
	}
	Audio::instance().update();
	map[currentMap]->update(deltaTime, texProgram,
		player->posPlayer, player->getPlayerSize(),
		player->getPlayerCollision(), player->left,
		&player->playerState, Game::instance().gravity);
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
	// Menu
	if (!menu && !instructions) {
		map[currentMap]->render();
		map[currentMap]->renderEnemies();
		player->render();
		// Tornar a fer set de modelview i texCoordDispl ja que player->render() les modifica.
		texProgram.setUniformMatrix4f("modelview", modelview);
		texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
		map[currentMap]->renderFront();
	}
	map[CANTI_TV]->renderBackground();
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



