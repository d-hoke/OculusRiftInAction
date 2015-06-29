#include "Common.h"

class CubeScene_Mono : public GlfwApp {
public:
  CubeScene_Mono() {
    Stacks::modelview().top() = glm::lookAt(
      vec3(0, OVR_DEFAULT_EYE_HEIGHT, 0.5f), 
      vec3(0, OVR_DEFAULT_EYE_HEIGHT, 0), 
      Vectors::UP);
  }

  virtual GLFWwindow * createRenderingTarget(
    glm::uvec2 & outSize, glm::ivec2 & outPosition) 
  {
    static const glm::uvec2 WINDOW_SIZE(1280, 800);
    static const glm::ivec2 WINDOW_POS(100, 100);
    outSize = WINDOW_SIZE;
    outPosition = WINDOW_POS;
    return glfw::createWindow(outSize, outPosition);
  }

  virtual void draw() {
    oglplus::Context::Clear().ColorBuffer().DepthBuffer();
    Stacks::projection().top() = glm::perspective(
        PI / 2.0f, aspect(getSize()), 0.01f, 100.0f);
    oria::renderExampleScene(
      OVR_DEFAULT_IPD, OVR_DEFAULT_EYE_HEIGHT);
  }
};

RUN_APP(CubeScene_Mono);
