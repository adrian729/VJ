#include <GL/glut.h>
#include "Game.h"
#include "Macros.h"


void Game::init()
{
	bPlay = true;
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
}

bool Game::update(int deltaTime)
{
	return bPlay;
}

void Game::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Game::keyPressed(int key)
{
	if (key == 27) // Escape code
		bPlay = false;
	if (key == GLFW_KEY_0)
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	if (key == GLFW_KEY_1)
		glClearColor(0.7f, 0.3f, 0.3f, 1.0f);
	if (key == GLFW_KEY_2)
		glClearColor(0.7f, 0.7f, 0.3f, 1.0f);
	if (key == GLFW_KEY_3)
		glClearColor(0.3f, 0.7f, 0.3f, 1.0f);
	if (key == GLFW_KEY_4)
		glClearColor(0.3f, 0.7f, 0.7f, 1.0f);
	if (key == GLFW_KEY_5)
		glClearColor(0.3f, 0.3f, 0.7f, 1.0f);
	if (key == GLFW_KEY_6)
		glClearColor(0.7f, 0.3f, 0.7f, 1.0f);
	if (key == GLFW_KEY_7)
		glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	if (key == GLFW_KEY_8)
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	if (key == GLFW_KEY_9)
		glClearColor(0.2f, 0.9f, 0.5f, 1.0f);
}

void Game::keyReleased(int key)
{
}

void Game::mouseMove(int x, int y)
{
}

void Game::mousePress(int button)
{
}

void Game::mouseRelease(int button)
{
}



