#pragma once

typedef std::shared_ptr<oglplus::shapes::ShapeWrapper> ShapeWrapperPtr;
typedef std::shared_ptr<oglplus::Buffer> BufferPtr;
typedef std::shared_ptr<oglplus::VertexArray> VertexArrayPtr;

namespace oria {

  ShapeWrapperPtr loadShape(const std::initializer_list<const GLchar*>& names, Resource resource, ProgramPtr program);

  void bindLights(ProgramPtr & program);

  void renderGeometry(ShapeWrapperPtr & shape, ProgramPtr & program, std::initializer_list<std::function<void()>> list);
  void renderGeometry(ShapeWrapperPtr & shape, ProgramPtr & program);
  void renderCube(const glm::vec3 & color = Colors::white);
  void renderColorCube();
  void renderSkybox(Resource firstImageResource);
  void renderFloor();
  void renderManikin();
  void renderRift();
  void renderArtificialHorizon(float alpha = 0.0f);

  void renderString(const std::string & str, glm::vec2 & cursor,
      float fontSize = 12.0f, Resource font =
          Resource::FONTS_INCONSOLATA_MEDIUM_SDFF);

  void renderString(const std::string & str, glm::vec3 & cursor,
      float fontSize = 12.0f, Resource font =
          Resource::FONTS_INCONSOLATA_MEDIUM_SDFF);

}
