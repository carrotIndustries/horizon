#include "face.hpp"
#include "board/board_layers.hpp"
#include "canvas/gl_util.hpp"
#include "canvas3d.hpp"
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace horizon {
FaceRenderer::FaceRenderer(Canvas3D *c) : ca(c)
{
}

static GLuint create_vao(GLuint program, GLuint &vbo_out, GLuint &ebo_out, GLuint &vbo_instance_out)
{
    GLuint position_index = glGetAttribLocation(program, "position");
    GLuint color_index = glGetAttribLocation(program, "color");
    GLuint offset_index = glGetAttribLocation(program, "offset");
    GLuint angle_index = glGetAttribLocation(program, "angle");
    GLuint flip_index = glGetAttribLocation(program, "flip");

    GLuint model_offset_index = glGetAttribLocation(program, "model_offset");
    GLuint model_rotation_index = glGetAttribLocation(program, "model_rotation");

    GLuint vao, buffer, ebuffer, ibuffer;

    /* we need to create a VAO to store the other buffers */
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    /* this is the VBO that holds the vertex data */
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    glGenBuffers(1, &ebuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebuffer);

    Canvas3D::FaceVertex vertices[] = {
            //   Position
            {-1, -1, 5, 255, 255, 0},
            {-1, 1, 5, 255, 0, 0},
            {1, 1, 5, 255, 0, 0},
            {1, -1, 5, 255, 0, 0},
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    uint32_t elements[] = {0, 1, 2, 2, 3, 0};
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    /* enable and set the position attribute */
    glEnableVertexAttribArray(position_index);
    glVertexAttribPointer(position_index, 3, GL_FLOAT, GL_FALSE, sizeof(Canvas3D::FaceVertex), 0);
    glEnableVertexAttribArray(color_index);
    glVertexAttribPointer(color_index, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Canvas3D::FaceVertex),
                          (void *)offsetof(Canvas3D::FaceVertex, r));

    glGenBuffers(1, &ibuffer);
    glBindBuffer(GL_ARRAY_BUFFER, ibuffer);

    Canvas3D::ModelTransform ivertices[] = {//   Position
                                            {0, 0, 0, 0},
                                            {20, 20, 32768, 0}};
    glBufferData(GL_ARRAY_BUFFER, sizeof(ivertices), ivertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(offset_index);
    glVertexAttribPointer(offset_index, 2, GL_FLOAT, GL_FALSE, sizeof(Canvas3D::ModelTransform), 0);
    glVertexAttribDivisor(offset_index, 1);

    glEnableVertexAttribArray(angle_index);
    glVertexAttribPointer(angle_index, 1, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(Canvas3D::ModelTransform),
                          (void *)offsetof(Canvas3D::ModelTransform, angle));
    glVertexAttribDivisor(angle_index, 1);

    glEnableVertexAttribArray(flip_index);
    glVertexAttribIPointer(flip_index, 1, GL_UNSIGNED_SHORT, sizeof(Canvas3D::ModelTransform),
                           (void *)offsetof(Canvas3D::ModelTransform, flip));
    glVertexAttribDivisor(flip_index, 1);

    glEnableVertexAttribArray(model_offset_index);
    glVertexAttribPointer(model_offset_index, 3, GL_FLOAT, GL_FALSE, sizeof(Canvas3D::ModelTransform),
                          (void *)offsetof(Canvas3D::ModelTransform, model_x));
    glVertexAttribDivisor(model_offset_index, 1);

    glEnableVertexAttribArray(model_rotation_index);
    glVertexAttribPointer(model_rotation_index, 3, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(Canvas3D::ModelTransform),
                          (void *)offsetof(Canvas3D::ModelTransform, model_roll));
    glVertexAttribDivisor(model_rotation_index, 1);

    /* enable and set the color attribute */
    /* reset the state; we will re-enable the VAO when needed */
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // glDeleteBuffers (1, &buffer);
    vbo_out = buffer;
    ebo_out = ebuffer;
    vbo_instance_out = ibuffer;

    return vao;
}

void FaceRenderer::realize()
{
    program = gl_create_program_from_resource("/net/carrotIndustries/horizon/src/canvas/face-vertex.glsl",
                                              "/net/carrotIndustries/horizon/src/canvas/face-fragment.glsl",
                                              "/net/carrotIndustries/horizon/src/canvas/"
                                              "face-geometry.glsl");
    vao = create_vao(program, vbo, ebo, vbo_instance);

    GET_LOC(this, view);
    GET_LOC(this, proj);
    GET_LOC(this, cam_normal);
    GET_LOC(this, z_top);
    GET_LOC(this, z_bottom);
}

void FaceRenderer::push()
{
    if (ca->models_loading_mutex.try_lock()) {
        auto n_vertices = ca->face_vertex_buffer.size();
        auto n_idx = ca->face_index_buffer.size();

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Canvas3D::FaceVertex) * n_vertices, ca->face_vertex_buffer.data(),
                     GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * n_idx, ca->face_index_buffer.data(),
                     GL_STATIC_DRAW);
        ca->models_loading_mutex.unlock();
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo_instance);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Canvas3D::ModelTransform) * ca->package_transforms.size(),
                 ca->package_transforms.data(), GL_STATIC_DRAW);
}

void FaceRenderer::render()
{
    if (ca->models_loading_mutex.try_lock()) {
        glUseProgram(program);
        glBindVertexArray(vao);

        glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(ca->viewmat));
        glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(ca->projmat));
        glUniform3fv(cam_normal_loc, 1, glm::value_ptr(ca->cam_normal));
        glUniform1f(z_top_loc,
                    ca->get_layer_offset(BoardLayers::TOP_COPPER) + ca->layers.at(BoardLayers::TOP_COPPER).thickness);
        glUniform1f(z_bottom_loc, ca->get_layer_offset(BoardLayers::BOTTOM_COPPER));

        for (const auto &it : ca->models) {
            if (ca->package_transform_idxs.count(it.first)) {
                auto idxs = ca->package_transform_idxs.at(it.first);
                glDrawElementsInstancedBaseInstance(GL_TRIANGLES, it.second.second, GL_UNSIGNED_INT,
                                                    (void *)(it.second.first * sizeof(int)), idxs.second, idxs.first);
            }
        }
        ca->models_loading_mutex.unlock();
    }
}
} // namespace horizon
