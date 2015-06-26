/************************************************************************************

 Authors     :   Bradley Austin Davis <bdavis@saintandreas.org>
 Copyright   :   Copyright Bradley Austin Davis. All Rights reserved.

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

#include "QtCommon.h"

#include <QDomDocument>
#include <QImage>

#ifdef HAVE_OPENCV
#include <opencv2/opencv.hpp>
#else
#include <oglplus/images/png.hpp>
#endif


  namespace qt {

    template<typename T>
    T toQtType(Resource res) {
      T result;
      size_t size = Resources::getResourceSize(res);
      result.resize(size);
      Resources::getResourceData(res, result.data());
      return result;
    }


    QSize sizeFromGlm(const vec2 & size) {
      return QSize(size.x, size.y);
    }

    QPointF pointFromGlm(const vec2 & pt) {
      return QPointF(pt.x, pt.y);
    }

    QByteArray toByteArray(Resource res) {
      return toQtType<QByteArray>(res);
    }

    QString toString(Resource res) {
      QByteArray data = toByteArray(res);
      return QString::fromUtf8(data.data(), data.size());
    }

    //QImage loadImageResource(Resource res) {
    //  QImage image;
    //  image.loadFromData(toByteArray(res));
    //  return image;
    //}
  }

QJsonValue path(const QJsonValue & parent, std::initializer_list<QVariant> elements) {
  QJsonValue current = parent;
  std::for_each(elements.begin(), elements.end(), [&](const QVariant & element) {
    if (current.isObject()) {
      QString path = element.toString();
      current = current.toObject().value(path);
    } else if (current.isArray()) {
      int offset = element.toInt();
      current = current.toArray().at(offset);
    } else {
      qWarning() << "Unable to continue";
      current = QJsonValue();
    }
  });
  return current;
}

typedef std::list<QString> List;
typedef std::map<QString, List> Map;
typedef std::pair<QString, List> Pair;

template <typename F>
void for_each_node(const QDomNodeList & list, F f) {
  for (int i = 0; i < list.size(); ++i) {
    f(list.at(i));
  }
}




static ImagePtr loadImageWithAlpha(const std::vector<uint8_t> & data, bool flip) {
  using namespace oglplus;
#ifdef HAVE_OPENCV
  cv::Mat image = cv::imdecode(data, cv::IMREAD_UNCHANGED);
  if (flip) {
    cv::flip(image, image, 0);
  }
  ImagePtr result(new images::Image(image.cols, image.rows, 1, 4, image.data,
    PixelDataFormat::BGRA, PixelDataInternalFormat::RGBA8));
  return result;
#else
  std::stringstream stream(std::string((const char*)&data[0], data.size()));
  return ImagePtr(new images::PNGImage(stream));
#endif
}

TexturePtr loadCursor(Resource res) {
  using namespace oglplus;
  TexturePtr texture(new Texture());
  Context::Bound(TextureTarget::_2D, *texture)
    .MagFilter(TextureMagFilter::Linear)
    .MinFilter(TextureMinFilter::Linear);

  ImagePtr image = loadImageWithAlpha(Platform::getResourceByteVector(res), true);
  // FIXME detect alignment properly, test on both OpenCV and LibPNG
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  Texture::Storage2D(TextureTarget::_2D, 1, PixelDataInternalFormat::RGBA8, image->Width() * 2, image->Height() * 2);
  {
    size_t size = image->Width() * 2 * image->Height() * 2 * 4;
    uint8_t * empty = new uint8_t[size]; memset(empty, 0, size);
    images::Image blank(image->Width() * 2, image->Height() * 2, 1, 4, empty);
    Texture::SubImage2D(TextureTarget::_2D, blank, 0, 0);
  }
  Texture::SubImage2D(TextureTarget::_2D, *image, image->Width(), 0);
  DefaultTexture().Bind(TextureTarget::_2D);
  return texture;
}

QSurfaceFormat getDesiredSurfaceFormat() {
    QSurfaceFormat format;
    format.setDepthBufferSize(16);
    format.setStencilBufferSize(8);
    format.setMajorVersion(4);
    format.setMinorVersion(3);
    format.setProfile(QSurfaceFormat::OpenGLContextProfile::CoreProfile);
#ifdef DEBUG
    format.setOption(QSurfaceFormat::DebugContext);
#endif
    return format;
}
