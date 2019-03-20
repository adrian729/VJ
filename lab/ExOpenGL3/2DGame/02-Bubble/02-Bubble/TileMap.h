#ifndef _TILE_MAP_INCLUDE
#define _TILE_MAP_INCLUDE


#include <glm/glm.hpp>
#include "Texture.h"
#include "ShaderProgram.h"


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

	void renderBackground() const;
	void render() const;
	void renderFront() const;
	void renderLights() const;
	void free();

	int getTileSize() const { return tileSize; }
	glm::ivec2 getMapSize() const { return mapSize; }
	glm::ivec2 getSceneSize() const { return tileSize * mapSize; }
	glm::vec2 getPlayerInitPosition() const { return initPlayerPos; }

	bool collisionMoveLeft(const glm::ivec2 &pos, const glm::ivec2 &size, glm::ivec2 *position, int &changeState) const;
	bool collisionMoveRight(const glm::ivec2 &pos, const glm::ivec2 &size, glm::ivec2 *position, int &changeState) const;
	bool collisionMoveDown(const glm::ivec2 &pos, const glm::ivec2 &size, glm::ivec2 *position, int &changeState) const;
	bool collisionMoveUp(const glm::ivec2 &pos, const glm::ivec2 &size, glm::ivec2 *position, int &changeState) const;

private:
	bool loadLevel(const string &levelName);
	bool loadBackground(const string &backgroundFile);
	bool loadTiles(const string &tilesFile);
	bool loadFrontTiles(const string &frontTilesFile);
	bool loadLights(const string &lightsFile);
	void prepareArrays(ShaderProgram &program);
	void prepareArraysBackground(ShaderProgram &program);
	void prepareArraysLights(ShaderProgram &program);
	void prepareArraysTiles(int *tileMap, GLuint &vao, GLuint &vbo, const glm::ivec2 &blockSize, const glm::ivec2 &initPos,
		const Texture &sheet, const glm::ivec2 &sheetSize, const glm::vec2 &texSize, ShaderProgram &program);

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

	// General
	GLint posLocation, texCoordLocation;
	int tileSize;
	glm::ivec2 mapSize, mapPixels; // size in tiles; size in pixels.
	glm::vec2 minCoords, initPlayerPos; // map init pos; player init pos (down right tile point)
	int *map, *frontMap;
};


#endif // _TILE_MAP_INCLUDE


