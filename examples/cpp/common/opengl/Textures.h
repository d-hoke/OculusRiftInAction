#pragma once

typedef std::shared_ptr<oglplus::Texture> TexturePtr;
typedef std::shared_ptr<oglplus::images::Image> ImagePtr;

namespace oria {
  ImagePtr loadPngImage(std::vector<uint8_t> & data);
  ImagePtr loadImage(Resource resource);
  TexturePtr load2dTextureFromPngData(std::vector<uint8_t> & data);
  TexturePtr load2dTexture(Resource resource);
  TexturePtr load2dTexture(Resource resource, uvec2 & outSize);
  TexturePtr loadCubemapTexture(Resource firstResource);
}
