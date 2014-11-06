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

#pragma once

namespace oria {

  // A wrapper for constructing and using a
  struct FramebufferWrapper {
    oglplus::Framebuffer   fbo;
    oglplus::Texture       color;
    oglplus::Renderbuffer  depth;

    void init(const glm::uvec2 & size) {
      using namespace oglplus;
      Context::Bound(Texture::Target::_2D, color)
        .MinFilter(TextureMinFilter::Linear)
        .MagFilter(TextureMagFilter::Linear)
        .WrapS(TextureWrap::ClampToEdge)
        .WrapT(TextureWrap::ClampToEdge)
        .Image2D(
        0, PixelDataInternalFormat::RGBA8,
        size.x, size.y,
        0, PixelDataFormat::RGB, PixelDataType::UnsignedByte, nullptr
        );

      Context::Bound(Renderbuffer::Target::Renderbuffer, depth)
        .Storage(
            PixelDataInternalFormat::DepthComponent,
            size.x, size.y);

      Context::Bound(Framebuffer::Target::Draw, fbo)
        .AttachTexture(FramebufferAttachment::Color, color, 0)
        .AttachRenderbuffer(FramebufferAttachment::Depth, depth)
        .Complete();
    }
  };

}

typedef std::shared_ptr<oria::FramebufferWrapper> FramebufferWrapperPtr;
