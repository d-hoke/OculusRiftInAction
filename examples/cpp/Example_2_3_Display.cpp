#include "Common.h"


class RiftDisplay : public GlfwApp {
  glm::uvec2 eyeSize;
  bool directMode{ false };
  void * nativeWindow{ nullptr };
  ovrHmd hmd;

public:
RiftDisplay() {
  hmd = ovrHmd_Create(0);
  if (!hmd) {
    FAIL("Unable to detect Rift display");
  }

  if (directMode) {
    // FIXME (find a monitor to place the window on)
    windowPosition = glm::ivec2(
      hmd->WindowsPos.x,
      hmd->WindowsPos.y);

    windowSize = glm::uvec2(
      hmd->Resolution.w,
      hmd->Resolution.h);
  } else {
    windowPosition = glm::ivec2(
      hmd->WindowsPos.x,
      hmd->WindowsPos.y);

    // FIXME (query actual size)
    windowSize = glm::uvec2(
      hmd->Resolution.w,
      hmd->Resolution.h);
  }

  eyeSize = windowSize;
  eyeSize.x /= 2;
}

void createRenderingTarget() {
  if (directMode) {
    // FIXME when the SDK works properly with different sized windows in GL
  } else {
    glfwWindowHint(GLFW_DECORATED, 0);
  }

  window = glfw::createWindow(windowSize, windowPosition);
  glfwMakeContextCurrent(window);

  nativeWindow = glfw::getNativeWindowHandle(window);
  ovrHmd_AttachToWindow(hmd, nativeWindow, nullptr, nullptr);

  unsigned int enabledCaps = ovrHmd_GetEnabledCaps(hmd);
  if (0 == (ovrHmdCap_ExtendDesktop & enabledCaps)) {
    ON_WINDOWS([&]{
      directMode = true;
    });
  }

  if (directMode) {
    nativeWindow = glfw::getNativeWindowHandle(window);
    ovrHmd_AttachToWindow(hmd, nativeWindow, nullptr, nullptr);
  } else {
    if (glfwGetWindowAttrib(window, GLFW_DECORATED)) {
      FAIL("Unable to create undecorated window");
    }
  }
}

void initGl() {
  GlfwApp::initGl();
  glEnable(GL_SCISSOR_TEST);
}

void draw() {
  glm::ivec2 position = glm::ivec2(0, 0);
  glm::vec4 color = glm::vec4(1, 0, 0, 1);

  oglplus::Context::Scissor(position.x, position.y, eyeSize.x, eyeSize.y);
  oglplus::Context::ClearColor(color.r, color.g, color.b, color.a);
  oglplus::Context::Clear().ColorBuffer();

  position = glm::ivec2(eyeSize.x, 0);
  color = glm::vec4(0, 0, 1, 1);

  oglplus::Context::Scissor(position.x, position.y, eyeSize.x, eyeSize.y);
  oglplus::Context::ClearColor(color.r, color.g, color.b, color.a);
  oglplus::Context::Clear().ColorBuffer();
}
};

RUN_OVR_APP(RiftDisplay);
