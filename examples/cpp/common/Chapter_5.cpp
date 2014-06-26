#include "Common.h"
#include "Chapter_5.h"
#include <glm/gtx/string_cast.hpp>

Chapter_5::Chapter_5() {
  ipd = ovrHmd_GetFloat(hmd, OVR_KEY_IPD, OVR_DEFAULT_IPD);
  eyeHeight = ovrHmd_GetFloat(hmd,
      OVR_KEY_PLAYER_HEIGHT,
      OVR_DEFAULT_PLAYER_HEIGHT);
  resetCamera();
}

void Chapter_5::initGl() {
  RiftGlfwApp::initGl();

  program = GlUtils::getProgram(
      Resource::SHADERS_LITCOLORED_VS,
      Resource::SHADERS_LITCOLORED_FS);
  wireProgram = GlUtils::getProgram(
    Resource::SHADERS_SIMPLE_VS,
    Resource::SHADERS_COLORED_FS);
  cube = GlUtils::getColorCubeGeometry();
  wireCube = GlUtils::getWireCubeGeometry();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_LINE_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glLineWidth(4);
  glClearColor(0.65f, 0.65f, 0.65f, 1);

  gl::Stacks::lights().addLight(glm::vec4(50, 50, 50, 1));
}

void Chapter_5::onKey(int key, int scancode, int action, int mods) {
  if (CameraControl::instance().onKey(key, scancode, action, mods)) {
    return;
  }

  if (GLFW_PRESS == action) {
    switch (key) {
    case GLFW_KEY_R:
      resetCamera();
      ovrHmd_ResetSensor(hmd);
      return;
    case GLFW_KEY_LEFT_BRACKET:
      SAY("%s", glm::to_string(camera).c_str());
      return;
    case GLFW_KEY_RIGHT_BRACKET:
      camera = glm::mat4x4(
          1.000000, -0.000000, 0.000000, -0.000000,
          -0.000000, 0.998896, -0.046983, 0.000000,
          0.000000, 0.046983, 0.998896, -0.000000,
          -0.000000, -2.200359, -15.852616, 1.000000);
      return;
    }
  }

  RiftGlfwApp::onKey(key, scancode, action, mods);
}

void Chapter_5::update() {

  // We invert the camera matrix to generate a "player" matrix,
  // because our interaction code works from the player's point
  // of view instead of the world's view of the camera.
  glm::mat4 player = glm::inverse(camera);
  CameraControl::instance().applyInteraction(player);
  camera = glm::inverse(player);

  gl::Stacks::modelview().top() = camera;
}

void Chapter_5::resetCamera() {
  camera = glm::lookAt(
      glm::vec3(0, eyeHeight, 0.5f),  // Position of the camera
      glm::vec3(0, eyeHeight, 0),     // Where the camera is looking
      GlUtils::Y_AXIS);               // Camera up axis
}


template <typename Function>
void drawScene(float eyeHeight, float ipd, Function render) {
  // Draw arches made of cubes.
  for (int x = -10; x <= 10; x++) {
    for (int y = 0; y <= 10; y++) {
      for (int z = -10; z <= 10; z++) {
        glm::vec3 pos(x, y, z);
        float len = glm::length(pos);
        if (len > 11 && len < 12) {
          render(pos, GlUtils::ONE);
        }
      }
    }
  }

  // Draw a floor made of cubes.
  for (int x = -12; x <= 12; x++) {
    for (int z = -12; z <= 12; z++) {
      render(glm::vec3(x, -0.5f, z), GlUtils::ONE);
    }
  }

  // Draw a single cube at the center of the room, at eye height,
  // and put it on a pedestal.
  render(glm::vec3(0, eyeHeight, 0), glm::vec3(ipd));
  render(glm::vec3(0, eyeHeight / 2, 0), glm::vec3(ipd / 2, eyeHeight, ipd / 2));
}


void Chapter_5::drawChapter5Scene() {
  gl::MatrixStack & mv = gl::Stacks::modelview();
  gl::MatrixStack & pv = gl::Stacks::projection();

  program->use();
  gl::Stacks::lights().apply(*program);
  gl::Stacks::projection().apply(*program);
  cube->bindVertexArray();
  drawScene(eyeHeight, ipd, [&](const glm::vec3 & tr, const glm::vec3 & sc){
    mv.push().translate(tr).scale(sc).apply(*program).pop();
    cube->draw();
  });
  cube->unbindVertexArray();

  wireProgram->use();
  gl::Stacks::lights().apply(*wireProgram);
  gl::Stacks::projection().apply(*wireProgram);
  wireProgram->setUniform("Color", glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
  wireCube->bindVertexArray();
  pv.withPush([&]{
    pv.top() = glm::translate(glm::mat4(), glm::vec3(0, 0, -0.00001)) * pv.top();
    pv.apply(*wireProgram);
    drawScene(eyeHeight, ipd, [&](const glm::vec3 & tr, const glm::vec3 & sc){
      mv.push().translate(tr).scale(sc).apply(*wireProgram).pop();
      wireCube->draw();
    });
  });
  wireCube->unbindVertexArray();
  gl::VertexArray::unbind();
  gl::Program::clear();
}

void Chapter_5::drawCube(const glm::vec3 & translate, const glm::vec3 & scale) {
}