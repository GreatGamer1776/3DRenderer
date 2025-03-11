#include "render.hpp"
#include <stdexcept>

void Render::startRender() {
	//Start the subsytem and log any errors in case it fails
	if (!SDL_InitSubSystem(SDL_INIT_VIDEO))
		SDL_LogCritical(SDL_LogCategory::SDL_LOG_CATEGORY_ERROR, SDL_GetError());
}

void Render::endRender() {
	//Quit the susbsytem
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	SDL_Quit();
}

//Create a poll event function for abstraction
std::optional<SDL_Event> Render::pollEvent()
{
	SDL_Event event;
	if (SDL_PollEvent(&event))
		return event; // return the event if there is one
	else
		return std::nullopt; //kinda like nullptr
}

