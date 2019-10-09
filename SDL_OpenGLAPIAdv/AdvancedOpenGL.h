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
#include <gtc/type_ptr.hpp>

#include <map>

#include <vector>



namespace DepthTesting
{
	void DepthTesting();
}


namespace StencilTesting
{
	void StencilTesting();
}

namespace StencilReflection
{
	void StencilReflection();
}

namespace Blending
{
	void Blending();
}

namespace BlendFunc
{
	void BlendFunc();
}

namespace FaceCulling
{
	void FaceCulling();
}

namespace FrameBuffer
{
	void FrameBuffer();
}


namespace FrameBufferMirror
{
	void FrameBufferMirror();
}

namespace CubeMaps
{
	void CubeMaps();
}

namespace CubeMapsReflection
{
	void CubeMapsReflection();
}

namespace AdvancedAPI
{
	void AdvancedAPI();
}

namespace GeometryShader
{
	void GeometryShader();
}

namespace Instancing
{
	void Instancing();
}