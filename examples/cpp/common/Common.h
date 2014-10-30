/************************************************************************************

 Authors     :   Bradley Austin Davis <bdavis@saintandreas.org>
 Copyright   :   Copyright Brad Davis. All Rights reserved.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 ************************************************************************************/

#pragma once


#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#if !defined(_DEBUG)
#undef RIFT_DEBUG
#endif
#endif

#define __STDC_FORMAT_MACROS 1

#include <algorithm>
#include <array>
#include <cassert>
#include <cinttypes>
#include <cmath>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/norm.hpp>

using glm::ivec3;
using glm::ivec2;
using glm::uvec2;
using glm::mat3;
using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::quat;

float aspect(const glm::vec2 & v) {
  return (float)v.x / (float)v.y;
}

#include <GL/glew.h>

#pragma warning( disable : 4068 4244 4267 4065)
#include <oglplus/config/gl.hpp>
#include <oglplus/all.hpp>
#include <oglplus/interop/glm.hpp>
#include <oglplus/bound/texture.hpp>
#include <oglplus/bound/framebuffer.hpp>
#include <oglplus/bound/renderbuffer.hpp>
#include <oglplus/bound/buffer.hpp>
#pragma warning( default : 4068 4244 4267 4065)

#include <Resources.h>

#include "Platform.h"
#include "Files.h"
#include "IO.h"

#include "rendering/Shaders.h"
#include "rendering/Lights.h"
#include "rendering/MatrixStack.h"
#include "rendering/State.h"
#include "rendering/Mesh.h"
#include "rendering/Geometry.h"

#include "GlUtils.h"
#include "GlfwApp.h"


#include <OVR_CAPI.h>
#include <Kernel/OVR_Types.h>
#include <OVR_CAPI_GL.h>


#include "OvrUtils.h"
#include "RiftApp.h"

//#include <GlDebug.h>
//#include <GlStacks.h>
//#include <GlQuery.h>
//#include <GlShaders.h>
//#include <GlGeometry.h>
//#include <GlLighting.h>


#ifndef PI
#define PI 3.14159265f
#endif

#ifndef HALF_PI
#define HALF_PI (PI / 2.0f)
#endif

#ifndef TWO_PI
#define TWO_PI (PI * 2.0f)
#endif

#ifndef RADIANS_TO_DEGREES
#define RADIANS_TO_DEGREES (180.0f / PI)
#endif

#ifndef DEGREES_TO_RADIANS
#define DEGREES_TO_RADIANS (PI / 180.0f)
#endif

// Combine some macros together to create a single macro
// to run an example app
#define RUN_APP(AppClass) \
    MAIN_DECL { \
        try { \
            return AppClass().run(); \
        } catch (std::exception & error) { \
            SAY_ERR(error.what()); \
        } catch (const std::string & error) { \
            SAY_ERR(error.c_str()); \
        } \
        return -1; \
    }

//#include "Config.h"
//
//#include "Font.h"
//#include "Files.h"
//
//#include "GlMesh.h"
//#include "GlUtils.h"
//#include "GlfwApp.h"
//
//#include "Interaction.h"
//
//#include "OvrUtils.h"
//#include "OpenCV.h"
