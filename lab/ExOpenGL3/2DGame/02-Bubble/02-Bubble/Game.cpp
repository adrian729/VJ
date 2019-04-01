#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Game.h"


void Game::init() {
	bPlay = true;
	gravity = true;
	ground = false;
	bass = false;
	for (int i = 0; i < sizeof(releaseKey); i++) {
		releaseKey[i] = true;
	}
	for (int i = 0; i < sizeof(releaseSpecialKeys); i++) {
		releaseSpecialKeys[i] = true;
	}
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	scene.init();
}

bool Game::update(int deltaTime) {
	scene.update(deltaTime);
	
	return bPlay;
}

void Game::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene.render();
}

void Game::keyPressed(int key) {
	if (key == 27) // Escape code
		bPlay = false;
	if (key == 32 && releaseKey[key] && ground && !bass)
		gravity = !gravity;
	keys[key] = true;
	releaseKey[key] = false;
}

void Game::keyReleased(int key) {
	keys[key] = false;
	releaseKey[key] = true;
}

void Game::specialKeyPressed(int key) {
	specialKeys[key] = true;
	releaseSpecialKeys[key] = false;
}

void Game::specialKeyReleased(int key) {
	specialKeys[key] = false;
	releaseSpecialKeys[key] = true;
}

void Game::mouseMove(int x, int y) {
}

void Game::mousePress(int button) {
}

void Game::mouseRelease(int button) {
}

bool Game::getKey(int key) const {
	return keys[key];
}

bool Game::getSpecialKey(int key) const {
	return specialKeys[key];
}





