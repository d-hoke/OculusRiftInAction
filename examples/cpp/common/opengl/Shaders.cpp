/************************************************************************************
 
 Authors     :   Bradley Austin Davis <bdavis@saintandreas.org>
 Copyright   :   Copyright Brad Davis. All Rights reserved.
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 
 ************************************************************************************/

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
        SAY_ERR((const char*)err.Message);
        return nullptr;
      }
    }
    return programs[key];
  }

}
