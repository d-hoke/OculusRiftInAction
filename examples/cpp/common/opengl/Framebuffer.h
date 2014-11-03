#pragma once

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

typedef std::shared_ptr<FramebufferWrapper> FramebufferWrapperPtr;
