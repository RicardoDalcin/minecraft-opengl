#include "world/Object.hpp"

Object::Object(std::string filename, float objectX, float objectZ)
{
  m_ObjectX = objectX;
  m_ObjectZ = objectZ;

  std::string warn;
  std::string err;

  bool ret = tinyobj::LoadObj(&m_Attributes, &m_Shapes, &m_Materials, &warn, &err, filename.c_str());

  if (!err.empty())
    fprintf(stderr, "\n%s\n", err.c_str());

  if (!ret)
    throw std::runtime_error("Erro ao carregar modelo.");

  printf("OK.\n");

  ComputeNormals();
  BuildVertices();
}

Object::~Object()
{
  // delete m_VAO;
  // delete m_VBO;
}

void Object::ComputeNormals()
{
  if (!m_Attributes.normals.empty())
    return;

  // Primeiro computamos as normais para todos os TRIÂNGULOS.
  // Segundo, computamos as normais dos VÉRTICES através do método proposto
  // por Gouraud, onde a normal de cada vértice vai ser a média das normais de
  // todas as faces que compartilham este vértice.

  size_t num_vertices = m_Attributes.vertices.size() / 3;

  std::vector<int> num_triangles_per_vertex(num_vertices, 0);
  std::vector<glm::vec4> vertex_normals(num_vertices, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

  for (size_t shape = 0; shape < m_Shapes.size(); ++shape)
  {
    size_t num_triangles = m_Shapes[shape].mesh.num_face_vertices.size();

    for (size_t triangle = 0; triangle < num_triangles; ++triangle)
    {
      assert(m_Shapes[shape].mesh.num_face_vertices[triangle] == 3);

      glm::vec4 vertices[3];
      for (size_t vertex = 0; vertex < 3; ++vertex)
      {
        tinyobj::index_t idx = m_Shapes[shape].mesh.indices[3 * triangle + vertex];
        const float vx = m_Attributes.vertices[3 * idx.vertex_index + 0];
        const float vy = m_Attributes.vertices[3 * idx.vertex_index + 1];
        const float vz = m_Attributes.vertices[3 * idx.vertex_index + 2];
        vertices[vertex] = glm::vec4(vx, vy, vz, 1.0);
      }

      const glm::vec4 a = vertices[0];
      const glm::vec4 b = vertices[1];
      const glm::vec4 c = vertices[2];

      const glm::vec4 n = Matrices::CrossProduct(b - a, c - a);

      for (size_t vertex = 0; vertex < 3; ++vertex)
      {
        tinyobj::index_t idx = m_Shapes[shape].mesh.indices[3 * triangle + vertex];
        num_triangles_per_vertex[idx.vertex_index] += 1;
        vertex_normals[idx.vertex_index] += n;
        m_Shapes[shape].mesh.indices[3 * triangle + vertex].normal_index = idx.vertex_index;
      }
    }
  }

  m_Attributes.normals.resize(3 * num_vertices);

  for (size_t i = 0; i < vertex_normals.size(); ++i)
  {
    glm::vec4 n = vertex_normals[i] / (float)num_triangles_per_vertex[i];
    n /= Matrices::Norm(n);
    m_Attributes.normals[3 * i + 0] = n.x;
    m_Attributes.normals[3 * i + 1] = n.y;
    m_Attributes.normals[3 * i + 2] = n.z;
  }
}

void Object::BuildVertices()
{
  glGenVertexArrays(1, &m_VertexArrayObjectId);
  glBindVertexArray(m_VertexArrayObjectId);

  std::vector<GLuint> indices;
  std::vector<float> model_coefficients;
  std::vector<float> normal_coefficients;
  std::vector<float> texture_coefficients;

  for (size_t shape = 0; shape < m_Shapes.size(); ++shape)
  {
    size_t first_index = indices.size();
    size_t num_triangles = m_Shapes[shape].mesh.num_face_vertices.size();

    const float minval = std::numeric_limits<float>::min();
    const float maxval = std::numeric_limits<float>::max();

    glm::vec3 bbox_min = glm::vec3(maxval, maxval, maxval);
    glm::vec3 bbox_max = glm::vec3(minval, minval, minval);

    for (size_t triangle = 0; triangle < num_triangles; ++triangle)
    {
      assert(m_Shapes[shape].mesh.num_face_vertices[triangle] == 3);

      for (size_t vertex = 0; vertex < 3; ++vertex)
      {
        tinyobj::index_t idx = m_Shapes[shape].mesh.indices[3 * triangle + vertex];

        indices.push_back(first_index + 3 * triangle + vertex);

        const float vx = m_Attributes.vertices[3 * idx.vertex_index + 0];
        const float vy = m_Attributes.vertices[3 * idx.vertex_index + 1];
        const float vz = m_Attributes.vertices[3 * idx.vertex_index + 2];
        // printf("tri %d vert %d = (%.2f, %.2f, %.2f)\n", (int)triangle, (int)vertex, vx, vy, vz);
        model_coefficients.push_back(vx);   // X
        model_coefficients.push_back(vy);   // Y
        model_coefficients.push_back(vz);   // Z
        model_coefficients.push_back(1.0f); // W

        bbox_min.x = std::min(bbox_min.x, vx);
        bbox_min.y = std::min(bbox_min.y, vy);
        bbox_min.z = std::min(bbox_min.z, vz);
        bbox_max.x = std::max(bbox_max.x, vx);
        bbox_max.y = std::max(bbox_max.y, vy);
        bbox_max.z = std::max(bbox_max.z, vz);

        // Inspecionando o código da tinyobjloader, o aluno Bernardo
        // Sulzbach (2017/1) apontou que a maneira correta de testar se
        // existem normais e coordenadas de textura no ObjModel é
        // comparando se o índice retornado é -1. Fazemos isso abaixo.

        if (idx.normal_index != -1)
        {
          const float nx = m_Attributes.normals[3 * idx.normal_index + 0];
          const float ny = m_Attributes.normals[3 * idx.normal_index + 1];
          const float nz = m_Attributes.normals[3 * idx.normal_index + 2];
          normal_coefficients.push_back(nx);   // X
          normal_coefficients.push_back(ny);   // Y
          normal_coefficients.push_back(nz);   // Z
          normal_coefficients.push_back(0.0f); // W
        }

        if (idx.texcoord_index != -1)
        {
          const float u = m_Attributes.texcoords[2 * idx.texcoord_index + 0];
          const float v = m_Attributes.texcoords[2 * idx.texcoord_index + 1];
          texture_coefficients.push_back(u);
          texture_coefficients.push_back(v);
        }
      }
    }

    size_t last_index = indices.size() - 1;

    m_FirstIndex = first_index;
    m_IndexCount = last_index - first_index + 1;
  }

  GLuint VBO_model_coefficients_id;
  glGenBuffers(1, &VBO_model_coefficients_id);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_model_coefficients_id);
  glBufferData(GL_ARRAY_BUFFER, model_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, model_coefficients.size() * sizeof(float), model_coefficients.data());
  GLuint location = 0;            // "(location = 0)" em "shader_vertex.glsl"
  GLint number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
  glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(location);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  if (!normal_coefficients.empty())
  {
    GLuint VBO_normal_coefficients_id;
    glGenBuffers(1, &VBO_normal_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_normal_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, normal_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, normal_coefficients.size() * sizeof(float), normal_coefficients.data());
    location = 1;             // "(location = 1)" em "shader_vertex.glsl"
    number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  if (!texture_coefficients.empty())
  {
    GLuint VBO_texture_coefficients_id;
    glGenBuffers(1, &VBO_texture_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_texture_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, texture_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, texture_coefficients.size() * sizeof(float), texture_coefficients.data());
    location = 2;             // "(location = 1)" em "shader_vertex.glsl"
    number_of_dimensions = 2; // vec2 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  GLuint indices_id;
  glGenBuffers(1, &indices_id);

  // "Ligamos" o buffer. Note que o tipo agora é GL_ELEMENT_ARRAY_BUFFER.
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(GLuint), indices.data());
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // XXX Errado!
  //

  // "Desligamos" o VAO, evitando assim que operações posteriores venham a
  // alterar o mesmo. Isso evita bugs.
  glBindVertexArray(0);
}

void Object::Draw(Shader *shader, glm::mat4 view, glm::mat4 projection)
{
  shader->Bind();

  shader->SetUniformMat4f("view", view);
  shader->SetUniformMat4f("projection", projection);

  glm::mat4 model = Matrices::MatrixTranslate(m_ObjectX, 30.0f, m_ObjectZ);

  shader->SetUniformMat4f("model", model);

  glBindVertexArray(m_VertexArrayObjectId);

  // Pedimos para a GPU rasterizar os vértices dos eixos XYZ
  // apontados pelo VAO como linhas. Veja a definição de
  // g_VirtualScene[""] dentro da função BuildTrianglesAndAddToVirtualScene(), e veja
  // a documentação da função glDrawElements() em
  // http://docs.gl/gl3/glDrawElements.
  glDrawElements(
      GL_TRIANGLES,
      m_IndexCount,
      GL_UNSIGNED_INT,
      (void *)(m_FirstIndex * sizeof(GLuint)));

  // "Desligamos" o VAO, evitando assim que operações posteriores venham a
  // alterar o mesmo. Isso evita bugs.
  glBindVertexArray(0);

  // // "Ligamos" o VAO. Note que agora não precisamos mais "ligar" os buffers
  // // (VBOs) pois eles já estão "ligados" ao VAO.
  // glBindVertexArray(m_VertexArrayObjectId);

  // // "Ligamos" o buffer de índices.
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VertexBufferObjectId);

  // // Desenhamos o objeto.
  // glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, 0);

  // // "Desligamos" o buffer de índices.
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // // "Desligamos" o VAO, evitando assim que operações posteriores venham a
  // // alterar o mesmo. Isso evita bugs.
  // glBindVertexArray(0);
}