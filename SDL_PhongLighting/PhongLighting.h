#pragma once

#include <SDL.h>
#include <glew.h>

#include <iostream>

#include "CShader.h"
#include "CTimer.h"
#include "CInputManager.h"
#include "CCamera3D.h"
#include "CSTexture.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

namespace BasicLighting
{
	void GLSLBasicLighting();
}

namespace Materials
{
	void GLSLMaterials();
}

namespace LightingMaps
{
	void GLSLLightingMaps();
}

namespace LightingCasters
{
	void GLSLLightingCasters();
}

namespace MultipleLighting
{
	void GLSLMultipleLighting();
}
