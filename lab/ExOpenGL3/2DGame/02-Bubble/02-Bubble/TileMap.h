#ifndef _TILE_MAP_INCLUDE
#define _TILE_MAP_INCLUDE


#include <glm/glm.hpp>
#include <vector>
#include "Texture.h"
#include "ShaderProgram.h"
#include "Globals.h"
#include "Enemy.h"


// Class Tilemap is capable of loading a tile map from a text file in a very
// simple format (see level01.txt for an example). With this information
// it builds a single VBO that contains all tiles. As a result the render
// method draws the whole map independently of what is visible.


class TileMap {

public:
	// Tile maps can only be created inside an OpenGL context
	static TileMap *createTileMap(const string &levelName, const glm::vec2 &minCoords, ShaderProgram &program);

	TileMap(const string &levelName, const glm::vec2 &minCoords, ShaderProgram &program);
	~TileMap();

	void update(int deltaTime, ShaderProgram &program,
		const glm::ivec2 &playerPos, const glm::ivec2 &playerSize,
		const glm::ivec3 &playerCollision, const bool &playerLeft,
		int *playerState, const bool &g);
	void renderBackground() const;
	void render() const;
	void renderFront() const;
	void renderLights() const;
	void renderEnemies() const;
	void free();

	int getTileSize() const { return tileSize; }
	glm::ivec2 getMapSize() const { return mapSize; }
	glm::ivec2 getSceneSize() const { return tileSize * mapSize; }
	glm::vec2 getPlayerInitPosition() const { return initPlayerPos; }

	bool collisionMoveLeft(const glm::ivec2 &pos, const glm::ivec2 &size, glm::ivec2 *position, int &changeState);
	bool collisionMoveRight(const glm::ivec2 &pos, const glm::ivec2 &size, glm::ivec2 *position, int &changeState);
	bool collisionMoveDown(const glm::ivec2 &pos, const glm::ivec2 &size, glm::ivec2 *position, int &changeState);
	bool collisionMoveUp(const glm::ivec2 &pos, const glm::ivec2 &size, glm::ivec2 *position, int &changeState);

	int animationCount;
	int changeMapInfoId;
	vector<glm::ivec3> changeMapInfo; // mapID, tile X, tile Y
	glm::ivec2 activatedCheckpoint;

private:
	bool loadLevel(const string &levelName, ShaderProgram &program);
	bool loadBackground(const string &backgroundFile);
	bool loadTiles(const string &tilesFile);
	bool loadFrontTiles(const string &frontTilesFile);
	bool loadLights(const string &lightsFile);
	bool loadEnemies(const string &enemiesFile, ShaderProgram &program);
	void prepareArrays(ShaderProgram &program);
	void prepareArraysBackground(ShaderProgram &program);
	void prepareArraysLights(ShaderProgram &program);
	void prepareArraysTiles(int *tileMap, GLuint &vao, GLuint &vbo, const glm::ivec2 &blockSize, const glm::ivec2 &initPos,
		const Texture &sheet, const glm::ivec2 &sheetSize, const glm::vec2 &texSize, ShaderProgram &program, const bool &front);

private:
	// Background
	GLuint vaoBack, vboBack;
	Texture backgroundImage;

	// TileMap
	GLuint vaoTileMap, vboTileMap;
	glm::ivec2 tileBlockSize, initTile, tilesheetSize;
	Texture tilesheet;
	glm::vec2 tileTexSize;

	// FrontTiles
	GLuint vaoFront, vboFront;
	glm::ivec2 frontBlockSize, initFrontTile, frontTilesheetSize;
	Texture frontTileSheet;
	glm::vec2 frontTexSize;

	// Special
	GLuint vaoSpecial, vboSpecial;
	int specialTilesheetSize;
	Texture specialTileSheet;

	// Lights
	GLuint vaoLights, vboLights;
	Texture lightsImage;

	// Enemies
	vector<Enemy*> enemies;

	// Movile Platforms (Y is fixed, so info is for X axis in pixels)
	vector<int> typeMP, startMP, endMP, positionMP, velocityMP, sMP, yMP;
	// sMP < 0 == left

	// General
	GLint posLocation, texCoordLocation;
	int tileSize;
	glm::ivec2 mapSize, mapPixels; // size in tiles; size in pixels.
	glm::vec2 minCoords, initPlayerPos; // map init pos; player init pos (down right tile point)
	int *map, *frontMap;
	int *mapType; // type of the maptile
};


#endif // _TILE_MAP_INCLUDE


