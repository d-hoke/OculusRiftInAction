#include "Common.h"
#include <oglplus/shapes/plane.hpp>

static const glm::uvec2 WINDOW_SIZE(1280, 800);
static const glm::ivec2 WINDOW_POS(100, 100);
static const glm::uvec2 RES(128, 80);

using namespace oglplus;

struct Screen {
  uvec2 resolution;
  TexturePtr texture;
};

struct RasterizerExample : public GlfwApp {
  ProgramPtr program;
  ShapeWrapperPtr shape;
  BufferPtr edge;

  virtual GLFWwindow * createRenderingTarget(glm::uvec2 & outSize, glm::ivec2 & outPosition) {
    outSize = WINDOW_SIZE;
    outPosition = WINDOW_POS;
    return glfw::createWindow(outSize, outPosition);
  }

  virtual void initGl() {
    Context::ClearColor(0.1f, 0.1f, 0.1f, 1);

    program = oria::loadProgram(
      Resource::SHADERS_RASTERIZER_VS,
      Resource::SHADERS_RASTERIZER_FS);
    
    shapes::Plane plane(Vec3f(1, 0, 0), Vec3f(0, 1, 0)); 

    const std::initializer_list<const GLchar*> names = { "Position", "TexCoord" };

    shape = ShapeWrapperPtr(new shapes::ShapeWrapper(names, plane, *program));
    //vao = VertexArrayPtr(new VertexArray());
    //vao->Bind();
    //pos = BufferPtr(new Buffer());
    //tex = BufferPtr(new Buffer());
    edge = BufferPtr(new Buffer());
    //ind = BufferPtr(new Buffer());

    //{
    std::vector<GLfloat> posv;
    plane.Positions(posv);

    //  pos->Bind(Buffer::Target::Array);
    //  Buffer::Data(Buffer::Target::Array, posv);
    //  VertexArrayAttrib(*program, oria::Layout::Attribute::Position)
    //    .Setup<GLfloat>(3)
    //    .Enable();

    //  std::vector<GLfloat> texv;
    //  plane.TexCoordinates(texv);

    //  tex->Bind(Buffer::Target::Array);
    //  Buffer::Data(Buffer::Target::Array, texv);
    //  VertexArrayAttrib(*program, oria::Layout::Attribute::TexCoord0)
    //    .Setup<GLfloat>(2)
    //    .Enable();

    std::vector<GLfloat> edgev;
    for (int i = 0; i < posv.size(); i += 3) {
      float x = posv[i] * RES.x / 2;
      edgev.push_back(x);
      float y = posv[i + 1] * RES.y / 2;
      edgev.push_back(y);
    }

    //instr.Draw(indInfo);
    shape->Use();

    edge->Bind(Buffer::Target::Array);
    Buffer::Data(Buffer::Target::Array, edgev);
    VertexArrayAttrib(*program, 4)
      .Setup<GLfloat>(2)
      .Enable();
    NoVertexArray().Bind();
  }

  virtual void draw() {
    Context::Clear().ColorBuffer();
    oria::renderGeometry(shape, program);
  }
};

RUN_APP(RasterizerExample);

