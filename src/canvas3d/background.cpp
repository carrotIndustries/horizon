#include "background.hpp"
#include "canvas3d.hpp"
#include "canvas/gl_util.hpp"
#include <cmath>

namespace horizon {
	BackgroundRenderer::BackgroundRenderer(Canvas3D *c): ca(c) {

	}

	static GLuint create_vao (GLuint program) {
		GLuint position_index = glGetAttribLocation (program, "position");
		GLuint vao, buffer;

		/* we need to create a VAO to store the other buffers */
		glGenVertexArrays (1, &vao);
		glBindVertexArray (vao);

		/* this is the VBO that holds the vertex data */
		glGenBuffers (1, &buffer);
		glBindBuffer (GL_ARRAY_BUFFER, buffer);

		float vertices[] = {
		//   Position
		   -1,  1,
		    1,  1,
		   -1, -1,
		    1, -1
		};
		glBufferData (GL_ARRAY_BUFFER, sizeof (vertices), vertices, GL_STATIC_DRAW);

		/* enable and set the position attribute */
		glEnableVertexAttribArray (position_index);
		glVertexAttribPointer (position_index, 2, GL_FLOAT, GL_FALSE,
							 sizeof(float)*2,
							 0);

		/* enable and set the color attribute */
		/* reset the state; we will re-enable the VAO when needed */
		glBindBuffer (GL_ARRAY_BUFFER, 0);
		glBindVertexArray (0);

		//glDeleteBuffers (1, &buffer);

		return vao;
	}

	void BackgroundRenderer::realize() {
		program = gl_create_program_from_resource("/net/carrotIndustries/horizon/canvas3d/shaders/background-vertex.glsl", "/net/carrotIndustries/horizon/canvas3d/shaders/background-fragment.glsl", nullptr);
		vao = create_vao(program);

		GET_LOC(this, color_top);
		GET_LOC(this, color_bottom);
	}

	void BackgroundRenderer::render() {
		glUseProgram(program);
		glBindVertexArray (vao);

		glUniform3f(color_top_loc, ca->background_top_color.r, ca->background_top_color.g, ca->background_top_color.b);
		glUniform3f(color_bottom_loc, ca->background_bottom_color.r, ca->background_bottom_color.g, ca->background_bottom_color.b);

		glDrawArrays (GL_TRIANGLE_STRIP, 0, 4);
	}

}
