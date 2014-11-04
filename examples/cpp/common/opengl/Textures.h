#pragma once

namespace oria {
  typedef std::shared_ptr<oglplus::Texture> TexturePtr;

  TexturePtr load2dTexture(Resource resource);
  TexturePtr loadCubemapTexture(Resource firstResource);
}
