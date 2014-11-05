#pragma once

typedef oglplus::Uniform<mat4> Mat4Uniform;
typedef std::shared_ptr<oglplus::Program> ProgramPtr;

namespace oria {
  ProgramPtr loadProgram(Resource vs, Resource fs);
}
