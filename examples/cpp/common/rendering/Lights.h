#pragma once

struct Light {
  vec3 position;
  vec4 color;

  Light(const vec3 & position = vec3(1), const vec4 & color = vec4(1)) {
    this->position = position;
    this->color = color;
  }
};

class Lights {
public:
  std::vector<vec3> lightPositions;
  std::vector<vec4> lightColors;
  vec4 ambient;

  // Singleton class
  Lights()
      : ambient(glm::vec4(0.2, 0.2, 0.2, 1.0)) {
    addLight();
  }

  void addLight(const glm::vec3 & position = vec3(1),
      const vec4 & color = glm::vec4(1)) {
    lightPositions.push_back(position);
    lightColors.push_back(color);
  }

  void addLight(const Light & light) {
    addLight(light.position, light.color);
  }

  void setAmbient(const glm::vec4 & ambient) {
    this->ambient = ambient;
  }
};
