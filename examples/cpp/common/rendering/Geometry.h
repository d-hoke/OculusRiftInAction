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

class Geometry {
public:
  typedef std::vector<vec4> VVec4;

  static const int VERTEX_ATTRIBUTE_SIZE = 4;
  static const int BYTES_PER_ATTRIBUTE = (sizeof(float) * VERTEX_ATTRIBUTE_SIZE);

  oglplus::Buffer vertexBuffer;
  oglplus::Buffer indexBuffer;
  oglplus::VertexArray vao;

  GLsizei elements;
  GLenum  elementType{ GL_TRIANGLES };

  Geometry() {
  }

  template <typename Function>
  Geometry(Function func, GLsizei count, GLenum elementType = GL_TRIANGLES) : elements(count), elementType(elementType) {
    func(vertexBuffer, indexBuffer, vao);
  }
  void bind() {
    vao.Bind();
  }

  void draw() {
    glDrawElements(elementType, elements, GL_UNSIGNED_INT, (void*)0);
  }

  void drawInstanced(int count) {
    glDrawElementsInstanced(elementType, elements, GL_UNSIGNED_INT, (void*)0, count);
  }

  void loadMesh(const Mesh & mesh);
};
