#ifndef _BACKGROUND_INCLUDE
#define _BACKGROUND_INCLUDE


#include <glm/glm.hpp>
#include "Texture.h"
#include "ShaderProgram.h"


// Class Background is capable of loading a Background from a file. 
// With this information it builds a single VBO that contains all tiles.
// As a result the render method draws the whole background independently
// of what is visible.


class Background {

public:
	// Tile maps can only be created inside an OpenGL context
	static Background *createBackground(const string &backgroundImage, const glm::vec2 &minCoords, const glm::vec2 &backgroundSize, ShaderProgram &program);

	Background(const string &backgroundImage, const glm::vec2 &minCoords, const glm::vec2 &backgroundSize, ShaderProgram &program);
	~Background();

	void render() const;
	void free();

private:
	void prepareArrays(const glm::vec2 &minCoords, const glm::vec2 &backgroundSize, ShaderProgram &program);

private:
	GLuint vao, vbo;
	GLint posLocation, texCoordLocation;
	Texture background;
};


#endif // _BACKGROUND_INCLUDE