#include "Common.h"


struct PerEyeArg {
  glm::ivec2 viewportPosition;
  glm::mat4 modelviewOffset;
};

class CubeScene_Stereo : public GlfwApp {
  PerEyeArg eyes[2];
  uvec2 eyeSize;

public:
  CubeScene_Stereo() {
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
    MatrixStack & mv = Stacks::modelview();

    for (int i = 0; i < 2; ++i) {
      PerEyeArg & eyeArgs = eyes[i];
      viewport(eyeSize, eyeArgs.viewportPosition);
      Stacks::withPush(mv, [&]{
        mv.preMultiply(eyeArgs.modelviewOffset);
        oria::renderExampleScene(
          OVR_DEFAULT_IPD, OVR_DEFAULT_EYE_HEIGHT);
      });
    }
  }
protected:
    void onSized(const glm::uvec2& newSize) {
        GlfwApp::onSized(newSize);
        eyeSize = newSize;
        eyeSize.x /= 2;
        Stacks::projection().top() = glm::perspective(
            PI / 2.0f, aspect(eyeSize), 0.01f, 100.0f);
        glm::vec3 offset(OVR_DEFAULT_IPD / 2.0f, 0, 0);
        eyes[ovrEye_Left] = {
            glm::ivec2(0, 0),
            glm::translate(glm::mat4(), offset)
        };
        eyes[ovrEye_Right] = {
            glm::ivec2(eyeSize.x, 0),
            glm::translate(glm::mat4(), -offset)
        };
    }

private:
};

RUN_APP(CubeScene_Stereo);
