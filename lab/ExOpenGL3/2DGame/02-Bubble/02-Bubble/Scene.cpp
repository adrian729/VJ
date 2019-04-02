#include <iostream>
#include <cmath>
#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"


#define TRANSITION_TIME 120

#define INFINITE_LOOP -1
#define MENU_SOUND_VOLUME .1f
#define INSTRUCTIONS_SOUND_VOLUME .2f
#define MAP_SOUND_VOLUME .04f

#define FURY_CURY_SOUND_VOLUME .3f
#define LASER_SOUND_VOLUME .1f

#define CANTI_TV 0

#define INI_MAPS 4


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
	if (map != NULL)
		delete[] map;
	if (soundMap != NULL)
		delete[] soundMap;
	if (player != NULL)
		delete player;
}

void Scene::init() {
	initShaders();

	currentMap = 1;
	currentLevel = 0;
	selection = 0;
	menu = true;
	instructions = false;
	canPress = vector<bool>(256, false);
	canPressSpace = false;
	timer = 0;

	// Canti TV
	map[0] = TileMap::createTileMap("CantiTV", 0, glm::vec2(0, 0), texProgram);
	sceneSize = map[CANTI_TV]->getSceneSize();
	tileSize = map[CANTI_TV]->getTileSize();
	soundMap[0] = "sound/Songs/16Bits-Bran_New_Lovesong.wav";

	actualSound = new audio(Audio::instance().createAudio(soundMap[0]));
	Audio::instance().play(actualSound, INFINITE_LOOP, MENU_SOUND_VOLUME);

	// Menu
	// Select Start
	map[1] = TileMap::createTileMap("MenuStart", 0, glm::vec2(0, 0), texProgram);
	// Select Instructions
	map[2] = TileMap::createTileMap("MenuInstructions", 0, glm::vec2(0, 0), texProgram);

	// Instructions
	map[3] = TileMap::createTileMap("Instructions", 0, glm::vec2(0, 0), texProgram);
	soundMap[3] = "sound/Songs/8Bits-Ride_On_A_Shooting_Star.mp3";


	// Scene 1

	changeLevel = false;

	// Level 0 (4)
	int lvl0 = 0 + INI_MAPS;
	checkpointMap = lvl0 + 1; // level 0 map 1
	map[lvl0] = TileMap::createTileMap("lvl0", lvl0, glm::vec2(SCREEN_X, SCREEN_Y), texProgram); // transition map
	soundMap[lvl0] = "sound/Songs/Little_Busters.mp3";
	// map 1 (5)
	map[lvl0 + 1] = TileMap::createTileMap("lvl0m1", lvl0, glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	//map[lvl0 + 1] = TileMap::createTileMap("lvl3m4", 13, glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	// map 2 (6)
	map[lvl0 + 2] = TileMap::createTileMap("lvl0m2", lvl0, glm::vec2(SCREEN_X, SCREEN_Y), texProgram);

	// Level 1 (7)
	int lvl1 = lvl0 + 2 + 1;
	map[lvl1] = TileMap::createTileMap("lvl1", lvl1, glm::vec2(SCREEN_X, SCREEN_Y), texProgram); // transition map
	soundMap[lvl1] = "sound/Songs/One_Life.mp3";
	// map 1 (8)
	map[lvl1 + 1] = TileMap::createTileMap("lvl1m1", lvl1, glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	// map 2 (9)
	map[lvl1 + 2] = TileMap::createTileMap("lvl1m2", lvl1, glm::vec2(SCREEN_X, SCREEN_Y), texProgram);

	// Level 2 (10)
	int lvl2 = lvl1 + 2 + 1;
	map[lvl2] = TileMap::createTileMap("lvl2", lvl2, glm::vec2(SCREEN_X, SCREEN_Y), texProgram); // transition map
	soundMap[lvl2] = "sound/Songs/Stalker_Goes_To_Babylon.mp3";
	// map 1 (11)
	map[lvl2 + 1] = TileMap::createTileMap("lvl2m1", lvl2, glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	// map 2 (12)
	map[lvl2 + 2] = TileMap::createTileMap("lvl2m2", lvl2, glm::vec2(SCREEN_X, SCREEN_Y), texProgram);


	// Scene 2

	// Level 3 (13)
	int lvl3 = lvl2 + 2 + 1;
	map[lvl3] = TileMap::createTileMap("lvl3", lvl3, glm::vec2(SCREEN_X, SCREEN_Y), texProgram); // transition map
	soundMap[lvl3] = "sound/Songs/Come_Down.mp3";
	// map 1 (14)
	map[lvl3 + 1] = TileMap::createTileMap("lvl3m1", lvl3, glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	// map 2 (15)
	map[lvl3 + 2] = TileMap::createTileMap("lvl3m2", lvl3, glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	// map 2 (16)
	map[lvl3 + 3] = TileMap::createTileMap("lvl3m3", lvl3, glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	// map 2 (17)
	map[lvl3 + 4] = TileMap::createTileMap("lvl3m4", lvl3, glm::vec2(SCREEN_X, SCREEN_Y), texProgram);

	// Level 4 (18)
	int lvl4 = lvl3 + 4 + 1;
	map[lvl4] = TileMap::createTileMap("lvl4", lvl4, glm::vec2(SCREEN_X, SCREEN_Y), texProgram); // transition map
	soundMap[lvl4] = "sound/Songs/Beautiful_Morning_With_You.mp3";
	// map 1 (19)
	map[lvl4 + 1] = TileMap::createTileMap("lvl4m1", lvl4, glm::vec2(SCREEN_X, SCREEN_Y), texProgram);




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

			if (actualSound) Audio::instance().release(actualSound);
			Audio::instance().play(new audio(Audio::instance().createAudio("sound/FuryCury.wav")), 1, FURY_CURY_SOUND_VOLUME);

			if (selection == 0) {

				changeMap = true;
				timer = 2;
				currentLevel = INI_MAPS; // lvl0

				changeLevel = false;

				actualSound = new audio(Audio::instance().createAudio(soundMap[currentLevel]));
				Audio::instance().play(actualSound, INFINITE_LOOP, MAP_SOUND_VOLUME);

				// Mapa "transicio"
				transitionMap = currentLevel + 1; // guardem a quin mapa anirem
				currentMap = currentLevel; // mapa "transicio"

			}
			else {
				instructions = true;
				currentMap = 3;

				actualSound = new audio(Audio::instance().createAudio(soundMap[3]));
				Audio::instance().play(actualSound, INFINITE_LOOP, INSTRUCTIONS_SOUND_VOLUME);
			}
			return;
		}

		if ((Game::instance().getSpecialKey(GLUT_KEY_UP) && canPress[GLUT_KEY_UP])
			|| (Game::instance().getSpecialKey(GLUT_KEY_RIGHT) && canPress[GLUT_KEY_RIGHT])) {

			canPress[GLUT_KEY_UP] = false;
			canPress[GLUT_KEY_RIGHT] = false;
			selection = (selection + 1) % 2;
			currentMap = selection + 1;

			Audio::instance().play(new audio(Audio::instance().createAudio("sound/laser1.wav")), 1, LASER_SOUND_VOLUME);
		}
		else if ((Game::instance().getSpecialKey(GLUT_KEY_DOWN) && canPress[GLUT_KEY_DOWN])
			|| (Game::instance().getSpecialKey(GLUT_KEY_LEFT) && canPress[GLUT_KEY_LEFT])) {

			canPress[GLUT_KEY_DOWN] = false;
			canPress[GLUT_KEY_LEFT] = false;
			selection = (selection - 1 + 2) % 2;
			currentMap = selection + 1;

			Audio::instance().play(new audio(Audio::instance().createAudio("sound/laser1.wav")), 1, LASER_SOUND_VOLUME);
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

			if (actualSound) Audio::instance().release(actualSound);
			Audio::instance().play(new audio(Audio::instance().createAudio("sound/FuryCury.wav")), 1, FURY_CURY_SOUND_VOLUME);

			actualSound = new audio(Audio::instance().createAudio(soundMap[0]));
			Audio::instance().play(actualSound, INFINITE_LOOP, MENU_SOUND_VOLUME);
		}
	}
	// Canviar de mapa
	else if (changeMap && timer < TRANSITION_TIME) {
		timer++;
		if (timer == 1) {

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			int nextMap = currentMap;
			if (player->playerState == RESTART) {
				nextMap = checkpointMap;
			}
			else if (player->playerState == CHANGE_MAP) {
				nextMap = map[currentMap]->changeMapInfo[map[currentMap]->changeMapInfoId].r;
			}
			if (changeLevel) {
				changeLevel = false;
				if (actualSound) Audio::instance().release(actualSound);
				actualSound = new audio(Audio::instance().createAudio(soundMap[currentLevel]));
				Audio::instance().play(actualSound, INFINITE_LOOP, MAP_SOUND_VOLUME);
			}
			else {
				timer = TRANSITION_TIME;
			}
			// Mapa "transicio"
			transitionMap = currentMap; // guardem a quin mapa estavem
			currentMap = currentLevel; // mapa "transicio"
		}

		// map[transitionMap]->free();

		if (timer >= TRANSITION_TIME) {
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
				if (map[currentMap]->level != map[newMap]->level) {
					changeLevel = true;
					currentLevel = map[newMap]->level;
				}
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
				if (map[currentMap]->level != map[newMap]->level) {
					changeLevel = true;
					currentLevel = map[newMap]->level;
				}
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
	if (!menu && !instructions && transitionMap == 0) {
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



