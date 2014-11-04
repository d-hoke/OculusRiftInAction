#pragma once

typedef std::shared_ptr<oglplus::shapes::ShapeWrapper> ShapeWrapperPtr;


#define GL_CHECK_ERROR oria::checkGlError();

namespace oria {
  inline void checkGlError() {
    GLenum error = glGetError(); 
    if (error != 0) {
      FAIL("GL error %d", error);
    } 
  }
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
    program->Use();
    GL_CHECK_ERROR;

    Mat4Uniform(*program, "ModelView").Set(Stacks::modelview().top());
    Mat4Uniform(*program, "Projection").Set(Stacks::projection().top());
    GL_CHECK_ERROR;

    shape->Use();
    shape->Draw();
    GL_CHECK_ERROR;

    oglplus::NoProgram().Bind();
    GL_CHECK_ERROR;

    oglplus::NoVertexArray().Bind();
    GL_CHECK_ERROR;

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

  inline void renderManikin() {
    using namespace oglplus;

    static ProgramPtr program;
    static ShapeWrapperPtr shape;
    if (!program) {
      oria::addShudownHook([&]{
        program.reset();
        shape.reset();
      });
      program = loadProgram(Resource::SHADERS_LIT_VS, Resource::SHADERS_LITCOLORED_FS);
      shape = ShapeWrapperPtr(new shapes::ShapeWrapper(List("Position")("Normal").Get(), shapes::CtmMesh(Resource::MESHES_MANIKIN_CTM), *program));;
    }

    renderGeometry(shape, program, { [&]{
      bindLights(program);
    } });
  }

}
