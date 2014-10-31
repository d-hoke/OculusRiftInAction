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

/************************************************************************************

Based on examples from the book Oculus Rift in Action

http://oculusriftinaction.com

************************************************************************************/

#include <iostream>
#include <memory>
#include <exception>

///////////////////////////////////////////////////////////////////////////////
//
// GLM is a C++ math library meant to mirror the syntax of GLSL 
//

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

// Import the most commonly used types into the default namespace
using glm::ivec3;
using glm::ivec2;
using glm::uvec2;
using glm::mat3;
using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::quat;

///////////////////////////////////////////////////////////////////////////////
//
// GLEW gives cross platform access to OpenGL 3.x+ functionality.  
//

#include <GL/glew.h>

//////////////////////////////////////////////////////////////////////
//
// OGLplus is a set of wrapper classes for giving OpenGL a more object
// oriented interface
//

#pragma warning( disable : 4068 4244 4267 4065)
#include <oglplus/config/gl.hpp>
#include <oglplus/all.hpp>
#include <oglplus/interop/glm.hpp>
#include <oglplus/bound/texture.hpp>
#include <oglplus/bound/framebuffer.hpp>
#include <oglplus/bound/renderbuffer.hpp>
#include <oglplus/bound/buffer.hpp>
#pragma warning( default : 4068 4244 4267 4065)

// A wrapper for constructing and using a 
struct FboWrapper {
  oglplus::Framebuffer   fbo;
  oglplus::Texture       color;
  oglplus::Renderbuffer  depth;

  void init(const glm::uvec2 & size) {
    using namespace oglplus;
    Context::Bound(Texture::Target::_2D, color)
      .MinFilter(TextureMinFilter::Linear)
      .MagFilter(TextureMagFilter::Linear)
      .WrapS(TextureWrap::ClampToEdge)
      .WrapT(TextureWrap::ClampToEdge)
      .Image2D(
      0, PixelDataInternalFormat::RGBA8,
      size.x, size.y,
      0, PixelDataFormat::RGB, PixelDataType::UnsignedByte, nullptr
      );

    Context::Bound(Renderbuffer::Target::Renderbuffer, depth)
      .Storage(
      PixelDataInternalFormat::DepthComponent,
      size.x, size.y
      );

    Context::Bound(Framebuffer::Target::Draw, fbo)
      .AttachTexture(FramebufferAttachment::Color, color, 0)
      .AttachRenderbuffer(FramebufferAttachment::Depth, depth)
      .Complete();
  }
};

typedef std::shared_ptr<FboWrapper> fbo_wrapper_ptr;

static const GLuint CUBE_VERTEX_COUNT = 6 * 2 * 3;

static const GLfloat CUBE_VERTEX_DATA[8][3] = {
    { -0.5f, -0.5f, +0.5f },
    { -0.5f, -0.5f, -0.5f },
    { -0.5f, +0.5f, -0.5f },
    { -0.5f, +0.5f, +0.5f },
    { +0.5f, -0.5f, +0.5f },
    { +0.5f, -0.5f, -0.5f },
    { +0.5f, +0.5f, -0.5f },
    { +0.5f, +0.5f, +0.5f }
};

static const GLfloat CUBE_VERTICES[CUBE_VERTEX_COUNT * 3] = {
  CUBE_VERTEX_DATA[0][0], CUBE_VERTEX_DATA[0][1], CUBE_VERTEX_DATA[0][2],
  CUBE_VERTEX_DATA[2][0], CUBE_VERTEX_DATA[2][1], CUBE_VERTEX_DATA[2][2],
  CUBE_VERTEX_DATA[1][0], CUBE_VERTEX_DATA[1][1], CUBE_VERTEX_DATA[1][2],
  CUBE_VERTEX_DATA[0][0], CUBE_VERTEX_DATA[0][1], CUBE_VERTEX_DATA[0][2],
  CUBE_VERTEX_DATA[3][0], CUBE_VERTEX_DATA[3][1], CUBE_VERTEX_DATA[3][2],
  CUBE_VERTEX_DATA[2][0], CUBE_VERTEX_DATA[2][1], CUBE_VERTEX_DATA[2][2],
  CUBE_VERTEX_DATA[0][0], CUBE_VERTEX_DATA[0][1], CUBE_VERTEX_DATA[0][2],
  CUBE_VERTEX_DATA[1][0], CUBE_VERTEX_DATA[1][1], CUBE_VERTEX_DATA[1][2],
  CUBE_VERTEX_DATA[4][0], CUBE_VERTEX_DATA[4][1], CUBE_VERTEX_DATA[4][2],
  CUBE_VERTEX_DATA[1][0], CUBE_VERTEX_DATA[1][1], CUBE_VERTEX_DATA[1][2],
  CUBE_VERTEX_DATA[5][0], CUBE_VERTEX_DATA[5][1], CUBE_VERTEX_DATA[5][2],
  CUBE_VERTEX_DATA[4][0], CUBE_VERTEX_DATA[4][1], CUBE_VERTEX_DATA[4][2],
  CUBE_VERTEX_DATA[1][0], CUBE_VERTEX_DATA[1][1], CUBE_VERTEX_DATA[1][2],
  CUBE_VERTEX_DATA[2][0], CUBE_VERTEX_DATA[2][1], CUBE_VERTEX_DATA[2][2],
  CUBE_VERTEX_DATA[5][0], CUBE_VERTEX_DATA[5][1], CUBE_VERTEX_DATA[5][2],
  CUBE_VERTEX_DATA[2][0], CUBE_VERTEX_DATA[2][1], CUBE_VERTEX_DATA[2][2],
  CUBE_VERTEX_DATA[6][0], CUBE_VERTEX_DATA[6][1], CUBE_VERTEX_DATA[6][2],
  CUBE_VERTEX_DATA[5][0], CUBE_VERTEX_DATA[5][1], CUBE_VERTEX_DATA[5][2],
  CUBE_VERTEX_DATA[4][0], CUBE_VERTEX_DATA[4][1], CUBE_VERTEX_DATA[4][2],
  CUBE_VERTEX_DATA[5][0], CUBE_VERTEX_DATA[5][1], CUBE_VERTEX_DATA[5][2],
  CUBE_VERTEX_DATA[6][0], CUBE_VERTEX_DATA[6][1], CUBE_VERTEX_DATA[6][2],
  CUBE_VERTEX_DATA[4][0], CUBE_VERTEX_DATA[4][1], CUBE_VERTEX_DATA[4][2],
  CUBE_VERTEX_DATA[6][0], CUBE_VERTEX_DATA[6][1], CUBE_VERTEX_DATA[6][2],
  CUBE_VERTEX_DATA[7][0], CUBE_VERTEX_DATA[7][1], CUBE_VERTEX_DATA[7][2],
  CUBE_VERTEX_DATA[2][0], CUBE_VERTEX_DATA[2][1], CUBE_VERTEX_DATA[2][2],
  CUBE_VERTEX_DATA[3][0], CUBE_VERTEX_DATA[3][1], CUBE_VERTEX_DATA[3][2],
  CUBE_VERTEX_DATA[7][0], CUBE_VERTEX_DATA[7][1], CUBE_VERTEX_DATA[7][2],
  CUBE_VERTEX_DATA[2][0], CUBE_VERTEX_DATA[2][1], CUBE_VERTEX_DATA[2][2],
  CUBE_VERTEX_DATA[7][0], CUBE_VERTEX_DATA[7][1], CUBE_VERTEX_DATA[7][2],
  CUBE_VERTEX_DATA[6][0], CUBE_VERTEX_DATA[6][1], CUBE_VERTEX_DATA[6][2],
  CUBE_VERTEX_DATA[0][0], CUBE_VERTEX_DATA[0][1], CUBE_VERTEX_DATA[0][2],
  CUBE_VERTEX_DATA[4][0], CUBE_VERTEX_DATA[4][1], CUBE_VERTEX_DATA[4][2],
  CUBE_VERTEX_DATA[3][0], CUBE_VERTEX_DATA[3][1], CUBE_VERTEX_DATA[3][2],
  CUBE_VERTEX_DATA[3][0], CUBE_VERTEX_DATA[3][1], CUBE_VERTEX_DATA[3][2],
  CUBE_VERTEX_DATA[4][0], CUBE_VERTEX_DATA[4][1], CUBE_VERTEX_DATA[4][2],
  CUBE_VERTEX_DATA[7][0], CUBE_VERTEX_DATA[7][1], CUBE_VERTEX_DATA[7][2]
};

static const GLfloat CUBE_NORMALS[6][3] = {
    { -1.0f, 0.0f, 0.0f },
    { 0.0f, -1.0f, 0.0f },
    { 0.0f, 0.0f, -1.0f },
    { 1.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f }
};

static const char * VERTEX_SHADER =
"#version 330\n"
"uniform mat4 ProjectionMatrix, CameraMatrix;"
"in vec4 Position;"
"in vec3 Normal;"
"out vec3 vertNormal;"
"void main(void)"
"{"
" vertNormal = Normal;"
" gl_Position = ProjectionMatrix *"
" CameraMatrix *"
" Position;"
"}";

static const char * FRAGMENT_SHADER =
"#version 330\n"
"in vec3 vertNormal;"
"out vec4 fragColor;"
"void main(void)"
"{"
" fragColor = vec4(abs(vertNormal), 1.0);"
"}";

// a class for encapsulating building and rendering an RGB cube
struct ColorCubeScene {

  // Program
  oglplus::Program prog;

  // A vertex array object for the rendered cube
  oglplus::VertexArray cube;

  // VBOs for the cube's vertices and normals
  oglplus::Buffer verts;
  oglplus::Buffer normals;

public:
  ColorCubeScene() {
    using namespace oglplus;
    // attach the shaders to the program
    prog.AttachShader(VertexShader().Source(GLSLSource(std::string(VERTEX_SHADER))).Compile());
    prog.AttachShader(FragmentShader().Source(GLSLSource(std::string(FRAGMENT_SHADER))).Compile());

    // link and use it
    prog.Link();
    prog.Use();

    // bind the VAO for the cube
    cube.Bind();

    // bind the VBO for the cube vertices
    verts.Bind(Buffer::Target::Array);
    // upload the data
    Buffer::Data(
      Buffer::Target::Array,
      CUBE_VERTEX_COUNT * 3,
      CUBE_VERTICES
      );

    // setup the vertex attribs array for the vertices
    VertexArrayAttrib vert_attr(prog, "Position");
    vert_attr.Setup<Vec3f>();
    vert_attr.Enable();
    GLfloat cube_normals[CUBE_VERTEX_COUNT * 3];
    for (GLuint f = 0; f != 6; ++f)
      for (GLuint v = 0; v != 6; ++v)
        for (GLuint ci = 0; ci != 3; ++ci)
          cube_normals[(f * 6 + v) * 3 + ci] = CUBE_NORMALS[f][ci];
    // bind the VBO for the cube normals
    normals.Bind(Buffer::Target::Array);
    // upload the data
    Buffer::Data(
      Buffer::Target::Array,
      CUBE_VERTEX_COUNT * 3,
      cube_normals
      );
    // setup the vertex attribs array for the vertices
    VertexArrayAttrib normal_attr(prog, "Normal");
    normal_attr.Setup<Vec3f>();
    normal_attr.Enable();
  }

  virtual void render(const mat4 & projection, const mat4 & modelview) {
    using namespace oglplus;
    prog.Use();
    Uniform<mat4>(prog, "CameraMatrix").Set(modelview);
    Uniform<mat4>(prog, "ProjectionMatrix").Set(projection);

    cube.Bind();
    Context::DrawArrays(PrimitiveType::Triangles, 0, 6 * 2 * 3);
  }
};

//////////////////////////////////////////////////////////////////////
//
// The OVR types header contains the OS detection macros:
//  OVR_OS_WIN32, OVR_OS_MAC, OVR_OS_LINUX (and others)
//

#include <Kernel/OVR_Types.h>

#define FAIL(X) throw std::runtime_error(X)

#if defined(OVR_OS_WIN32)
// Not really needed but it prevents a macro redifition warning later
#include <Windows.h>
#define ON_WINDOWS(runnable) runnable()
#define NOT_ON_WINDOWS(runnable)
#else
#define ON_WINDOWS(runnable)
#define NOT_ON_WINDOWS(runnable) runnable()
#endif

#if defined(OVR_OS_MAC)
#define ON_MAC(runnable) runnable()
#define NOT_ON_MAC(runnable)
#else
#define ON_MAC(runnable)
#define NOT_ON_MAC(runnable) runnable()
#endif

#if defined(OVR_OS_LINUX)
#define ON_LINUX(runnable) runnable()
#define NOT_ON_LINUX(runnable)
#else
#define ON_LINUX(runnable)
#define NOT_ON_LINUX(runnable) runnable()
#endif

#ifdef OVR_OS_WIN32
#define MAIN_DECL int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
#define MAIN_DECL int main(int argc, char ** argv)
#endif

//////////////////////////////////////////////////////////////////////
//
// GLFW provides cross platform window creation
//

#include <GLFW/glfw3.h>

#if defined(OVR_OS_WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#elif defined(OVR_OS_MAC)
#define GLFW_EXPOSE_NATIVE_COCOA
#define GLFW_EXPOSE_NATIVE_NSGL
#elif defined(OVR_OS_LINUX)
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#endif

// For some interaction with the Oculus SDK we'll need the native 
// window handle
#include <GLFW/glfw3native.h>

namespace glfw {
  inline uvec2 getSize(GLFWmonitor * monitor) {
    const GLFWvidmode * mode = glfwGetVideoMode(monitor);
    return uvec2(mode->width, mode->height);
  }

  inline ivec2 getPosition(GLFWmonitor * monitor) {
    ivec2 result;
    glfwGetMonitorPos(monitor, &result.x, &result.y);
    return result;
  }

  inline ivec2 getSecondaryScreenPosition(const uvec2 & size) {
    GLFWmonitor * primary = glfwGetPrimaryMonitor();
    int monitorCount;
    GLFWmonitor ** monitors = glfwGetMonitors(&monitorCount);
    GLFWmonitor * best = nullptr;
    uvec2 bestSize;
    for (int i = 0; i < monitorCount; ++i) {
      GLFWmonitor * cur = monitors[i];
      if (cur == primary) {
        continue;
      }
      uvec2 curSize = getSize(cur);
      if (best == nullptr || (bestSize.x < curSize.x && bestSize.y < curSize.y)) {
        best = cur;
        bestSize = curSize;
      }
    }
    if (nullptr == best) {
      best = primary;
      bestSize = getSize(best);
    }
    ivec2 pos = getPosition(best);
    if (bestSize.x > size.x) {
      pos.x += (bestSize.x - size.x) / 2;
    }

    if (bestSize.y > size.y) {
      pos.y += (bestSize.y - size.y) / 2;
    }

    return pos;
  }

  inline GLFWmonitor * getMonitorAtPosition(const ivec2 & position) {
    int count;
    GLFWmonitor ** monitors = glfwGetMonitors(&count);
    for (int i = 0; i < count; ++i) {
      ivec2 candidatePosition;
      glfwGetMonitorPos(monitors[i], &candidatePosition.x, &candidatePosition.y);
      if (candidatePosition == position) {
        return monitors[i];
      }
    }
    return nullptr;
  }

  inline GLFWwindow * createWindow(const uvec2 & size, const ivec2 & position = ivec2(INT_MIN)) {
    GLFWwindow * window = glfwCreateWindow(size.x, size.y, "glfw", nullptr, nullptr);
    if (!window) {
      FAIL("Unable to create rendering window");
    }
    if ((position.x > INT_MIN) && (position.y > INT_MIN)) {
      glfwSetWindowPos(window, position.x, position.y);
    }
    return window;
  }

  inline GLFWwindow * createWindow(int w, int h, int x = INT_MIN, int y = INT_MIN) {
    return createWindow(uvec2(w, h), ivec2(x, y));
  }

  inline GLFWwindow * createFullscreenWindow(const uvec2 & size, GLFWmonitor * targetMonitor) {
    return glfwCreateWindow(size.x, size.y, "glfw", targetMonitor, nullptr);
  }

  inline GLFWwindow * createSecondaryScreenWindow(const uvec2 & size) {
    return createWindow(size, glfw::getSecondaryScreenPosition(size));
  }
}

// A class to encapsulate using GLFW to handle input and render a scene
class GlfwApp {
  uvec2    windowSize;
  ivec2    windowPosition;
  GLFWwindow *  window{ nullptr };

protected:
  oglplus::Context gl;
  unsigned int  frame{ 0 };

public:
  GlfwApp() {
    // Initialize the GLFW system for creating and positioning windows
    if (!glfwInit()) {
      FAIL("Failed to initialize GLFW");
    }
    glfwSetErrorCallback(ErrorCallback);
  }

  virtual ~GlfwApp() {
    if (nullptr != window) {
      glfwDestroyWindow(window);
    }
    glfwTerminate();
  }

  virtual int run() {
    preCreate();

    window = createRenderingTarget(windowSize, windowPosition);

    if (!window) {
      std::cout << "Unable to create OpenGL window" << std::endl;
      return -1;
    }

    postCreate();

    initGl();

    while (!glfwWindowShouldClose(window)) {
      ++frame;
      glfwPollEvents();
      update();
      draw();
      finishFrame();
    }

    return 0;
  }


protected:
  virtual GLFWwindow * createRenderingTarget(uvec2 & size, ivec2 & pos) = 0;

  virtual void draw() = 0;

  void preCreate() {
    glfwWindowHint(GLFW_DEPTH_BITS, 16);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Without this line we get
    // FATAL (86): NSGL: The targeted version of OS X only supports OpenGL 3.2 and later versions if they are forward-compatible
    ON_MAC([]{
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    });
#ifdef DEBUG_BUILD
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
  }

  void postCreate()  {
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Initialize the OpenGL bindings
    // For some reason we have to set this experminetal flag to properly
    // init GLEW if we use a core context.
    glewExperimental = GL_TRUE;
    if (0 != glewInit()) {
      FAIL("Failed to initialize GLEW");
    }
  }

  virtual void initGl() {
  }

  virtual void finishFrame() {
    glfwSwapBuffers(window);
  }

  virtual void destroyWindow() {
    glfwSetKeyCallback(window, nullptr);
    glfwSetMouseButtonCallback(window, nullptr);
    glfwDestroyWindow(window);
  }

  virtual void onKey(int key, int scancode, int action, int mods) {
    if (GLFW_PRESS != action) {
      return;
    }

    switch (key) {
    case GLFW_KEY_ESCAPE:
      glfwSetWindowShouldClose(window, 1);
      return;
    }
  }

  virtual void update() {  }

  virtual void onMouseButton(int button, int action, int mods) { }

protected:
  virtual void viewport(const ivec2 & pos, const uvec2 & size) {
    gl.Viewport(pos.x, pos.y, size.x, size.y);
  }

private:

  static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    GlfwApp * instance = (GlfwApp *)glfwGetWindowUserPointer(window);
    instance->onKey(key, scancode, action, mods);
  }

  static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    GlfwApp * instance = (GlfwApp *)glfwGetWindowUserPointer(window);
    instance->onMouseButton(button, action, mods);
  }

  static void ErrorCallback(int error, const char* description) {
    FAIL(description);
  }
};


//////////////////////////////////////////////////////////////////////
//
// The Oculus VR C API provides access to information about the HMD
// and SDK based distortion.  
//
#include <OVR_CAPI_GL.h>


namespace ovr {

  // Convenience method for looping over each eye with a lambda
  template <typename Function>
  inline void for_each_eye(Function function) {
    for (ovrEyeType eye = ovrEyeType::ovrEye_Left;
      eye < ovrEyeType::ovrEye_Count;
      eye = static_cast<ovrEyeType>(eye + 1)) {
      function(eye);
    }
  }

  inline mat4 toGlm(const ovrMatrix4f & om) {
    return glm::transpose(glm::make_mat4(&om.M[0][0]));
  }

  inline mat4 toGlm(const ovrFovPort & fovport, float nearPlane = 0.01f, float farPlane = 10000.0f) {
    return toGlm(ovrMatrix4f_Projection(fovport, nearPlane, farPlane, true));
  }

  inline vec3 toGlm(const ovrVector3f & ov) {
    return glm::make_vec3(&ov.x);
  }

  inline vec2 toGlm(const ovrVector2f & ov) {
    return glm::make_vec2(&ov.x);
  }

  inline uvec2 toGlm(const ovrSizei & ov) {
    return uvec2(ov.w, ov.h);
  }

  inline quat toGlm(const ovrQuatf & oq) {
    return glm::make_quat(&oq.x);
  }

  inline mat4 toGlm(const ovrPosef & op) {
    mat4 orientation = glm::mat4_cast(toGlm(op.Orientation));
    mat4 translation = glm::translate(mat4(), ovr::toGlm(op.Position));
    return translation * orientation;
  }

  inline ovrMatrix4f fromGlm(const mat4 & m) {
    ovrMatrix4f result;
    mat4 transposed(glm::transpose(m));
    memcpy(result.M, &(transposed[0][0]), sizeof(float) * 16);
    return result;
  }

  inline ovrVector3f fromGlm(const vec3 & v) {
    ovrVector3f result;
    result.x = v.x;
    result.y = v.y;
    result.z = v.z;
    return result;
  }

  inline ovrVector2f fromGlm(const vec2 & v) {
    ovrVector2f result;
    result.x = v.x;
    result.y = v.y;
    return result;
  }

  inline ovrSizei fromGlm(const uvec2 & v) {
    ovrSizei result;
    result.w = v.x;
    result.h = v.y;
    return result;
  }

  inline ovrQuatf fromGlm(const quat & q) {
    ovrQuatf result;
    result.x = q.x;
    result.y = q.y;
    result.z = q.z;
    result.w = q.w;
    return result;
  }
}

class RiftManagerApp {
protected:
  ovrHmd hmd;

  uvec2 hmdNativeResolution;
  ivec2 hmdDesktopPosition;

public:
  RiftManagerApp(ovrHmdType defaultHmdType = ovrHmd_DK2) {
    hmd = ovrHmd_Create(0);
    if (nullptr == hmd) {
      hmd = ovrHmd_CreateDebug(defaultHmdType);
      hmdDesktopPosition = ivec2(100, 100);
    }
    else {
      hmdDesktopPosition = ivec2(hmd->WindowsPos.x, hmd->WindowsPos.y);
    }
    hmdNativeResolution = ivec2(hmd->Resolution.w, hmd->Resolution.h);
  }

  virtual ~RiftManagerApp() {
    ovrHmd_Destroy(hmd);
    hmd = nullptr;
  }

  int getEnabledCaps() {
    return ovrHmd_GetEnabledCaps(hmd);
  }

  void enableCaps(int caps) {
    ovrHmd_SetEnabledCaps(hmd, getEnabledCaps() | caps);
  }

  void toggleCaps(ovrHmdCaps cap) {
    if (cap & getEnabledCaps()) {
      disableCaps(cap);
    }
    else {
      enableCaps(cap);
    }
  }

  void disableCaps(int caps) {
    ovrHmd_SetEnabledCaps(hmd, getEnabledCaps() & ~caps);
  }
};

/**
A class that takes care of the basic duties of putting an OpenGL
window on the desktop in the correct position so that it's visible
through the Rift.
*/
class RiftGlfwApp : public GlfwApp, public RiftManagerApp {

public:
  RiftGlfwApp() {
  }

  virtual ~RiftGlfwApp() {
  }


  virtual GLFWwindow * createRenderingTarget(uvec2 & size, ivec2 & pos) {
    size = hmdNativeResolution;
    pos = hmdDesktopPosition;

    bool directHmdMode = false;
    ON_WINDOWS([&]{
      directHmdMode = (0 == (ovrHmdCap_ExtendDesktop & ovrHmd_GetEnabledCaps(hmd)));
    });

    GLFWwindow * result;
    if (directHmdMode) {
      // In direct mode, try to put the output window on a secondary screen
      // (for easier debugging, assuming your dev environment is on the primary)
      result = glfw::createSecondaryScreenWindow(size);
    } else {
      glfwWindowHint(GLFW_DECORATED, 0);
      result = glfw::createWindow(size, pos);
    }

    ON_WINDOWS([&]{
      if (directHmdMode) {
        // If we're in direct mode, attach to the window
        ovrHmd_AttachToWindow(hmd, glfwGetWin32Window(result), nullptr, nullptr);
      }
    });
    
    return result;
  }
};

class RiftApp : public RiftGlfwApp {
public:

protected:
  ovrTexture eyeTextures[2];
  ovrVector3f eyeOffsets[2];

private:
  ovrEyeRenderDesc eyeRenderDescs[2];
  mat4 projections[2];
  ovrPosef eyePoses[2];
  ovrEyeType currentEye;
  fbo_wrapper_ptr eyeFbos[2];

public:

  RiftApp() {
    if (!ovrHmd_ConfigureTracking(hmd,
      ovrTrackingCap_Orientation | ovrTrackingCap_Position | ovrTrackingCap_MagYawCorrection, 0)) {
      FAIL("Could not attach to sensor device");
    }

    memset(eyeTextures, 0, 2 * sizeof(ovrGLTexture));

    ovr::for_each_eye([&](ovrEyeType eye){
      ovrSizei eyeTextureSize = ovrHmd_GetFovTextureSize(hmd, eye, hmd->MaxEyeFov[eye], 1.0f);
      ovrTextureHeader & eyeTextureHeader = eyeTextures[eye].Header;
      eyeTextureHeader.TextureSize = eyeTextureSize;
      eyeTextureHeader.RenderViewport.Size = eyeTextureSize;
      eyeTextureHeader.API = ovrRenderAPI_OpenGL;
    });
  }

  virtual ~RiftApp() {
  }

protected:
  virtual void initGl() {
    RiftGlfwApp::initGl();

    ovrGLConfig cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.OGL.Header.API = ovrRenderAPI_OpenGL;
    cfg.OGL.Header.RTSize = ovr::fromGlm(hmdNativeResolution);
    cfg.OGL.Header.Multisample = 0;

    int distortionCaps = 0
      | ovrDistortionCap_Vignette
      | ovrDistortionCap_Chromatic
      | ovrDistortionCap_TimeWarp
      ;

    int configResult = ovrHmd_ConfigureRendering(hmd, &cfg.Config,
      distortionCaps, hmd->MaxEyeFov, eyeRenderDescs);


    ovr::for_each_eye([&](ovrEyeType eye){
      const ovrEyeRenderDesc & erd = eyeRenderDescs[eye];
      ovrMatrix4f ovrPerspectiveProjection = ovrMatrix4f_Projection(erd.Fov, 0.01f, 100000.0f, true);
      projections[eye] = ovr::toGlm(ovrPerspectiveProjection);
      eyeOffsets[eye] = erd.HmdToEyeViewOffset;

      // Allocate the frameBuffer that will hold the scene, and then be
      // re-rendered to the screen with distortion
      auto & eyeTextureHeader = eyeTextures[eye];
      eyeFbos[eye] = fbo_wrapper_ptr(new FboWrapper());
      eyeFbos[eye]->init(ovr::toGlm(eyeTextureHeader.Header.TextureSize));
      // Get the actual OpenGL texture ID
      ((ovrGLTexture&)eyeTextureHeader).OGL.TexId = oglplus::GetName(eyeFbos[eye]->color);
    });
  }


  // Override the base class to prevent the swap buffer call, because OVR does it in end frame
  virtual void finishFrame() {
  }

  virtual void onKey(int key, int scancode, int action, int mods) {
    if (GLFW_PRESS == action) switch (key) {
    case GLFW_KEY_R:
      ovrHmd_RecenterPose(hmd);
      return;
    }

    RiftGlfwApp::onKey(key, scancode, action, mods);
  }

  virtual void draw() final {
    ovrHmd_GetEyePoses(hmd, frame, eyeOffsets, eyePoses, nullptr);

    ovrHmd_BeginFrame(hmd, frame);
    for (int i = 0; i < 2; ++i) {
      ovrEyeType eye = currentEye = hmd->EyeRenderOrder[i];
      const ovrRecti & vp = eyeTextures[eye].Header.RenderViewport;

      // Render the scene to an offscreen buffer
      eyeFbos[eye]->fbo.Bind(oglplus::Framebuffer::Target::Draw);
      gl.Viewport(vp.Pos.x, vp.Pos.y, vp.Size.w, vp.Size.h);
      renderScene(projections[eye], ovr::toGlm(eyePoses[eye]));
    }
    oglplus::DefaultFramebuffer().Bind(oglplus::Framebuffer::Target::Draw);
    ovrHmd_EndFrame(hmd, eyePoses, eyeTextures);
  }

  virtual void renderScene(const glm::mat4 & projection, const glm::mat4 & headPose) = 0;
};

// An example application that renders a simple cube
class ExampleApp : public RiftApp {
  mat4 modelview;
  float ipd{ OVR_DEFAULT_IPD };
  std::unique_ptr<ColorCubeScene> cubeScene;

public:
  ExampleApp() {
    modelview = glm::lookAt(glm::vec3(0, 0, OVR_DEFAULT_IPD * 5.0f), glm::vec3(0), glm::vec3(0, 1, 0));
  }

protected:
  virtual void initGl() {
    RiftApp::initGl();
    using namespace oglplus;
    Context::ClearColor(0.2f, 0.2f, 0.2f, 0.0f);
    Context::ClearDepth(1.0f);
    Context::Disable(Capability::Dither);
//    Context::Enable(Capability::CullFace);
    Context::CullFace(Face::Back);
    Context::Enable(Capability::DepthTest);

    cubeScene = std::unique_ptr<ColorCubeScene>(new ColorCubeScene());
    ovrHmd_RecenterPose(hmd);
  }

  virtual void update() {
    RiftApp::update();
    // Update stuff
  }

  void renderScene(const glm::mat4 & projection, const glm::mat4 & headPose) {
    // Clear the scene
    oglplus::Context::Clear().ColorBuffer().DepthBuffer();
    // apply the head pose to the current modelview matrix
    glm::mat4 modelview = glm::inverse(headPose) * this->modelview;
    // Scale the size of the cube to the distance between the eyes
    modelview = glm::scale(modelview, glm::vec3(ipd));
    // Render the cube
    cubeScene->render(projection, modelview);
  }
};

// Execute our example class
MAIN_DECL{
  int result = -1;
  try {
    if (!ovr_Initialize()) {
      FAIL("Failed to initialize the Oculus SDK");
    }
    result = ExampleApp().run();
  } catch (std::exception & error) {
    std::cerr << error.what() << std::endl;
  }
  ovr_Shutdown(); 
  return result;
}