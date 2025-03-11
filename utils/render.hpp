#pragma once

#include <glad/gl.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <memory>
#include <string>
#include <optional>


namespace Render{

	//Window widtha nd height for resizing and check if runnings
	inline int windowWidth{ 1280 };
	inline int windowHeight{ 720 };
	inline bool isRunning = false;

	void startRender();

	void endRender();

	std::optional<SDL_Event> pollEvent();	

}