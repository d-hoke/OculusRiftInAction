#pragma once

namespace oria {

  typedef oglplus::Uniform<mat4> Mat4Uniform;
  typedef std::shared_ptr<oglplus::Program> ProgramPtr;


  //inline bool resourceChanged(Resource resource) {
  //  typedef std::unordered_map<Resource, time_t> ResourceAgeMap;
  //  static ResourceAgeMap resourceAgeMap;
  //  if (!resourceAgeMap.count(resource)) {
  //    return true;
  //  }
  //  return Resources::getResourceModified(resource) > resourceAgeMap[resource];
  //}

  inline  ProgramPtr loadProgram(Resource vs, Resource fs) {
    typedef std::unordered_map<std::string, ProgramPtr> ProgramMap;

    static ProgramMap programs;
    static bool registeredShutdown = false;
    if (!registeredShutdown) {
      addShudownHook([&]{
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
        programs[key] = result;
      }
      catch (ProgramBuildError & err) {
        OVR_UNUSED(err);
        SAY_ERR((const char*)err.Message);
        return nullptr;
      }
    }
    return programs[key];
  }

}


//#define SET_UNIFORM(p, u, t, v) \
//    oglplus::Uniform<t>(p, Layout::Uniform::u).Set(v)
//
//
//#define SET_PROJECTION(program) \
//  SET_UNIFORM(program, Projection, mat4, Stacks::projection().top())
//
//#define SET_MODELVIEW(program) \
//  SET_UNIFORM(program, ModelView, mat4, Stacks::modelview().top())
//
//#define SET_LIGHTS(program, lights) \
//  SET_UNIFORM(program, LightAmbient, vec4, lights.ambient); \
//  SET_UNIFORM(program, LightCount, int, lights.lightPositions.size()); \
//  for(size_t  i = 0; i < lights.lightPositions.size(); ++i) { \
//    SET_UNIFORM(program, LightPosition + i, vec3, lights.lightPositions[i]); \
//    SET_UNIFORM(program, LightColor + i, vec4, lights.lightColors[i]); \
//  }
