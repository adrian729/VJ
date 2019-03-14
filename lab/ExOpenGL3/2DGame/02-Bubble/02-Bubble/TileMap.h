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
	static TileMap *createTileMap(const string &levelFile, const string &frontTilesFile, const glm::vec2 &minCoords, ShaderProgram &program);

	TileMap(const string &levelFile, const string &frontTilesFile, const glm::vec2 &minCoords, ShaderProgram &program);
	~TileMap();

	void render() const;
	void renderFront() const;
	void free();

	int getTileSize() const { return tileSize; }
	glm::ivec2 getMapSize() const { return mapSize; }

	bool collisionMoveLeft(const glm::ivec2 &pos, const glm::ivec2 &size, int *posX) const;
	bool collisionMoveRight(const glm::ivec2 &pos, const glm::ivec2 &size, int *posX) const;
	bool collisionMoveDown(const glm::ivec2 &pos, const glm::ivec2 &size, int *posY) const;
	bool collisionMoveUp(const glm::ivec2 &pos, const glm::ivec2 &size, int *posY) const;

private:
	bool loadLevel(const string &levelFile, const string &frontTilesFile);
	void prepareArrays(const glm::vec2 &minCoords, ShaderProgram &program);

private:
	GLuint vao, vbo;
	GLuint vaoFront, vboFront;
	GLint posLocation, texCoordLocation;
	glm::ivec2 position, mapSize, tilesheetSize, frontTilesheetSize;
	int tileSize, blockSize;
	int frontTileSize, frontBlockSize;
	Texture tilesheet, frontTileSheet;
	glm::vec2 tileTexSize, frontTileTexSize;
	int *map, *frontMap;
};


#endif // _TILE_MAP_INCLUDE


