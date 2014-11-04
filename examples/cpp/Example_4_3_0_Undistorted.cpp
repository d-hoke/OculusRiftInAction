#include "Common.h"

Resource SCENE_IMAGES_DK1[2] = {
  Resource::IMAGES_TUSCANY_UNDISTORTED_LEFT_DK1_PNG,
  Resource::IMAGES_TUSCANY_UNDISTORTED_RIGHT_DK1_PNG
};

Resource SCENE_IMAGES_DK2[2] = {
  Resource::IMAGES_TUSCANY_UNDISTORTED_LEFT_DK2_PNG,
  Resource::IMAGES_TUSCANY_UNDISTORTED_RIGHT_DK2_PNG
};

class UndistortedExample : public RiftGlfwApp {

protected:
  oria::TexturePtr textures[2];
  oria::ProgramPtr program;
  ShapeWrapperPtr shape;

public:
  virtual ~UndistortedExample() {
  }

  void initGl() {
    using namespace oglplus;
    RiftGlfwApp::initGl();
    Context::Disable(Capability::Blend);
    Context::Disable(Capability::DepthTest);

    program = oria::loadProgram(
        Resource::SHADERS_TEXTURED_VS,
        Resource::SHADERS_TEXTURED_FS);
    shape = ShapeWrapperPtr(new shapes::ShapeWrapper(List("Position")("TexCoord").Get(), shapes::Plane(), *program));

    Resource * sceneImages = SCENE_IMAGES_DK2;
    if (hmd->Type == ovrHmd_DK1) {
      sceneImages = SCENE_IMAGES_DK1;
    }

    for_each_eye([&](ovrEyeType eye) {
      textures[eye] = oria::load2dTexture(sceneImages[eye]);
    });
  }

  virtual void shutdownGl() {
    program.reset();
    shape.reset();
    for_each_eye([&](ovrEyeType eye) {
      textures[eye].reset();
    });
    RiftGlfwApp::shutdownGl();
  }


  void draw() {
    oglplus::Context::Clear().ColorBuffer();
    for_each_eye([&](ovrEyeType eye) {
      renderEye(eye);
    });
  }

  void renderEye(ovrEyeType eye) {
    viewport(eye);
    textures[eye]->Bind(oglplus::Texture::Target::_2D);
    oria::renderGeometry(shape, program);
  }
};

RUN_OVR_APP(UndistortedExample)
