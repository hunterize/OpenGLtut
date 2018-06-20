#pragma once

#include <SDL.h>
#include <glew.h>

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION

enum class GAMESTATE
{
	RUNNING,
	EXIT
};

void ProcessInput();
void Run();

void SDLWindow();
void SDLVertex();
void SDLVAO();
void SDLVertexImmediate();
void SDLEBO();
void SDLTexture();