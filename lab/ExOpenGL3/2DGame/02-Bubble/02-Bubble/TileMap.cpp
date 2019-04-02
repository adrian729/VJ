#include <iostream>
#include <fstream>
#include <sstream>
#include "TileMap.h"
#include "Globals.h"

using namespace std;


#define CONVEYOR_STEP 3
#define DIV_FPS 6


TileMap *TileMap::createTileMap(const string &levelName, const int &level, const glm::vec2 &minCoords, ShaderProgram &program) {
	TileMap *map = new TileMap(levelName, level, minCoords, program);
	return map;
}

TileMap::TileMap(const string &levelName, const int &level, const glm::vec2 &minCoords, ShaderProgram &program) {

	this->level = level;
	changeMapInfo.push_back(glm::ivec3(4, 25, 26));
	changeMapInfoId = changeMapInfo.size();

	animationCount = 0;

	this->minCoords = minCoords;
	// -4 i +4 per donar "efecte 2.5D" una mica
	this->minCoords.x -= 4;
	this->minCoords.y += 4;
	loadLevel(levelName, program);
	prepareArrays(program);
}

TileMap::~TileMap() {
	if (map != NULL)
		delete map;
	if (frontMap != NULL)
		delete frontMap;
}

void TileMap::update(int deltaTime, ShaderProgram &program,
	const glm::ivec2 &playerPos, const glm::ivec2 &playerSize,
	const glm::ivec3 &playerCollision, const bool &playerLeft,
	int *playerState, const bool &g) {

	animationCount++;
	if (animationCount > FPS / DIV_FPS) {
		animationCount = 0;
		for (int j = mapSize.y - 1; j >= 0; j--) {
			for (int i = 0; i < mapSize.x; i++) {
				int type = mapType[j * mapSize.x + i];
				if (type > 0 && type < CHANGE_MAP_TILE) {
					if (type == CONTROL_POINT) {
						if (i == activatedCheckpoint.x && j == activatedCheckpoint.y) {
							type = ACTIVATED_CONTROL_POINT;
							mapType[j * mapSize.x + i] = ACTIVATED_CONTROL_POINT;
						}
					}
					else if (type == ACTIVATED_CONTROL_POINT) {
						if (i != activatedCheckpoint.x || j != activatedCheckpoint.y) {
							type = CONTROL_POINT;
							mapType[j * mapSize.x + i] = CONTROL_POINT;
						}
					}
					// tileMap
					map[j * mapSize.x + i] = type + (map[j * mapSize.x + i] - type + 1) % 8;
					// frontMap
					frontMap[j * mapSize.x + i] = type + (map[j * mapSize.x + i] - type + 1) % 8;
				}
			}
		}
	}

	glGenVertexArrays(1, &vaoTileMap);
	glGenBuffers(1, &vboTileMap);
	glGenVertexArrays(1, &vaoFront);
	glGenBuffers(1, &vboFront);
	prepareArraysTiles(map, vaoTileMap, vboTileMap, tileBlockSize, initTile, tilesheet, tilesheetSize, tileTexSize, program, false);
	prepareArraysTiles(frontMap, vaoFront, vboFront, frontBlockSize, initFrontTile, frontTileSheet, frontTilesheetSize, frontTexSize, program, true);
	
	int playerStartX, playerStartY, playerEndX, playerEndY;
	playerStartX = playerPos.x;
	playerEndX = playerStartX + playerSize.x;
	playerStartY = playerPos.y;
	playerEndY = playerStartY + playerSize.y;
	if (playerLeft) {
		playerStartX += playerCollision.r;
		playerEndX -= playerCollision.b;
	}
	else {
		playerStartX += playerCollision.b;
		playerEndX -= playerCollision.r;
	}
	if (g) {
		playerStartY += playerCollision.g;
	}
	else {
		playerEndY -= playerCollision.g;
	}

	// Check collision player/enemy
	int enemyStartX, enemyStartY, enemyEndX, enemyEndY;
	for (int i = 0; i < enemies.size(); i++) {
		enemies[i]->update(deltaTime);

		glm::ivec3 enemyStartCollision = enemies[i]->getStartCollision();
		enemyStartX = enemies[i]->posEnemy.x;
		enemyEndX = enemyStartX + enemies[i]->enemySize.x;
		enemyStartY = enemies[i]->posEnemy.y;
		enemyEndY = enemyStartY + enemies[i]->enemySize.y;
		if (enemies[i]->s < 0) {
			enemyStartX += enemyStartCollision.r;
			enemyEndX -= enemyStartCollision.b;
		}
		else {
			enemyStartX += enemyStartCollision.b;
			enemyEndX -= enemyStartCollision.r;
		}
		if (enemies[i]->upDown == 0) {
			enemyStartY += enemyStartCollision.g;
		}
		else {
			enemyEndY -= enemyStartCollision.g;
		}

		if (playerEndX > enemyStartX &&
			playerStartX < enemyEndX &&
			playerEndY > enemyStartY &&
			playerStartY < enemyEndY) {
			*playerState = DEAD;
			return;
		}
	}
}

void TileMap::renderBackground() {
	glEnable(GL_TEXTURE_2D);
	backgroundImage.use();
	glBindVertexArray(vaoBack);
	glEnableVertexAttribArray(posLocation);
	glEnableVertexAttribArray(texCoordLocation);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisable(GL_TEXTURE_2D);
	//glGenVertexArrays(1, &vaoBack);
	//glGenBuffers(1, &vboBack);
}

void TileMap::render() {
	glEnable(GL_TEXTURE_2D);
	tilesheet.use();
	glBindVertexArray(vaoTileMap);
	glEnableVertexAttribArray(posLocation);
	glEnableVertexAttribArray(texCoordLocation);
	glDrawArrays(GL_TRIANGLES, 0, 6 * mapSize.x * mapSize.y);
	glDisable(GL_TEXTURE_2D);
	//glDeleteBuffers(1, &vaoTileMap);
	//glGenVertexArrays(1, &vaoTileMap);
	//glGenBuffers(1, &vboTileMap);
}

void TileMap::renderFront() {
	glEnable(GL_TEXTURE_2D);
	frontTileSheet.use();
	glBindVertexArray(vaoFront);
	glEnableVertexAttribArray(posLocation);
	glEnableVertexAttribArray(texCoordLocation);
	glDrawArrays(GL_TRIANGLES, 0, 6 * mapSize.x * mapSize.y);
	glDisable(GL_TEXTURE_2D);
	//glDeleteBuffers(1, &vaoFront);
	//glGenVertexArrays(1, &vaoFront);
	//glGenBuffers(1, &vboFront);
}

void TileMap::renderLights() const {
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
	//lightsImage.use();
	//glBindVertexArray(vaoLights);
	//glEnableVertexAttribArray(posLocation);
	//glEnableVertexAttribArray(texCoordLocation);
	//glDrawArrays(GL_TRIANGLES, 0, 6);
	//glDisable(GL_BLEND);
}

void TileMap::renderEnemies() const {
	for (int i = 0; i < enemies.size(); i++) {
		enemies[i]->render();
	}
}

void TileMap::free() {
	glDeleteBuffers(1, &vboBack);
	glDeleteBuffers(1, &vboTileMap);
	glDeleteBuffers(1, &vboFront);
	glDeleteBuffers(1, &vboLights);
}

bool TileMap::loadLevel(const string &levelName, ShaderProgram &program) {
	string levelFile = "levels/" + levelName + "/level";
	ifstream fin;
	string line;
	stringstream sstream;
	glm::ivec2 initPlayerTile;

	fin.open(levelFile.c_str());
	if (!fin.is_open())
		return false;
	sstream.clear(); // idk why, without this it doesn't work
	getline(fin, line);
	sstream.str(line);
	sstream >> mapSize.x >> mapSize.y;
	getline(fin, line);
	sstream.str(line);
	sstream >> tileSize;
	getline(fin, line);
	sstream.str(line);
	sstream >> initPlayerTile.x >> initPlayerTile.y;
	fin.close();

	mapPixels = mapSize * tileSize;

	initPlayerPos = initPlayerTile;
	initPlayerPos += 1;
	initPlayerPos *= tileSize;

	bool breturn = true;
	cout << "level: " << levelName << endl;
	breturn = breturn && loadBackground("levels/" + levelName + "/background");
	cout << "bck: " << breturn << endl;
	breturn = breturn && loadTiles("levels/" + levelName + "/tileMap");
	cout << "tiles: " << breturn << endl;
	breturn = breturn && loadFrontTiles("levels/" + levelName + "/frontTiles");
	cout << "front: " << breturn << endl;
	// breturn = breturn && loadLights("levels/" + levelName + "/lights");
	// cout << "lights: " << breturn << endl;
	breturn = breturn && loadEnemies("levels/" + levelName + "/enemies", program);
	cout << "enemies: " << breturn << endl;

	return breturn;
}

bool TileMap::loadBackground(const string &backgroundFile) {
	ifstream fin;
	string line, backgroundImageFile;
	stringstream sstream;

	fin.open(backgroundFile.c_str());
	if (!fin.is_open())
		return false;
	getline(fin, line);
	sstream.str(line);
	sstream >> backgroundImageFile;
	backgroundImage.loadFromFile(backgroundImageFile, TEXTURE_PIXEL_FORMAT_RGBA);
	backgroundImage.setWrapS(GL_CLAMP_TO_EDGE);
	backgroundImage.setWrapT(GL_CLAMP_TO_EDGE);
	backgroundImage.setMinFilter(GL_NEAREST);
	backgroundImage.setMagFilter(GL_NEAREST);
	fin.close();

	return true;
}

bool TileMap::loadTiles(const string &tilesFile) {
	ifstream fin;
	string line, tilesheetFile;
	stringstream sstream;
	char tile;

	// A partir de la tile 16 son tiles "especials".
	// 17 - 24 spikes
	// 25 - 32 cinta left
	// 33 - 40 cinta right

	// Tiles
	fin.open(tilesFile.c_str());
	if (!fin.is_open())
		return false;
	/*
	getline(fin, line);
	if (line.compare(0, 7, "TILEMAP") != 0)
		return false;
	*/
	getline(fin, line);
	sstream.str(line);
	sstream >> tileBlockSize.x >> tileBlockSize.y;
	getline(fin, line);
	sstream.str(line);
	sstream >> initTile.x >> initTile.y;
	getline(fin, line);
	sstream.str(line);
	sstream >> tilesheetFile;
	tilesheet.loadFromFile(tilesheetFile, TEXTURE_PIXEL_FORMAT_RGBA);
	tilesheet.setWrapS(GL_CLAMP_TO_EDGE);
	tilesheet.setWrapT(GL_CLAMP_TO_EDGE);
	tilesheet.setMinFilter(GL_NEAREST);
	tilesheet.setMagFilter(GL_NEAREST);
	getline(fin, line);
	sstream.str(line);
	sstream >> tilesheetSize.x >> tilesheetSize.y;
	tileTexSize = glm::vec2(1.f / tilesheetSize.x, 1.f / tilesheetSize.y);

	map = new int[mapSize.x * mapSize.y];
	mapType = new int[mapSize.x * mapSize.y];

	string tmp;
	glm::ivec3 changeTileInfo;
	for (int j = 0; j < mapSize.y; j++) {
		for (int i = 0; i < mapSize.x; i++) {
			string tileID = "";
			fin.get(tile);
			while (tile != ',') {
				// if changeMap Tile
				if (tile == 'c') {
					changeTileInfo = glm::ivec3();
					// Map ID
					tmp = "";
					fin.get(tile);
					while (tile != ';') {
						tmp += tile;
						fin.get(tile);
					}
					changeTileInfo.r = stoi(tmp);
					// X tile coord
					tmp = "";
					fin.get(tile);
					while (tile != ';') {
						tmp += tile;
						fin.get(tile);
					}
					changeTileInfo.g = (stoi(tmp) + 1) * tileSize;
					// Y tile coord
					tmp = "";
					fin.get(tile);
					while (tile != ',') {
						tmp += tile;
						fin.get(tile);
					}
					changeTileInfo.b = (stoi(tmp) + 1) * tileSize;
					tileID = to_string(CHANGE_MAP_TILE + changeMapInfo.size());
					changeMapInfo.push_back(changeTileInfo);
				}
				else if (tile == 'm') {
					// startMP, endMP, positionMP, velocityMP, sMP;
					// Tile Type
					tmp = "";
					fin.get(tile);
					while (tile != ';') {
						tmp += tile;
						fin.get(tile);
					}
					typeMP.push_back(stoi(tmp));
					// Direction
					tmp = "";
					fin.get(tile);
					while (tile != ';') {
						tmp += tile;
						fin.get(tile);
					}
					if (stoi(tmp) > 0) sMP.push_back(1);
					else sMP.push_back(-1);
					// Start
					tmp = "";
					fin.get(tile);
					while (tile != ';') {
						tmp += tile;
						fin.get(tile);
					}
					startMP.push_back(stoi(tmp)*tileSize);
					// End
					tmp = "";
					fin.get(tile);
					while (tile != ';') {
						tmp += tile;
						fin.get(tile);
					}
					endMP.push_back(stoi(tmp)*tileSize);
					// Velocity
					tmp = "";
					fin.get(tile);
					while (tile != ',') {
						tmp += tile;
						fin.get(tile);
					}
					velocityMP.push_back(stoi(tmp));

					tileID = to_string(PLATFORM_TILE + positionMP.size());
					positionMP.push_back(i * tileSize);
					yMP.push_back(j * tileSize);
				}
				else {
					tileID += tile;
					fin.get(tile);
				}
			}
			int id = stoi(tileID);

			map[j*mapSize.x + i] = id;
			if (id >= PLATFORM_TILE) mapType[j*mapSize.x + i] = PLATFORM_TILE;
			else if (id >= CHANGE_MAP_TILE) mapType[j*mapSize.x + i] = CHANGE_MAP_TILE;
			else if (id > 16) mapType[j*mapSize.x + i] = ((id - 1) / 8) * 8 + 1;
			else mapType[j*mapSize.x + i] = 0;
		}
		fin.get(tile);
#ifndef _WIN32
		fin.get(tile);
#endif
	}
	fin.close();

	return true;
}

bool TileMap::loadFrontTiles(const string &frontTilesFile) {
	ifstream fin;
	string line, frontTilesheetFile;
	stringstream sstream;
	char tile;

	// A partir de la tile 16 son tiles "especials".
	// 17 - 24 spikes
	// 25 - 32 cinta left
	// 33 - 41 cinta right

	fin.open(frontTilesFile.c_str());
	if (!fin.is_open())
		return false;
	getline(fin, line);
	sstream.str(line);
	sstream >> frontBlockSize.x >> frontBlockSize.y;
	getline(fin, line);
	sstream.str(line);
	sstream >> initFrontTile.x >> initFrontTile.y;
	getline(fin, line);
	sstream.str(line);
	sstream >> frontTilesheetFile;
	frontTileSheet.loadFromFile(frontTilesheetFile, TEXTURE_PIXEL_FORMAT_RGBA);
	frontTileSheet.setWrapS(GL_CLAMP_TO_EDGE);
	frontTileSheet.setWrapT(GL_CLAMP_TO_EDGE);
	frontTileSheet.setMinFilter(GL_NEAREST);
	frontTileSheet.setMagFilter(GL_NEAREST);
	getline(fin, line);
	sstream.str(line);
	sstream >> frontTilesheetSize.x >> frontTilesheetSize.y;
	frontTexSize = glm::vec2(1.f / frontTilesheetSize.x, 1.f / frontTilesheetSize.y);

	frontMap = new int[mapSize.x * mapSize.y];
	for (int j = 0; j < mapSize.y; j++) {
		for (int i = 0; i < mapSize.x; i++) {
			string tileID = "";
			fin.get(tile);
			if (tile == 'm') {
				frontMap[j*mapSize.x + i] = map[j*mapSize.x + i];
				fin.get(tile);
			}
			else {
				while (tile != ',') {
					tileID += tile;
					fin.get(tile);
				}
				frontMap[j*mapSize.x + i] = stoi(tileID);
			}
		}
		fin.get(tile);
#ifndef _WIN32
		fin.get(tile);
#endif
	}
	fin.close();

	return true;
}

bool TileMap::loadLights(const string &lightsFile) {
	ifstream fin;
	string line, lightsImageFile;
	stringstream sstream;

	fin.open(lightsFile.c_str());
	if (!fin.is_open())
		return false;
	getline(fin, line);
	sstream.str(line);
	sstream >> lightsImageFile;
	lightsImage.loadFromFile(lightsImageFile, TEXTURE_PIXEL_FORMAT_RGBA);
	lightsImage.setWrapS(GL_CLAMP_TO_EDGE);
	lightsImage.setWrapT(GL_CLAMP_TO_EDGE);
	lightsImage.setMinFilter(GL_NEAREST);
	lightsImage.setMagFilter(GL_NEAREST);
	fin.close();

	return true;
}

bool TileMap::loadEnemies(const string &enemiesFile, ShaderProgram &program) {
	ifstream fin;
	string line;
	stringstream sstream;
	int numEnemies;
	char param;

	fin.open(enemiesFile.c_str());
	if (!fin.is_open())
		return false;
	getline(fin, line);
	sstream.str(line);
	sstream >> numEnemies;

	for (int i = 0; i < numEnemies; i++) {
		string paramValue;

		// type
		paramValue = "";
		fin.get(param);
		while (param != ',') {
			paramValue += param;
			fin.get(param);
		}
		int type = stoi(paramValue);

		// direction
		paramValue = "";
		fin.get(param);
		while (param != ',') {
			paramValue += param;
			fin.get(param);
		}
		int dir = stoi(paramValue);

		// position p
		paramValue = "";
		fin.get(param);
		while (param != ',') {
			paramValue += param;
			fin.get(param);
		}
		int p = stoi(paramValue);

		// start pos a
		paramValue = "";
		fin.get(param);
		while (param != ',') {
			paramValue += param;
			fin.get(param);
		}
		int a = stoi(paramValue);

		// end pos b
		paramValue = "";
		fin.get(param);
		while (param != ',') {
			paramValue += param;
			fin.get(param);
		}
		int b = stoi(paramValue);

		// up or down sprites
		paramValue = "";
		fin.get(param);
		while (param != ';') {
			paramValue += param;
			fin.get(param);
		}
		int upDown = stoi(paramValue);

		int xStart, yStart, xEnd, yEnd;
		bool direction = dir == 0;
		if (direction) {
			yStart = yEnd = p;
			xStart = a;
			xEnd = b;
		}
		else {
			xStart = xEnd = p;
			yStart = a;
			yEnd = b;
		}

		glm::ivec2 start = (glm::ivec2(xStart, yStart) - 1)*tileSize;
		glm::ivec2 end = (glm::ivec2(xEnd, yEnd) - 1)*tileSize;

		enemies.push_back(new Enemy());
		enemies[i]->init(glm::ivec2(SCREEN_X, SCREEN_Y), program,
			type, direction, start, end, upDown);
		fin.get(param);
#ifndef _WIN32
		fin.get(param);
#endif
	}

	fin.close();

	return true;
}

void TileMap::prepareArrays(ShaderProgram &program) {

	glGenVertexArrays(1, &vaoBack);
	glGenBuffers(1, &vboBack);
	glGenVertexArrays(1, &vaoTileMap);
	glGenBuffers(1, &vboTileMap);
	glGenVertexArrays(1, &vaoFront);
	glGenBuffers(1, &vboFront);

	prepareArraysBackground(program);
	prepareArraysTiles(map, vaoTileMap, vboTileMap, tileBlockSize, initTile, tilesheet, tilesheetSize, tileTexSize, program, false);
	prepareArraysTiles(frontMap, vaoFront, vboFront, frontBlockSize, initFrontTile, frontTileSheet, frontTilesheetSize, frontTexSize, program, true);

	//prepareArraysLights(program);
}

void TileMap::prepareArraysBackground(ShaderProgram &program) {
	glm::vec2 pos, texCoordTile[2];
	vector<float> vertices;

	pos = glm::vec2(minCoords.x, minCoords.y);
	texCoordTile[0] = glm::vec2(0.f);
	texCoordTile[1] = glm::vec2(1.f);
	// First triangle
	vertices.push_back(minCoords.x); vertices.push_back(minCoords.y);
	vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
	vertices.push_back(minCoords.x + mapPixels.x); vertices.push_back(minCoords.y);
	vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[0].y);
	vertices.push_back(minCoords.x + mapPixels.x); vertices.push_back(minCoords.y + mapPixels.y);
	vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
	// Second triangle
	vertices.push_back(minCoords.x); vertices.push_back(minCoords.y);
	vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
	vertices.push_back(minCoords.x + mapPixels.x); vertices.push_back(minCoords.y + mapPixels.y);
	vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
	vertices.push_back(minCoords.x); vertices.push_back(minCoords.y + mapPixels.y);
	vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[1].y);

	glGenVertexArrays(1, &vaoBack);
	glBindVertexArray(vaoBack);
	glGenBuffers(1, &vboBack);
	glBindBuffer(GL_ARRAY_BUFFER, vboBack);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	posLocation = program.bindVertexAttribute("position", 2, 4 * sizeof(float), 0);
	texCoordLocation = program.bindVertexAttribute("texCoord", 2, 4 * sizeof(float), (void *)(2 * sizeof(float)));
}

//void TileMap::prepareArraysLights(ShaderProgram &program) {
//	glm::vec2 pos, texCoordTile[2];
//	vector<float> vertices;
//
//	pos = glm::vec2(minCoords.x, minCoords.y);
//	texCoordTile[0] = glm::vec2(0.f);
//	texCoordTile[1] = glm::vec2(1.f);
//	// First triangle
//	vertices.push_back(minCoords.x); vertices.push_back(minCoords.y);
//	vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
//	vertices.push_back(minCoords.x + mapPixels.x); vertices.push_back(minCoords.y);
//	vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[0].y);
//	vertices.push_back(minCoords.x + mapPixels.x); vertices.push_back(minCoords.y + mapPixels.y);
//	vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
//	// Second triangle
//	vertices.push_back(minCoords.x); vertices.push_back(minCoords.y);
//	vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
//	vertices.push_back(minCoords.x + mapPixels.x); vertices.push_back(minCoords.y + mapPixels.y);
//	vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
//	vertices.push_back(minCoords.x); vertices.push_back(minCoords.y + mapPixels.y);
//	vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[1].y);
//
//	glGenVertexArrays(1, &vaoLights);
//	glBindVertexArray(vaoLights);
//	glGenBuffers(1, &vboLights);
//	glBindBuffer(GL_ARRAY_BUFFER, vboLights);
//	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), &vertices[0], GL_STATIC_DRAW);
//	posLocation = program.bindVertexAttribute("position", 2, 4 * sizeof(float), 0);
//	texCoordLocation = program.bindVertexAttribute("texCoord", 2, 4 * sizeof(float), (void *)(2 * sizeof(float)));
//}

void TileMap::prepareArraysTiles(int *tileMap, GLuint &vao, GLuint &vbo, const glm::ivec2 &blockSize, const glm::ivec2 &initPos,
	const Texture &sheet, const glm::ivec2 &sheetSize, const glm::vec2 &texSize, ShaderProgram &program, const bool &front) {

	int tile, nTiles = 0;
	glm::vec2 posTile, texCoordTile[2], halfTexel;
	vector<float> vertices;

	//halfTexel = glm::vec2(0.5f / sheet.width(), 0.5f / sheet.height());

	// Hem d'anar en compte en quin ordre afegim els tiles ja que si solapen l'ultim pintat queda per sobre
	for (int j = mapSize.y - 1; j >= 0; j--) {
		for (int i = 0; i < mapSize.x; i++) {
			tile = tileMap[j * mapSize.x + i];
			if (tile >= PLATFORM_TILE) {
				int MPID = tile - PLATFORM_TILE;
				tile = typeMP[MPID];
				int posX = positionMP[MPID];
				if (!front) {
					posX += (sMP[MPID] * velocityMP[MPID]);
					if (posX < startMP[MPID]) {
						posX = startMP[MPID];
						sMP[MPID] = 1;
					}
					else if (posX > endMP[MPID]) {
						posX = endMP[MPID];
						sMP[MPID] = -1;
					}
				}
				positionMP[MPID] = posX;
				// Non-empty tile
				nTiles++;
				posTile = glm::vec2(minCoords.x + posX - initPos.x, minCoords.y + j * tileSize - initPos.y);
				texCoordTile[0] = glm::vec2(float((tile - 1) % sheetSize.x) / sheetSize.x, float((tile - 1) / sheetSize.y) / sheetSize.y);
				texCoordTile[1] = texCoordTile[0] + texSize;
				//texCoordTile[0] += halfTexel;
				texCoordTile[1] -= halfTexel;
				// First triangle
				vertices.push_back(posTile.x); vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + blockSize.x); vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + blockSize.x); vertices.push_back(posTile.y + blockSize.y);
				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
				// Second triangle
				vertices.push_back(posTile.x); vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + blockSize.x); vertices.push_back(posTile.y + blockSize.y);
				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
				vertices.push_back(posTile.x); vertices.push_back(posTile.y + blockSize.y);
				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[1].y);
			}
			else if (tile != 0 && tile < CHANGE_MAP_TILE) {
				// Non-empty tile
				nTiles++;
				posTile = glm::vec2(minCoords.x + i * tileSize - initPos.x, minCoords.y + j * tileSize - initPos.y);
				texCoordTile[0] = glm::vec2(float((tile - 1) % sheetSize.x) / sheetSize.x, float((tile - 1) / sheetSize.y) / sheetSize.y);
				texCoordTile[1] = texCoordTile[0] + texSize;
				//texCoordTile[0] += halfTexel;
				texCoordTile[1] -= halfTexel;
				// First triangle
				vertices.push_back(posTile.x); vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + blockSize.x); vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + blockSize.x); vertices.push_back(posTile.y + blockSize.y);
				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
				// Second triangle
				vertices.push_back(posTile.x); vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + blockSize.x); vertices.push_back(posTile.y + blockSize.y);
				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
				vertices.push_back(posTile.x); vertices.push_back(posTile.y + blockSize.y);
				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[1].y);
			}
		}
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	if (vertices.size() > 0)
		glBufferData(GL_ARRAY_BUFFER, 24 * nTiles * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	posLocation = program.bindVertexAttribute("position", 2, 4 * sizeof(float), 0);
	texCoordLocation = program.bindVertexAttribute("texCoord", 2, 4 * sizeof(float), (void *)(2 * sizeof(float)));
}

// Collision tests for axis aligned bounding boxes.
// Position corrected if a collision is detected.

bool TileMap::collisionMoveLeft(const glm::ivec2 &pos, const glm::ivec2 &size, glm::ivec2 *position, int &changeState) {
	int x, y0, y1;
	x = pos.x / tileSize;
	y0 = pos.y / tileSize;
	y1 = (pos.y + size.y - 1) / tileSize;

	bool collisionMade = false;
	glm::ivec2 check = glm::ivec2(-1, -1);
	glm::ivec2 newPosition = *position;
	for (int y = y0; y <= y1; y++) {
		if (map[y*mapSize.x + x] != 0 && mapType[y*mapSize.x + x] < PLATFORM_TILE) {
			if (!collisionMade && mapType[y*mapSize.x + x] != CONTROL_POINT && mapType[y*mapSize.x + x] != ACTIVATED_CONTROL_POINT) {
				newPosition.x = (x + 1)*tileSize;
				collisionMade = true;
			}
			if (mapType[y*mapSize.x + x] == SPIKE) {
				changeState = DEAD;
			}
			else if (mapType[y*mapSize.x + x] == CONTROL_POINT) {
				changeState = CHECKPOINT;
				check = glm::ivec2(x, y);
			}
			else if (mapType[y*mapSize.x + x] >= CHANGE_MAP_TILE && changeState < DEAD) {
				changeState = CHANGE_MAP;
				changeMapInfoId = map[y*mapSize.x + x] - CHANGE_MAP_TILE;
				return true;
			}
		}
	}
	if (changeState == CHECKPOINT && check.x > 0) activatedCheckpoint = check;
	(*position).x = newPosition.x;

	// Check collision player/platform
	int mpStartX, mpY, mpEndX, playerX;
	y0 = (*position).y / tileSize;
	y1 = ((*position).y + size.y - 1) / tileSize;
	playerX = (*position).x;
	for (int i = 0; i < positionMP.size(); i++) {
		mpStartX = positionMP[i];
		mpEndX = positionMP[i] + tileSize - 1;
		mpY = yMP[i] / tileSize;
		for (int y = y0; y <= y1; y++) {
			if (mpY == y) {
				if (playerX >= mpStartX && playerX <= mpEndX) {
					(*position).x = mpEndX + 1;
					playerX = (*position).x + size.x - 1;
					collisionMade = true;
				}
			}
		}
	}

	return collisionMade;
}

bool TileMap::collisionMoveRight(const glm::ivec2 &pos, const glm::ivec2 &size, glm::ivec2 *position, int &changeState) {
	int x, y0, y1;
	x = (pos.x + size.x - 1) / tileSize;
	y0 = pos.y / tileSize;
	y1 = (pos.y + size.y - 1) / tileSize;

	bool collisionMade = false;
	glm::ivec2 check = glm::ivec2(-1, -1);
	glm::ivec2 newPosition = *position;
	for (int y = y0; y <= y1; y++) {
		if (map[y*mapSize.x + x] != 0 && mapType[y*mapSize.x + x] < PLATFORM_TILE) {
			if (!collisionMade && mapType[y*mapSize.x + x] != CONTROL_POINT && mapType[y*mapSize.x + x] != ACTIVATED_CONTROL_POINT) {
				newPosition.x = x * tileSize - size.x;
				collisionMade = true;
			}
			if (mapType[y*mapSize.x + x] == SPIKE) {
				changeState = DEAD;
				return true;
			}
			else if (mapType[y*mapSize.x + x] == CONTROL_POINT) {
				changeState = CHECKPOINT;
				check = glm::ivec2(x, y);
			}
			else if (mapType[y*mapSize.x + x] >= CHANGE_MAP_TILE && changeState < DEAD) {
				changeState = CHANGE_MAP;
				changeMapInfoId = map[y*mapSize.x + x] - CHANGE_MAP_TILE;
				return true;
			}
		}
	}
	if (changeState == CHECKPOINT && check.x > 0) activatedCheckpoint = check;
	(*position).x = newPosition.x;

	// Check collision player/platform
	int mpStartX, mpY, mpEndX, playerX;
	y0 = (*position).y / tileSize;
	y1 = ((*position).y + size.y - 1) / tileSize;
	playerX = (*position).x + size.x - 1;
	for (int i = 0; i < positionMP.size(); i++) {
		mpStartX = positionMP[i];
		mpEndX = positionMP[i] + tileSize - 1;
		mpY = yMP[i] / tileSize;
		for (int y = y0; y <= y1; y++) {
			if (mpY == y) {
				if (playerX >= mpStartX && playerX <= mpEndX) {
					(*position).x = mpStartX - size.x;
					playerX = (*position).x + size.x - 1;
					collisionMade = true;
				}
			}
		}
	}

	return collisionMade;
}

bool TileMap::collisionMoveDown(const glm::ivec2 &pos, const glm::ivec2 &size, glm::ivec2 *position, int &changeState) {
	int x0, x1, y;
	x0 = pos.x / tileSize;
	x1 = (pos.x + size.x - 1) / tileSize;
	y = (pos.y + size.y - 1) / tileSize;

	bool collisionMade = false;
	int conveyor_step = 0;
	glm::ivec2 check = glm::ivec2(-1, -1);
	glm::ivec2 newPosition = *position;
	for (int x = x0; x <= x1; x++) {
		if (map[y*mapSize.x + x] != 0 && mapType[y*mapSize.x + x] < PLATFORM_TILE) {
			if (!collisionMade && mapType[y*mapSize.x + x] != CONTROL_POINT && mapType[y*mapSize.x + x] != ACTIVATED_CONTROL_POINT) {
				newPosition.y = tileSize * y - size.y;
				collisionMade = true;
			}
			if (mapType[y*mapSize.x + x] == SPIKE) {
				changeState = DEAD;
				return true;
			}
			else if (mapType[y*mapSize.x + x] == CONVEYOR_LEFT) {
				conveyor_step = -CONVEYOR_STEP;
			}
			else if (mapType[y*mapSize.x + x] == CONVEYOR_RIGHT) {
				conveyor_step = CONVEYOR_STEP;
			}
			else if (mapType[y*mapSize.x + x] == CONTROL_POINT) {
				changeState = CHECKPOINT;
				check = glm::ivec2(x, y);
			}
			else if (mapType[y*mapSize.x + x] >= CHANGE_MAP_TILE) {
				changeState = CHANGE_MAP;
				changeMapInfoId = map[y*mapSize.x + x] - CHANGE_MAP_TILE;
				return true;
			}
		}
	}
	(*position).x += conveyor_step;
	(*position).y = newPosition.y;

	if (changeState == CHECKPOINT && check.x > 0) activatedCheckpoint = check;
	if (conveyor_step < 0) collisionMoveLeft(*position, size, position, changeState);
	else if (conveyor_step > 0) collisionMoveRight(*position, size, position, changeState);

	// Check collision player/platform (up and down)
	int mpStartX, mpY, mpEndX;
	for (int i = 0; i < positionMP.size(); i++) {
		mpStartX = positionMP[i];
		mpY = yMP[i] / tileSize;
		mpEndX = positionMP[i] + tileSize;
		if (mpY == ((*position).y + size.y - 1) / tileSize) {
			if ((*position).x < mpEndX && ((*position).x + size.x) > mpStartX) {
				collisionMade = true;
				(*position).x += (sMP[i] * velocityMP[i]);
				(*position).y = yMP[i] - size.y - 1;

				if (sMP[i] < 0) collisionMoveLeft(*position, size, position, changeState);
				else if (sMP[i] > 0) collisionMoveRight(*position, size, position, changeState);

				collisionMade = true;
			}
		}
	}

	return collisionMade;
}

bool TileMap::collisionMoveUp(const glm::ivec2 &pos, const glm::ivec2 &size, glm::ivec2 *position, int &changeState) {
	int x0, x1, y;
	x0 = pos.x / tileSize;
	x1 = (pos.x + size.x - 1) / tileSize;
	y = pos.y / tileSize;

	bool collisionMade = false;
	int conveyor_step = 0;
	glm::ivec2 check = glm::ivec2(-1, -1);
	glm::ivec2 newPosition = *position;
	for (int x = x0; x <= x1; x++) {
		if (map[y*mapSize.x + x] != 0 && mapType[y*mapSize.x + x] < PLATFORM_TILE) {
			if (!collisionMade && mapType[y*mapSize.x + x] != CONTROL_POINT && mapType[y*mapSize.x + x] != ACTIVATED_CONTROL_POINT) {
				newPosition.y = tileSize * (y + 1);
				collisionMade = true;
			}
			if (mapType[y*mapSize.x + x] == SPIKE) {
				changeState = DEAD;
				return true;
			}
			else if (mapType[y*mapSize.x + x] == CONVEYOR_LEFT) {
				conveyor_step = -CONVEYOR_STEP;
			}
			else if (mapType[y*mapSize.x + x] == CONVEYOR_RIGHT) {
				conveyor_step = CONVEYOR_STEP;
			}
			else if (mapType[y*mapSize.x + x] == CONTROL_POINT) {
				changeState = CHECKPOINT;
				check = glm::ivec2(x, y);
			}
			else if (mapType[y*mapSize.x + x] >= CHANGE_MAP_TILE) {
				changeState = CHANGE_MAP;
				changeMapInfoId = map[y*mapSize.x + x] - CHANGE_MAP_TILE;
				return true;
			}
		}
	}
	(*position).x += conveyor_step;
	(*position).y = newPosition.y;

	if (changeState == CHECKPOINT && check.x > 0) activatedCheckpoint = check;
	if (conveyor_step < 0) collisionMoveLeft(*position, size, position, changeState);
	else if (conveyor_step > 0) collisionMoveRight(*position, size, position, changeState);

	// Check collision player/platform (up and down)
	int mpStartX, mpY, mpEndX;
	for (int i = 0; i < positionMP.size(); i++) {
		mpStartX = positionMP[i];
		mpY = yMP[i] / tileSize;
		mpEndX = positionMP[i] + tileSize;
		if (mpY == (*position).y / tileSize) {
			if ((*position).x < mpEndX && ((*position).x + size.x) > mpStartX) {
				collisionMade = true;
				(*position).x += (sMP[i] * velocityMP[i]);
				(*position).y = yMP[i] + tileSize;

				if (sMP[i] < 0) collisionMoveLeft(*position, size, position, changeState);
				else if (sMP[i] > 0) collisionMoveRight(*position, size, position, changeState);

				collisionMade = true;
			}
		}
	}

	return collisionMade;
}