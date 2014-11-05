#include "Common.h"

namespace oria {

  ProgramPtr loadProgram(Resource vs, Resource fs) {
    typedef std::unordered_map<std::string, ProgramPtr> ProgramMap;

    static ProgramMap programs;
    static bool registeredShutdown = false;
    if (!registeredShutdown) {
      Platform::addShutdownHook([&]{
        programs.clear();
      });
      registeredShutdown = true;
    }

    std::string key = Resources::getResourcePath(vs) + ":" +
      Resources::getResourcePath(fs);
    if (!programs.count(key)) {
      using namespace oglplus;
      ProgramPtr result = ProgramPtr(new Program());
      try {
        std::string vsSource = Platform::getResourceData(vs);
        std::string fsSource = Platform::getResourceData(fs);
        // attach the shaders to the program
        result->AttachShader(
          VertexShader()
          .Source(GLSLSource(vsSource))
          .Compile()
          );
        result->AttachShader(
          FragmentShader()
          .Source(GLSLSource(Platform::getResourceData(fs)))
          .Compile()
          );
        result->Link();
//        programs[key] = result;
        return result;
      }
      catch (ProgramBuildError & err) {
        err;
        SAY_ERR((const char*)err.Message);
        return nullptr;
      }
    }
    return programs[key];
  }

}
