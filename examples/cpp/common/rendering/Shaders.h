#pragma once

namespace Layout {
  namespace Attribute {
    enum {
      Position = 0,
      TexCoord0 = 1,
      Normal = 2,
      Color = 3,
      TexCoord1 = 4,
      InstanceTransform = 5,
    };
  }

  namespace Uniform {
    enum {
      Projection = 0,
      ModelView = 1,
      NormalMatrix = 2,
      Time = 3,
      Color = 4,
      LightAmbient = 8,
      LightCount = 9,
      ForceAlpha = 10,
      LightPosition = 16,
      LightColor = 24,
    };
  }
}

#define SET_UNIFORM(p, u, t, v) \
  oglplus::Uniform<t>(p, Layout::Uniform::u).Set(v)

#define SET_PROJECTION(program) \
  SET_UNIFORM(program, Projection, mat4, Stacks::projection().top())

#define SET_MODELVIEW(program) \
  SET_UNIFORM(program, ModelView, mat4, Stacks::modelview().top())

#define SET_LIGHTS(program, lights) \
  SET_UNIFORM(program, LightAmbient, vec4, lights.ambient); \
  SET_UNIFORM(program, LightCount, int, lights.lightPositions.size()); \
  for(size_t  i = 0; i < lights.lightPositions.size(); ++i) { \
    SET_UNIFORM(program, LightPosition + i, vec3, lights.lightPositions[i]); \
    SET_UNIFORM(program, LightColor + i, vec4, lights.lightColors[i]); \
  }
