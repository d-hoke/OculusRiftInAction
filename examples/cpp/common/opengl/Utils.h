#pragma once

typedef std::shared_ptr<oglplus::shapes::ShapeWrapper> ShapeWrapperPtr;

namespace oria {

  inline void bindLights(ProgramPtr & program) {
    using namespace oglplus;
    Lights & lights = Stacks::lights();
    int count = (int)lights.lightPositions.size();
    Uniform<vec4>(*program, "Ambient").Set(lights.ambient);
    Uniform<int>(*program, "LightCount").Set(count);
    if (count) {
      Uniform<Vec4f>(*program, "LightColor[0]").SetValues(count, (Vec4f*)&lights.lightColors.at(0).r);
      Uniform<Vec4f>(*program, "LightPosition[0]").SetValues(count, (Vec4f*)&lights.lightPositions.at(0).x);
    }
  }

  inline void renderGeometry(ShapeWrapperPtr & shape, ProgramPtr & program, std::initializer_list<std::function<void()>> list) {
    program->Use();

    Mat4Uniform(*program, "ModelView").Set(Stacks::modelview().top());
    Mat4Uniform(*program, "Projection").Set(Stacks::projection().top());

    std::for_each(list.begin(), list.end(), [&](const std::function<void()>&f){
      f();
    });

    shape->Use();
    shape->Draw();

    oglplus::NoProgram().Bind();
    oglplus::NoVertexArray().Bind();
  }

  inline void renderGeometry(ShapeWrapperPtr & shape, ProgramPtr & program) {
    renderGeometry(shape, program, {});
  }

  void renderCube(const glm::vec3 & color = Colors::white);
  void renderColorCube();
  void renderSkybox(Resource firstImageResource);
  void renderFloor();
  void renderManikin();
}
