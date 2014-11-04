#include "Common.h"


/**
*  @file oglplus/shapes/obj_mesh.hpp
*  @brief Loader of meshes stored in .obj file format
*
*  @author Matus Chochlik
*
*  Copyright 2010-2014 Matus Chochlik. Distributed under the Boost
*  Software License, Version 1.0. (See accompanying file
*  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/

#include <oglplus/face_mode.hpp>
#include <oglplus/shapes/draw.hpp>
#include <oglplus/shapes/vert_attr_info.hpp>
#include <oglplus/detail/any_iter.hpp>
#include <oglplus/math/sphere.hpp>
#include <oglplus/shapes/blender_mesh.hpp>

#include <oglplus/shapes/obj_mesh.hpp>

#include <openctmpp.h>

#include <array>
#include <vector>
#include <iostream>
#include <cctype>
#include <string>

namespace oglplus {
  namespace shapes {

    /// Class providing attributes and instructions for drawing of mesh loaded from obj
    class CtmMesh
      : public DrawingInstructionWriter
      , public DrawMode
    {
    public:
      typedef std::vector<GLuint> IndexArray;

    private:
      struct _loading_options
      {
        bool load_normals;
        bool load_tangents;
        bool load_bitangents;
        bool load_texcoords;
        bool load_materials;

        _loading_options(bool load_all = true)
        {
          All(load_all);
        }

        _loading_options& All(bool load_all = true)
        {
          load_normals = load_all;
          load_tangents = load_all;
          load_bitangents = load_all;
          load_texcoords = load_all;
          load_materials = load_all;
          return *this;
        }

        _loading_options& Nothing(void)
        {
          return All(false);
        }

        _loading_options& Normals(bool load = true)
        {
          load_normals = load;
          return *this;
        }

        _loading_options& Tangents(bool load = true)
        {
          load_tangents = load;
          return *this;
        }

        _loading_options& Bitangents(bool load = true)
        {
          load_bitangents = load;
          return *this;
        }

        _loading_options& TexCoords(bool load = true)
        {
          load_texcoords = load;
          return *this;
        }

        _loading_options& Materials(bool load = true)
        {
          load_materials = load;
          return *this;
        }
      };

      /// The type of the index container returned by Indices()
      // vertex positions
      std::vector<float> _pos_data;
      // vertex normals
      std::vector<float> _nml_data;
      // vertex tex coords
      std::vector<float> _tex_data;
      IndexArray _idx_data;
      unsigned int _prim_count;


      struct _vert_indices
      {
        GLuint _pos;
        GLuint _nml;
        GLuint _tex;
        GLuint _mtl;

        _vert_indices(void)
          : _pos(0)
          , _nml(0)
          , _tex(0)
          , _mtl(0)
        { }
      };

      bool _load_index(
        GLuint& value,
        GLuint count,
        std::string::const_iterator& i,
        std::string::const_iterator& e
        );

      void _call_load_meshes(
        Resource resource,
        aux::AnyInputIter<const char*> names_begin,
        aux::AnyInputIter<const char*> names_end,
        _loading_options opts
        ) {

        CTMimporter importer;
        importer.LoadData(Platform::getResourceData(resource));
        int vertexCount = importer.GetInteger(CTM_VERTEX_COUNT);
        {
          const float * ctmData = importer.GetFloatArray(CTM_VERTICES);
          _pos_data = std::vector<float>(ctmData, ctmData + (vertexCount * 3));
        }

        if (opts.load_texcoords && importer.GetInteger(CTM_UV_MAP_COUNT)) {
          const float * ctmData = importer.GetFloatArray(CTM_UV_MAP_1);
          _tex_data = std::vector<float>(ctmData, ctmData + (vertexCount * 2));
        }

        if (opts.load_normals && importer.GetInteger(CTM_HAS_NORMALS)) {
          const float * ctmData = importer.GetFloatArray(CTM_NORMALS);
          _nml_data = std::vector<float>(ctmData, ctmData + (vertexCount * 3));
        }

        {
          _prim_count = importer.GetInteger(CTM_TRIANGLE_COUNT);
          int indexCount = 3 * _prim_count;
          const CTMuint * ctmIntData = importer.GetIntegerArray(CTM_INDICES);
          _idx_data = IndexArray(ctmIntData, ctmIntData + indexCount);
        }
      }

    public:
      typedef _loading_options LoadingOptions;

      CtmMesh(
        Resource resource,
        LoadingOptions opts = LoadingOptions()
        )
      {
        const char** p = nullptr;
        _call_load_meshes(resource, p, p, opts);
      }

      /// Returns the winding direction of faces
      FaceOrientation FaceWinding(void) const
      {
        return FaceOrientation::CCW;
      }

      typedef GLuint(CtmMesh::*VertexAttribFunc)(std::vector<GLfloat>&) const;

      /// Makes the vertex positions and returns the number of values per vertex
      template <typename T>
      GLuint Positions(std::vector<T>& dest) const
      {
        dest.clear();
        dest.insert(dest.begin(), _pos_data.begin(), _pos_data.end());
        return 3;
      }

      /// Makes the vertex normals and returns the number of values per vertex
      template <typename T>
      GLuint Normals(std::vector<T>& dest) const
      {
        dest.clear();
        dest.insert(dest.begin(), _nml_data.begin(), _nml_data.end());
        return 3;
      }

      /// Makes the vertex tangents and returns the number of values per vertex
      template <typename T>
      GLuint Tangents(std::vector<T>& dest) const
      {
        dest.clear();
        return 3;
      }

      /// Makes the vertex bi-tangents and returns the number of values per vertex
      template <typename T>
      GLuint Bitangents(std::vector<T>& dest) const
      {
        dest.clear();
        return 3;
      }

      /// Makes the texture coordinates returns the number of values per vertex
      template <typename T>
      GLuint TexCoordinates(std::vector<T>& dest) const
      {
        dest.clear();
        dest.insert(dest.begin(), _tex_data.begin(), _tex_data.end());
        return 2;
      }
      
      typedef VertexAttribsInfo<
        CtmMesh,
        std::tuple<
        VertexPositionsTag,
        VertexNormalsTag,
        VertexTangentsTag,
        VertexBitangentsTag,
        VertexTexCoordinatesTag
        >
      > VertexAttribs;

      Spheref MakeBoundingSphere(void) const {
          GLfloat min_x = _pos_data[3], max_x = _pos_data[3];
          GLfloat min_y = _pos_data[4], max_y = _pos_data[4];
          GLfloat min_z = _pos_data[5], max_z = _pos_data[5];
          for (std::size_t v = 0, vn = _pos_data.size() / 3; v != vn; ++v)
          {
            GLfloat x = _pos_data[v * 3 + 0];
            GLfloat y = _pos_data[v * 3 + 1];
            GLfloat z = _pos_data[v * 3 + 2];

            if (min_x > x) min_x = x;
            if (min_y > y) min_y = y;
            if (min_z > z) min_z = z;
            if (max_x < x) max_x = x;
            if (max_y < y) max_y = y;
            if (max_z < z) max_z = z;
          }

          Vec3f c(
            (min_x + max_x) * 0.5f,
            (min_y + max_y) * 0.5f,
            (min_z + max_z) * 0.5f
            );

          return Spheref(
            c.x(), c.y(), c.z(),
            Distance(c, Vec3f(min_x, min_y, min_z))
            );
      }

      /// Queries the bounding sphere coordinates and dimensions
      template <typename T>
      void BoundingSphere(oglplus::Sphere<T>& bounding_sphere) const
      {
        bounding_sphere = oglplus::Sphere<T>(MakeBoundingSphere());
      }


      /// Returns element indices that are used with the drawing instructions
      const IndexArray & Indices(Default = Default()) const
      {
        return _idx_data;
      }

      /// Returns the instructions for rendering of faces
      DrawingInstructions Instructions(PrimitiveType primitive) const {
        DrawingInstructions instr = this->MakeInstructions();
        DrawOperation operation;
        operation.method = DrawOperation::Method::DrawElements;
        operation.mode = primitive;
        operation.first = 0;
        operation.count = _prim_count * 3;
        operation.restart_index = DrawOperation::NoRestartIndex();
        operation.phase = 0;
        this->AddInstruction(instr, operation);
        return std::move(instr);
      }

      /// Returns the instructions for rendering of faces
      DrawingInstructions Instructions(Default = Default()) const
      {
        return Instructions(PrimitiveType::Triangles);
      }
    };
  } // shapes
} // oglplus

namespace oglplus {
  Vec4f fromGlm(const vec4 & v) {
    return Vec4f(v.x, v.y, v.z, v.w);
  }
}

namespace oria {

#define SET_UNIFORM_BY_NAME(p, u, t, v) \
  oglplus::Uniform<t>(p, u).Set(v)

  inline void bindLights(ProgramPtr & program) {
    using namespace oglplus;
    Lights & lights = Stacks::lights();
    SET_UNIFORM_BY_NAME(*program, "Ambient", vec4, lights.ambient);
    SET_UNIFORM_BY_NAME(*program, "LightCount", int, lights.lightPositions.size());
    std::vector<Vec4f> lps;
    std::vector<Vec4f> lcs;
    for (int i = 0; i < lights.lightPositions.size(); ++i) {
      lps.push_back(fromGlm(lights.lightPositions.at(i)));
      lcs.push_back(fromGlm(lights.lightColors.at(i)));
    }
    oglplus::Uniform<oglplus::Vec4f>(*program, "LightColor[0]").Set(lcs);
    oglplus::Uniform<oglplus::Vec4f>(*program, "LightPosition[0]").Set(lps);
    //oglplus::Uniform<vec4>(*program, "LightPosition[0]").Set(lights.lightPositions);
    //for (size_t i = 0; i < lights.lightPositions.size(); ++i) {
    //  SET_UNIFORM_BY_NAME(program, "LightPosition" + i, vec3, lights.lightPositions[i]); 
    //  SET_UNIFORM_BY_NAME(program, LightColor + i, vec4, lights.lightColors[i]); 
    //}
  }

#define SET_LIGHTS(program, lights) \


  inline void renderManikin() {
    using namespace oglplus;

    static ProgramPtr program;
    static ShapeWrapperPtr shape;
    if (!program) {
      oria::addShudownHook([&]{
        program.reset();
        shape.reset();
      });
      program = loadProgram(Resource::SHADERS_LIT_VS, Resource::SHADERS_LITCOLORED_FS);
      shape = ShapeWrapperPtr(new shapes::ShapeWrapper(List("Position")("Normal").Get(), shapes::CtmMesh(Resource::MESHES_MANIKIN_CTM), *program));;
    }

    renderGeometry(shape, program, { [&]{
      bindLights(program);
    } });
  }
}
struct EyeArgs {
  glm::mat4               projection;
  FramebufferWrapperPtr   framebuffer;
};

class HelloRift : public GlfwApp {
protected:
  ovrHmd                  hmd{ 0 };
  bool                    directMode{ false };
  bool                    debugDevice{ false };
  EyeArgs                 perEyeArgs[2];
  ovrTexture              textures[2];
  ovrVector3f             eyeOffsets[2];

  float                   eyeHeight{ OVR_DEFAULT_EYE_HEIGHT };
  float                   ipd{ OVR_DEFAULT_IPD };
  glm::mat4               player;

public:
  HelloRift() {
    ovr_Initialize();
    hmd = ovrHmd_Create(0);
    if (nullptr == hmd) {
      debugDevice = true;
      hmd = ovrHmd_CreateDebug(ovrHmd_DK2);
    }

    ON_WINDOWS([&]{
      directMode = (0 == (ovrHmd_GetEnabledCaps(hmd) & ovrHmdCap_ExtendDesktop));
    });
    ovrHmd_ConfigureTracking(hmd,
      ovrTrackingCap_Orientation |
      ovrTrackingCap_Position, 0);
    windowPosition = glm::ivec2(hmd->WindowsPos.x, hmd->WindowsPos.y);
    windowSize = glm::uvec2(hmd->Resolution.w, hmd->Resolution.h);
    if (debugDevice) {
//      windowPosition.x = 0;
//      windowPosition.y = -1080;
    }
    ON_LINUX([&]{
      std::swap(windowSize.x, windowSize.y);
    });
    resetPosition();
  }

  ~HelloRift() {
    ovrHmd_Destroy(hmd);
    hmd = 0;
  }

  virtual void resetPosition() {
    //eyeHeight = 0;
    static const glm::vec3 EYE = glm::vec3(0, eyeHeight, ipd * 5.0f);
    static const glm::vec3 LOOKAT = glm::vec3(0, eyeHeight, 0);
    player = glm::inverse(glm::lookAt(EYE, LOOKAT, Vectors::UP));
    ovrHmd_RecenterPose(hmd);
  }

  virtual void finishFrame() {
    /*
     * The parent class calls glfwSwapBuffers in finishFrame,
     * but with the Oculus SDK, the SDK it responsible for buffer
     * swapping, so we have to override the method and ensure it
     * does nothing, otherwise the dual buffer swaps will
     * cause a constant flickering of the display.
     */
  }

  virtual void createRenderingTarget() {
    bool directHmdMode = false;

    // The ovrHmdCap_ExtendDesktop only reliably reports on Windows currently
    ON_WINDOWS([&]{
      directHmdMode = (0 == (ovrHmdCap_ExtendDesktop & ovrHmd_GetEnabledCaps(hmd)));
    });

    // On linux it's recommended to leave the screen in it's default portrait orientation.
    // The SDK currently allows no mechanism to test if this is the case.  I could query
    // GLFW for the current resolution of the Rift, but that sounds too much like actual
    // work.
    ON_LINUX([&]{
      std::swap(windowSize.x, windowSize.y);
    });

    if (directHmdMode && !debugDevice) {
      // In direct mode, try to put the output window on a secondary screen
      // (for easier debugging, assuming your dev environment is on the primary)
      window = glfw::createSecondaryScreenWindow(windowSize);
    } else {
      // If we're creating a desktop window, we should strip off any window decorations
      // which might change the location of the rendered contents relative to the lenses.
      //
      // Another alternative would be to create the window in fullscreen mode, on
      // platforms that support such a thing.
      glfwWindowHint(GLFW_DECORATED, 0);
      window = glfw::createWindow(windowSize, windowPosition);
    }

    // If we're in direct mode, attach to the window
    if (directHmdMode) {
      void * nativeWindowHandle = nullptr;
      ON_WINDOWS([&]{ nativeWindowHandle = (void*)glfwGetWin32Window(window); });
      ON_LINUX([&]{ nativeWindowHandle = (void*)glfwGetX11Window(window); });
      ON_MAC([&]{ nativeWindowHandle = (void*)glfwGetCocoaWindow(window); });
      if (nullptr != nativeWindowHandle) {
        ovrHmd_AttachToWindow(hmd, nativeWindowHandle, nullptr, nullptr);
      }
    }
  }

  void initGl() {
    GlfwApp::initGl();
    ovrFovPort eyeFovPorts[2];
    for_each_eye([&](ovrEyeType eye){
      EyeArgs & eyeArgs = perEyeArgs[eye];
      ovrTextureHeader & eyeTextureHeader = textures[eye].Header;
      eyeFovPorts[eye] = hmd->DefaultEyeFov[eye];
      eyeTextureHeader.TextureSize = ovrHmd_GetFovTextureSize(hmd, eye, hmd->DefaultEyeFov[eye], 1.0f);
      eyeTextureHeader.RenderViewport.Size = eyeTextureHeader.TextureSize;
      eyeTextureHeader.RenderViewport.Pos.x = 0;
      eyeTextureHeader.RenderViewport.Pos.y = 0;
      eyeTextureHeader.API = ovrRenderAPI_OpenGL;

      eyeArgs.framebuffer = FramebufferWrapperPtr(new FramebufferWrapper());
      eyeArgs.framebuffer->init(ovr::toGlm(eyeTextureHeader.TextureSize));
      ((ovrGLTexture&)textures[eye]).OGL.TexId = oglplus::GetName(eyeArgs.framebuffer->color);
    });

    ovrGLConfig cfg;
    memset(&cfg, 0, sizeof(ovrGLConfig));
    cfg.OGL.Header.API = ovrRenderAPI_OpenGL;
    cfg.OGL.Header.RTSize = ovr::fromGlm(windowSize);
    // FIXME Doesn't work as expected
    //if (0 == (ovrHmd_GetEnabledCaps(hmd) & ovrHmdCap_ExtendDesktop)) {
    //  cfg.OGL.Header.RTSize.w /= 4;
    //  cfg.OGL.Header.RTSize.h /= 4;
    //}

    cfg.OGL.Header.Multisample = 1;

    ON_WINDOWS([&]{
      cfg.OGL.Window = 0;
    });

    ON_LINUX([&]{
      cfg.OGL.Disp = glfwGetX11Display();
      cfg.OGL.Win = glfwGetX11Window(window);
    });

    int distortionCaps =
      ovrDistortionCap_TimeWarp |
      ovrDistortionCap_Chromatic |
      ovrDistortionCap_Vignette;

    ON_LINUX([&]{
      distortionCaps |= ovrDistortionCap_LinuxDevFullscreen;
    });

    ovrEyeRenderDesc              eyeRenderDescs[2];
    int configResult = ovrHmd_ConfigureRendering(hmd, &cfg.Config,
      distortionCaps, eyeFovPorts, eyeRenderDescs);

    for_each_eye([&](ovrEyeType eye){
      EyeArgs & eyeArgs = perEyeArgs[eye];
      eyeOffsets[eye] = eyeRenderDescs[eye].HmdToEyeViewOffset;
      eyeArgs.projection = ovr::toGlm(
        ovrMatrix4f_Projection(eyeFovPorts[eye], 0.01f, 1000.0f, true));
    });
  }

  void onKey(int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
      static ovrHSWDisplayState hswDisplayState;
      ovrHmd_GetHSWDisplayState(hmd, &hswDisplayState);
      if (hswDisplayState.Displayed) {
        ovrHmd_DismissHSWDisplay(hmd);
        return;
      }
    }

    //if (CameraControl::instance().onKey(key, scancode, action, mods)) {
    //  return;
    //}

    if (GLFW_PRESS != action) {
      GlfwApp::onKey(key, scancode, action, mods);
      return;
    }

    int caps = ovrHmd_GetEnabledCaps(hmd);
    switch (key) {
    case GLFW_KEY_V:
      if (caps & ovrHmdCap_NoVSync) {
        ovrHmd_SetEnabledCaps(hmd, caps & ~ovrHmdCap_NoVSync);
      } else {
        ovrHmd_SetEnabledCaps(hmd, caps | ovrHmdCap_NoVSync);
      }
      break;

    case GLFW_KEY_P:
      if (caps & ovrHmdCap_LowPersistence) {
        ovrHmd_SetEnabledCaps(hmd, caps & ~ovrHmdCap_LowPersistence);
      }
      else {
        ovrHmd_SetEnabledCaps(hmd, caps | ovrHmdCap_LowPersistence);
      }
      break;

    case GLFW_KEY_R:
      resetPosition();
      break;

    default:
      GlfwApp::onKey(key, scancode, action, mods);
      break;
    }
  }

  virtual void update() {
    //CameraControl::instance().applyInteraction(player);
    Stacks::modelview().top() = glm::inverse(player);
  }

  void draw() {
    static int frameIndex = 0;
    static ovrPosef eyePoses[2];
    ++frameIndex;
    ovrHmd_GetEyePoses(hmd, frameIndex, eyeOffsets, eyePoses, nullptr);

    ovrHmd_BeginFrame(hmd, frameIndex);
    glEnable(GL_DEPTH_TEST);

    for( int i = 0; i < 2; ++i) {
      ovrEyeType eye = hmd->EyeRenderOrder[i];
      EyeArgs & eyeArgs = perEyeArgs[eye];

      const ovrRecti & vp = textures[eye].Header.RenderViewport;
      eyeArgs.framebuffer->fbo.Bind(oglplus::Framebuffer::Target::Draw);
      oglplus::Context::Viewport(vp.Pos.x, vp.Pos.y, vp.Size.w, vp.Size.h);
      
      Stacks::projection().top() = eyeArgs.projection;
      MatrixStack & mv = Stacks::modelview();
      mv.withPush([&]{
        // Apply the per-eye offset & the head pose
        mv.top() = glm::inverse(ovr::toGlm(eyePoses[eye])) * mv.top();
        renderScene();
      });
    };
    oglplus::DefaultFramebuffer().Bind(oglplus::Framebuffer::Target::Draw);

    ovrHmd_EndFrame(hmd, eyePoses, textures);
  }

  virtual void renderScene() {
    oglplus::Context::Clear().DepthBuffer().ColorBuffer();
    oria::renderSkybox(Resource::IMAGES_SKY_CITY_XNEG_PNG);
    oria::renderFloor();

    // Scale the size of the cube to the distance between the eyes
    MatrixStack & mv = Stacks::modelview();

    mv.withPush([&]{
      mv.translate(glm::vec3(0, eyeHeight, 0)).scale(glm::vec3(ipd));
      oria::renderColorCube();
    });

    mv.withPush([&]{
      mv.translate(glm::vec3(0, 0, ipd * -5.0));

      oglplus::Context::Disable(oglplus::Capability::CullFace);
      oria::renderManikin();
    });

    //MatrixStack & mv = Stacks::modelview();
    //mv.with_push([&]{
    //  mv.translate(glm::vec3(0, 0, ipd * -5));
    //  GlUtils::renderManikin();
    //});
  }
};

RUN_APP(HelloRift);

