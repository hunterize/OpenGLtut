#pragma once

#include <SDL.h>
#include <glew.h>

#include <iostream>

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
void SDLTransform();
void SDLCoordinate2D();
void SDLCoordinate3D();