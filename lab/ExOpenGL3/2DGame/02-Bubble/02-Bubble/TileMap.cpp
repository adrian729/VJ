#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "TileMap.h"


using namespace std;


#define SPIKE 17
#define CONVEYOR_LEFT 25
#define CONVEYOR_RIGHT 33

enum PlayerStates {
	NONE, CHANGING_GRAVITY, DEAD
};

TileMap *TileMap::createTileMap(const string &levelName, const glm::vec2 &minCoords, ShaderProgram &program) {
	TileMap *map = new TileMap(levelName, minCoords, program);
	return map;
}

TileMap::TileMap(const string &levelName, const glm::vec2 &minCoords, ShaderProgram &program) {
	this->minCoords = minCoords;
	// -4 i +4 per donar "efecte 2.5D" una mica
	this->minCoords.x -= 4;
	this->minCoords.y += 4;
	loadLevel(levelName);
	prepareArrays(program);
}

TileMap::~TileMap() {
	if (map != NULL)
		delete map;
	if (frontMap != NULL)
		delete frontMap;
}

void TileMap::renderBackground() const {
	glEnable(GL_TEXTURE_2D);
	backgroundImage.use();
	glBindVertexArray(vaoBack);
	glEnableVertexAttribArray(posLocation);
	glEnableVertexAttribArray(texCoordLocation);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisable(GL_TEXTURE_2D);
}

void TileMap::render() const {
	glEnable(GL_TEXTURE_2D);
	tilesheet.use();
	glBindVertexArray(vaoTileMap);
	glEnableVertexAttribArray(posLocation);
	glEnableVertexAttribArray(texCoordLocation);
	glDrawArrays(GL_TRIANGLES, 0, 6 * mapSize.x * mapSize.y);
	glDisable(GL_TEXTURE_2D);
}

void TileMap::renderFront() const {
	glEnable(GL_TEXTURE_2D);
	frontTileSheet.use();
	glBindVertexArray(vaoFront);
	glEnableVertexAttribArray(posLocation);
	glEnableVertexAttribArray(texCoordLocation);
	glDrawArrays(GL_TRIANGLES, 0, 6 * mapSize.x * mapSize.y);
	glDisable(GL_TEXTURE_2D);
}

void TileMap::renderLights() const {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
	lightsImage.use();
	glBindVertexArray(vaoLights);
	glEnableVertexAttribArray(posLocation);
	glEnableVertexAttribArray(texCoordLocation);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisable(GL_BLEND);
}

void TileMap::free() {
	glDeleteBuffers(1, &vboBack);
	glDeleteBuffers(1, &vboTileMap);
	glDeleteBuffers(1, &vboFront);
	glDeleteBuffers(1, &vboLights);
}

bool TileMap::loadLevel(const string &levelName) {
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
	breturn = breturn && loadBackground("levels/" + levelName + "/background");
	cout << "bck: " << breturn << endl;
	breturn = breturn && loadTiles("levels/" + levelName + "/tileMap");
	cout << "tiles: " << breturn << endl;
	breturn = breturn && loadFrontTiles("levels/" + levelName + "/frontTiles");
	cout << "front: " << breturn << endl;
	breturn = breturn && loadLights("levels/" + levelName + "/lights");
	cout << "lights: " << breturn << endl;

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
	// 33 - 41 cinta right

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
	for (int j = 0; j < mapSize.y; j++) {
		for (int i = 0; i < mapSize.x; i++) {
			string tileID = "";
			fin.get(tile);
			while (tile != ',') {
				tileID += tile;
				fin.get(tile);
			}
			map[j*mapSize.x + i] = stoi(tileID);
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
			while (tile != ',') {
				tileID += tile;
				fin.get(tile);
			}
			frontMap[j*mapSize.x + i] = stoi(tileID);
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

void TileMap::prepareArrays(ShaderProgram &program) {
	prepareArraysBackground(program);
	prepareArraysTiles(map, vaoTileMap, vboTileMap, tileBlockSize, initTile, tilesheet, tilesheetSize, tileTexSize, program);
	prepareArraysTiles(frontMap, vaoFront, vboFront, frontBlockSize, initFrontTile, frontTileSheet, frontTilesheetSize, frontTexSize, program);
	prepareArraysLights(program);
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

void TileMap::prepareArraysLights(ShaderProgram &program) {
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

	glGenVertexArrays(1, &vaoLights);
	glBindVertexArray(vaoLights);
	glGenBuffers(1, &vboLights);
	glBindBuffer(GL_ARRAY_BUFFER, vboLights);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	posLocation = program.bindVertexAttribute("position", 2, 4 * sizeof(float), 0);
	texCoordLocation = program.bindVertexAttribute("texCoord", 2, 4 * sizeof(float), (void *)(2 * sizeof(float)));
}

void TileMap::prepareArraysTiles(int *tileMap, GLuint &vao, GLuint &vbo, const glm::ivec2 &blockSize, const glm::ivec2 &initPos,
	const Texture &sheet, const glm::ivec2 &sheetSize, const glm::vec2 &texSize, ShaderProgram &program) {

	int tile, nTiles = 0;
	glm::vec2 posTile, texCoordTile[2], halfTexel;
	vector<float> vertices;

	//halfTexel = glm::vec2(0.5f / sheet.width(), 0.5f / sheet.height());

	// Hem d'anar en compte en quin ordre afegim els tiles ja que si solapen l'ultim pintat queda per sobre
	for (int j = mapSize.y - 1; j >= 0; j--) {
		for (int i = 0; i < mapSize.x; i++) {
			tile = tileMap[j * mapSize.x + i];
			if (tile != 0) {
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
	glBufferData(GL_ARRAY_BUFFER, 24 * nTiles * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	posLocation = program.bindVertexAttribute("position", 2, 4 * sizeof(float), 0);
	texCoordLocation = program.bindVertexAttribute("texCoord", 2, 4 * sizeof(float), (void *)(2 * sizeof(float)));
}

// Collision tests for axis aligned bounding boxes.
// Position corrected if a collision is detected.

bool TileMap::collisionMoveLeft(const glm::ivec2 &pos, const glm::ivec2 &size, glm::ivec2 *position, int &changeState) const {
	int x, y0, y1;
	x = pos.x / tileSize;
	y0 = pos.y / tileSize;
	y1 = (pos.y + size.y - 1) / tileSize;

	bool collisionMade = false;
	for (int y = y0; y <= y1; y++) {
		if (map[y*mapSize.x + x] != 0) {
			if (!collisionMade) {
				(*position).x = (x + 1)*tileSize;
				collisionMade = true;
			}
			if (map[y*mapSize.x + x] == SPIKE && changeState < DEAD) {
				changeState = DEAD;
			}
		}
	}
	if (collisionMade) return true;

	return false;
}

bool TileMap::collisionMoveRight(const glm::ivec2 &pos, const glm::ivec2 &size, glm::ivec2 *position, int &changeState) const {
	int x, y0, y1;
	x = (pos.x + size.x - 1) / tileSize;
	y0 = pos.y / tileSize;
	y1 = (pos.y + size.y - 1) / tileSize;

	bool collisionMade = false;
	for (int y = y0; y <= y1; y++) {
		if (map[y*mapSize.x + x] != 0) {
			if (!collisionMade) {
				(*position).x = x * tileSize - size.x;
				collisionMade = true;
			}
			if (map[y*mapSize.x + x] == SPIKE && changeState < DEAD) {
				changeState = DEAD;
			}
		}
	}
	if (collisionMade) return true;

	return false;
}

bool TileMap::collisionMoveDown(const glm::ivec2 &pos, const glm::ivec2 &size, glm::ivec2 *position, int &changeState) const {
	int x0, x1, y;
	x0 = pos.x / tileSize;
	x1 = (pos.x + size.x - 1) / tileSize;
	y = (pos.y + size.y - 1) / tileSize;

	bool collisionMade = false;
	for (int x = x0; x <= x1; x++) {
		if (map[y*mapSize.x + x] != 0) {
			if (!collisionMade) {
				(*position).y = tileSize * y - size.y;
				collisionMade = true;
			}
			if (map[y*mapSize.x + x] == SPIKE && changeState < DEAD) {
				changeState = DEAD;
			}
		}
	}
	if (collisionMade) return true;

	return false;
}

bool TileMap::collisionMoveUp(const glm::ivec2 &pos, const glm::ivec2 &size, glm::ivec2 *position, int &changeState) const {
	int x0, x1, y;
	x0 = pos.x / tileSize;
	x1 = (pos.x + size.x - 1) / tileSize;
	y = pos.y / tileSize;

	bool collisionMade = false;
	for (int x = x0; x <= x1; x++) {
		if (map[y*mapSize.x + x] != 0) {
			if (!collisionMade) {
				(*position).y = tileSize * (y + 1);
				collisionMade = true;
			}
			if (map[y*mapSize.x + x] == SPIKE && changeState < DEAD) {
				changeState = DEAD;
			}
		}
	}
	if (collisionMade) return true;

	return false;
}
