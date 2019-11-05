#pragma once

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

namespace BlinnPhongLighting
{
	void BlinnPhongLighting();
}

namespace GammaCorrection
{
	void GammaCorrection();
}

namespace ShadowMapping
{
	void ShadowMapping();
}