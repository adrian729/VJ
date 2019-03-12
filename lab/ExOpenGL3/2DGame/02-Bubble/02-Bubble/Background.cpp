#include <iostream>
#include <vector>
#include "Background.h"


using namespace std;


Background *Background::createBackground(const string &backgroundImage, const glm::vec2 &minCoords, const glm::vec2 &backgroundSize, ShaderProgram &program)
{
	Background *background = new Background(backgroundImage, minCoords, backgroundSize, program);

	return background;
}


Background::Background(const string &backgroundImage, const glm::vec2 &minCoords, const glm::vec2 &backgroundSize, ShaderProgram &program)
{
	background.loadFromFile(backgroundImage, TEXTURE_PIXEL_FORMAT_RGBA);
	background.setWrapS(GL_CLAMP_TO_EDGE);
	background.setWrapT(GL_CLAMP_TO_EDGE);
	background.setMinFilter(GL_NEAREST);
	background.setMagFilter(GL_NEAREST);
	prepareArrays(minCoords, backgroundSize, program);
}


Background::~Background() {
}


void Background::render() const {
	glEnable(GL_TEXTURE_2D);
	background.use();
	glBindVertexArray(vao);
	glEnableVertexAttribArray(posLocation);
	glEnableVertexAttribArray(texCoordLocation);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisable(GL_TEXTURE_2D);
}


void Background::free() {
	glDeleteBuffers(1, &vbo);
}

void Background::prepareArrays(const glm::vec2 &minCoords, const glm::vec2 &backgroundSize, ShaderProgram &program) {
	// Background
	glm::vec2 pos, texCoordTile[2];
	vector<float> vertices;

	pos = glm::vec2(minCoords.x, minCoords.y);
	texCoordTile[0] = glm::vec2(0.f);
	texCoordTile[1] = glm::vec2(1.f);
	// First triangle
	vertices.push_back(pos.x); vertices.push_back(pos.y);
	vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
	vertices.push_back(pos.x + backgroundSize.x); vertices.push_back(pos.y);
	vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[0].y);
	vertices.push_back(pos.x + backgroundSize.x); vertices.push_back(pos.y + backgroundSize.y);
	vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
	// Second triangle
	vertices.push_back(pos.x); vertices.push_back(pos.y);
	vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
	vertices.push_back(pos.x + backgroundSize.x); vertices.push_back(pos.y + backgroundSize.y);
	vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
	vertices.push_back(pos.x); vertices.push_back(pos.y + backgroundSize.y);
	vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[1].y);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	posLocation = program.bindVertexAttribute("position", 2, 4 * sizeof(float), 0);
	texCoordLocation = program.bindVertexAttribute("texCoord", 2, 4 * sizeof(float), (void *)(2 * sizeof(float)));
}
