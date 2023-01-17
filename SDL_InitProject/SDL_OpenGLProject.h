#pragma once

#define WINDOWS_IGNORE_PACKING_MISMATCH
#include <SDL_syswm.h>
#include <ShellScalingApi.h>
#undef LoadImage

#include <SDL.h>
#include <glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <iostream>

#include "CShader.h"
#include "CInputManager.h"
#include "CCamera3D.h"
#include "CSTexture.h"

namespace Demo	
{
	void Demo();
}