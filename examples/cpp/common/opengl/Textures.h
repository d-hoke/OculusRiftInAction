#pragma once

#ifdef HAVE_OPENCV
#include <opencv2/opencv.hpp>
#else
#include <oglplus/images/png.hpp>
#endif

namespace oria {
  typedef std::shared_ptr<oglplus::Texture> TexturePtr;
  typedef std::shared_ptr<oglplus::images::Image> ImagePtr;

  inline ImagePtr loadImage(Resource resource) {
    using namespace oglplus;
#ifdef HAVE_OPENCV
    std::vector<uint8_t> data = Platform::getResourceByteVector(resource);
    cv::Mat image = cv::imdecode(data, CV_LOAD_IMAGE_COLOR);
    cv::flip(image, image, 0);
    return ImagePtr(new images::Image(image.cols, image.rows, 1, 3, image.data,
      PixelDataFormat::BGR, PixelDataInternalFormat::RGBA8));
#else
    std::stringstream stream = Platform::getResourceStream(resource);
    return ImagePtr(new images::PNGImage(stream));
#endif
  }

  typedef std::map<Resource, TexturePtr> TextureMap;
  typedef TextureMap::iterator TextureMapItr;

  inline TextureMap & getTextureMap() {
    static TextureMap map;
    static bool registeredShutdown = false;
    if (!registeredShutdown) {
      addShudownHook([&]{
        map.clear();
      });
      registeredShutdown = true;
    }
    
    return map;
  }

  template <typename T, typename F> 
  T loadOrPopulate(std::map<Resource, T> & map, Resource resource, F loader) {
    auto itr = map.find(resource);
    if (map.end() == itr) {
      T built = loader();
      if (!built) {
        FAIL("Unable to construct object");
      }
      map[resource] = built;
      return built;
    }
    return itr->second;
  }

  inline TexturePtr load2dTexture(Resource resource) {
    return loadOrPopulate(getTextureMap(), resource, [&]{
      using namespace oglplus;
      TexturePtr texture(new Texture());
      Context::Bound(TextureTarget::_2D, *texture)
        .MagFilter(TextureMagFilter::Linear)
        .MinFilter(TextureMinFilter::Linear);
      Texture::Image2D(
        TextureTarget::_2D,
        *loadImage(resource)
      );
      return texture;
    });
  }

  inline TexturePtr loadCubemapTexture(Resource firstResource) {
    return loadOrPopulate(getTextureMap(), firstResource, [&]{
      using namespace oglplus;
      TexturePtr texture(new Texture());
      Context::Bound(TextureTarget::CubeMap, *texture)
        .MagFilter(TextureMagFilter::Linear)
        .MinFilter(TextureMinFilter::Linear)
        .WrapS(TextureWrap::ClampToEdge)
        .WrapT(TextureWrap::ClampToEdge)
        .WrapR(TextureWrap::ClampToEdge);

      static int RESOURCE_ORDER[] = {
        1, 0, 3, 2, 5, 4
      };

      glm::uvec2 size;
      for (int i = 0; i < 6; ++i) {
        Resource image = static_cast<Resource>(firstResource + i);
        int cubeMapFace = RESOURCE_ORDER[i];
        Texture::Image2D(
          Texture::CubeMapFace(cubeMapFace),
          *loadImage(image)
        );
      }
      return texture;
    });
  }


}
