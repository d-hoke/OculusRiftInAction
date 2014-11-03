#pragma once

typedef std::shared_ptr<oglplus::shapes::ShapeWrapper> ShapeWrapperPtr;

namespace oria {

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
    program->Use();

    Mat4Uniform(*program, "ModelView").Set(Stacks::modelview().top());
    Mat4Uniform(*program, "Projection").Set(Stacks::projection().top());

    shape->Use();
    shape->Draw();

    oglplus::NoProgram().Bind();
    oglplus::NoVertexArray().Bind();
  }

  inline void renderCube(const glm::vec3 & color = Colors::white) {
    using namespace oglplus;

    static ProgramPtr program; 
    static ShapeWrapperPtr shape;
    if (!program) {
      program = loadProgram(Resource::SHADERS_SIMPLE_VS, Resource::SHADERS_COLORED_FS);
      shape = ShapeWrapperPtr(new shapes::ShapeWrapper(List("Position").Get(), shapes::Cube(), *program));
      oria::addShudownHook([&]{
        program.reset();
        shape.reset();
      });
    }
    program->Use();
    Uniform<vec4>(*program, "Color").Set(vec4(color, 1));
    renderGeometry(shape, program);
  }

  inline void renderColorCube() {
    using namespace oglplus;

    static ProgramPtr program; 
    static ShapeWrapperPtr shape;
    if (!program) {
      program = loadProgram(Resource::SHADERS_COLORCUBE_VS, Resource::SHADERS_COLORED_FS);
      shape = ShapeWrapperPtr(new shapes::ShapeWrapper(List("Position")("Normal").Get(), shapes::Cube(), *program));;
      oria::addShudownHook([&]{
        program.reset();
        shape.reset();
      });
    }
    
    renderGeometry(shape, program);
  }

  inline void renderSkybox(Resource firstImageResource) {
    using namespace oglplus;

    static ProgramPtr program;
    static ShapeWrapperPtr shape;
    if (!program) {
      program = loadProgram(Resource::SHADERS_CUBEMAP_VS, Resource::SHADERS_CUBEMAP_FS);
      shape = ShapeWrapperPtr(new shapes::ShapeWrapper(List("Position").Get(), shapes::SkyBox(), *program));
      oria::addShudownHook([&]{
        program.reset();
        shape.reset();
      });
    }

    TexturePtr texture = loadCubemapTexture(firstImageResource);
    texture->Bind(TextureTarget::CubeMap);
    MatrixStack & mv = Stacks::modelview();
    mv.withPush([&]{
      Context::Disable(Capability::DepthTest);
      Context::Disable(Capability::CullFace);
      renderGeometry(shape, program);
      Context::Enable(Capability::CullFace);
      Context::Enable(Capability::DepthTest);
    });
    DefaultTexture().Bind(TextureTarget::CubeMap);
  }

  inline void renderFloor() {
    using namespace oglplus;
    const float SIZE = 100;
    static ProgramPtr program;
    static ShapeWrapperPtr shape; 
    static TexturePtr texture;
    if (!program) {
      program = loadProgram(Resource::SHADERS_TEXTURED_VS, Resource::SHADERS_TEXTURED_FS);
      shape = ShapeWrapperPtr(new shapes::ShapeWrapper(List("Position")("TexCoord").Get(), shapes::Plane(), *program));
      texture = load2dTexture(Resource::IMAGES_FLOOR_PNG);
      Context::Bound(TextureTarget::_2D, *texture).MinFilter(TextureMinFilter::LinearMipmapNearest).GenerateMipmap();
      oria::addShudownHook([&]{
        program.reset();
        shape.reset();
        texture.reset();
      });
    }

    texture->Bind(TextureTarget::_2D);
    MatrixStack & mv = Stacks::modelview();
    mv.withPush([&]{
      mv.scale(vec3(SIZE));
      renderGeometry(shape, program, { [&]{
        oglplus::Uniform<vec2>(*program, "UvMultiplier").Set(vec2(SIZE * 2.0f));
      } });
    });

    DefaultTexture().Bind(TextureTarget::_2D);
  }

}