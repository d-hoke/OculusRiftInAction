#pragma once

#include <openctmpp.h>

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
