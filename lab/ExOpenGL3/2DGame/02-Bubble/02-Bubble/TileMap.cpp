#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "TileMap.h"


using namespace std;


TileMap *TileMap::createTileMap(const string &levelFile, const string &frontTilesFile, const glm::vec2 &minCoords, ShaderProgram &program) {
	TileMap *map = new TileMap(levelFile, frontTilesFile, minCoords, program);

	return map;
}


TileMap::TileMap(const string &levelFile, const string &frontTilesFile, const glm::vec2 &minCoords, ShaderProgram &program) {
	loadLevel(levelFile, frontTilesFile);
	prepareArrays(minCoords, program);
}

TileMap::~TileMap() {
	if (map != NULL)
		delete map;
	if (frontMap != NULL)
		delete frontMap;
}

void TileMap::render() const {
	glEnable(GL_TEXTURE_2D);
	tilesheet.use();
	glBindVertexArray(vao);
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

void TileMap::free() {
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &vboFront);

}

bool TileMap::loadLevel(const string &levelFile, const string &frontTilesFile)
{
	ifstream fin;
	string line, tilesheetFile, frontTilesheetFile;
	stringstream sstream;
	char tile;

	// Tiles
	fin.open(levelFile.c_str());
	if (!fin.is_open())
		return false;
	getline(fin, line);
	if (line.compare(0, 7, "TILEMAP") != 0)
		return false;
	getline(fin, line);
	sstream.str(line);
	sstream >> mapSize.x >> mapSize.y;
	getline(fin, line);
	sstream.str(line);
	sstream >> tileSize >> blockSize;
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
			fin.get(tile);
			if (tile == ' ')
				map[j*mapSize.x + i] = 0;
			else
				map[j*mapSize.x + i] = tile - int('0');
		}
		fin.get(tile);
#ifndef _WIN32
		fin.get(tile);
#endif
	}
	fin.close();


	// Front Tiles
	fin.open(frontTilesFile.c_str());
	if (!fin.is_open())
		return false;
	getline(fin, line);
	if (line.compare(0, 12, "FRONTTILEMAP") != 0)
		return false;
	getline(fin, line);
	sstream.str(line);
	sstream >> frontTileSize >> frontBlockSize;
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
	frontTileTexSize = glm::vec2(1.f / frontTilesheetSize.x, 1.f / frontTilesheetSize.y);

	cout << frontTileSize << endl;

	frontMap = new int[mapSize.x * mapSize.y];
	for (int j = 0; j < mapSize.y; j++)
	{
		for (int i = 0; i < mapSize.x; i++)
		{
			fin.get(tile);
			if (tile == ' ')
				frontMap[j*mapSize.x + i] = 0;
			else
				frontMap[j*mapSize.x + i] = tile - int('0');
		}
		fin.get(tile);
#ifndef _WIN32
		fin.get(tile);
#endif
	}
	fin.close();

	return true;
}

void TileMap::prepareArrays(const glm::vec2 &minCoords, ShaderProgram &program)
{
	// Tiles
	int tile, nTiles = 0;
	glm::vec2 posTile, texCoordTile[2], halfTexel;
	vector<float> vertices;

	halfTexel = glm::vec2(0.5f / tilesheet.width(), 0.5f / tilesheet.height());

	// Hem d'anar en compte en quin ordre afegim els tiles ja que si solapen l'ultim pintat queda per sobre
	for (int j = mapSize.y - 1; j >= 0; j--)
	{
		for (int i = 0; i < mapSize.x; i++)
		{
			tile = map[j * mapSize.x + i];
			if (tile != 0)
			{
				// Non-empty tile
				nTiles++;
				//j - 1 perque les nostres tiles comencen a mig block (en les y). -4 i +4 per donar "efecte 2.5D" una mica
				posTile = glm::vec2(minCoords.x + i * tileSize - 4, minCoords.y + (j - 1) * tileSize + 4);
				texCoordTile[0] = glm::vec2(float((tile - 1) % 2) / tilesheetSize.x, float((tile - 1) / 2) / tilesheetSize.y);
				texCoordTile[1] = texCoordTile[0] + tileTexSize;
				//texCoordTile[0] += halfTexel;
				texCoordTile[1] -= halfTexel;
				// First triangle
				vertices.push_back(posTile.x); vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + blockSize); vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + blockSize); vertices.push_back(posTile.y + blockSize);
				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
				// Second triangle
				vertices.push_back(posTile.x); vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + blockSize); vertices.push_back(posTile.y + blockSize);
				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
				vertices.push_back(posTile.x); vertices.push_back(posTile.y + blockSize);
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

	// TODO: arreglar per a que coloqui be els tiles de front si els fem de 32x32
	// Front Tiles
	nTiles = 0;
	vertices = vector<float>();

	halfTexel = glm::vec2(0.5f / frontTileSheet.width(), 0.5f / tilesheet.height());

	// Hem d'anar en compte en quin ordre afegim els tiles ja que si solapen l'ultim pintat queda per sobre
	for (int j = mapSize.y - 1; j >= 0; j--)
	{
		for (int i = 0; i < mapSize.x; i++)
		{
			tile = frontMap[j * mapSize.x + i];
			if (tile != 0)
			{
				// Non-empty tile
				nTiles++;
				//j - 1 perque les nostres tiles comencen a mig block (en les y). -4 i +4 per donar "efecte 2.5D" una mica
				posTile = glm::vec2(minCoords.x + i * frontTileSize - 4, minCoords.y + (j - 1) * frontTileSize + 4);
				texCoordTile[0] = glm::vec2(float((tile - 1) % 2) / frontTilesheetSize.x, float((tile - 1) / 2) / frontTilesheetSize.y);
				texCoordTile[1] = texCoordTile[0] + frontTileTexSize;
				//texCoordTile[0] += halfTexel;
				texCoordTile[1] -= halfTexel;
				// First triangle
				vertices.push_back(posTile.x); vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + frontBlockSize); vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + frontBlockSize); vertices.push_back(posTile.y + frontBlockSize);
				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
				// Second triangle
				vertices.push_back(posTile.x); vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + frontBlockSize); vertices.push_back(posTile.y + frontBlockSize);
				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
				vertices.push_back(posTile.x); vertices.push_back(posTile.y + frontBlockSize);
				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[1].y);
			}
		}
	}

	glGenVertexArrays(1, &vaoFront);
	glBindVertexArray(vaoFront);
	glGenBuffers(1, &vboFront);
	glBindBuffer(GL_ARRAY_BUFFER, vboFront);
	glBufferData(GL_ARRAY_BUFFER, 24 * nTiles * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	posLocation = program.bindVertexAttribute("position", 2, 4 * sizeof(float), 0);
	texCoordLocation = program.bindVertexAttribute("texCoord", 2, 4 * sizeof(float), (void *)(2 * sizeof(float)));
}

// Collision tests for axis aligned bounding boxes.
// Position corrected if a collision is detected.

bool TileMap::collisionMoveLeft(const glm::ivec2 &pos, const glm::ivec2 &size, int *posX) const
{
	int x, y0, y1;

	x = pos.x / tileSize;
	y0 = pos.y / tileSize;
	y1 = (pos.y + size.y - 1) / tileSize;
	for (int y = y0; y <= y1; y++)
	{
		if (map[y*mapSize.x + x] != 0)
		{
			*posX = (x + 1)*tileSize;
			return true;
		}
	}

	return false;
}

bool TileMap::collisionMoveRight(const glm::ivec2 &pos, const glm::ivec2 &size, int *posX) const
{
	int x, y0, y1;

	x = (pos.x + size.x - 1) / tileSize;
	y0 = pos.y / tileSize;
	y1 = (pos.y + size.y - 1) / tileSize;
	for (int y = y0; y <= y1; y++)
	{
		if (map[y*mapSize.x + x] != 0)
		{
			*posX = x * tileSize - size.x;
			return true;
		}
	}

	return false;
}

bool TileMap::collisionMoveDown(const glm::ivec2 &pos, const glm::ivec2 &size, int *posY) const
{
	int x0, x1, y;

	x0 = pos.x / tileSize;
	x1 = (pos.x + size.x - 1) / tileSize;
	y = (pos.y + size.y - 1) / tileSize;
	for (int x = x0; x <= x1; x++)
	{
		if (map[y*mapSize.x + x] != 0)
		{
			*posY = tileSize * y - size.y;
			return true;
		}
	}

	return false;
}

bool TileMap::collisionMoveUp(const glm::ivec2 &pos, const glm::ivec2 &size, int *posY) const
{
	int x0, x1, y;

	x0 = pos.x / tileSize;
	x1 = (pos.x + size.x - 1) / tileSize;
	y = pos.y / tileSize;
	for (int x = x0; x <= x1; x++)
	{
		if (map[y*mapSize.x + x] != 0)
		{
			*posY = tileSize * (y + 1);
			return true;
		}
	}

	return false;
}
