#ifndef _MODEL_HPP_
#define _MODEL_HPP_
#include <stdio.h>
#include "../../glm/vec4.hpp"
#include "../../glm/vec3.hpp"
#include "../../glm/mat4x4.hpp"
#include "../../glm/gtc/matrix_transform.hpp"
#include "../framework/gl_framework.hpp"

extern GLFWwindow* window;

/**
 * @brief      Class for storing vertex attributes.
 */
class Vertex {
public:
	Vertex();
	Vertex(glm::vec3 p, glm::vec3 c);
	glm::vec3 position;
	glm::vec3 color;
};


/**
 * @brief      Class for storing model data.
 */
class Model {
public:
	std::vector<Vertex> vertex_list;
	GLuint vbo;
	/**
	 * @brief      load model from a file
	 *
	 * @return     true if successfully loaded
	 */
	bool load(char* filename);

	/**
	 * @brief      assign model buffer to vbo and accordingly adjust how the
	 *             values will be interpreted by vertex shader
	 */
	void assignBuffer();

	void draw(GLuint vPosition, GLuint vColor, GLuint uModelViewMatrix, GLenum mode, glm::mat4 modelview_matrix);
};


/**
 * @brief      Class for storing model attributes in the world
 */
class WorldModel {
public:
	Model m;

	glm::vec3 scale_vec;
	glm::vec3 rotation_vec;
	glm::vec3 position_vec;

	glm::mat4 transformation_mtx;
	/**
	 * @brief      Use position, scale and rotation to get modelling
	 *             transformation.
	 *
	 * @return     The modelling transformation matrix in homogemous space.
	 */
	void calc_modelling_transformation();
};

/**
 * @brief      Class for camera data.
 */
class WorldCamera {
public:
	glm::vec3 eye_position;
	glm::vec3 look_at;
	glm::vec3 up;

	float L, R, T, B, N, F;

	GLuint vao;
	GLuint vPosition, vColor, uModelViewMatrix;
	Model frustum, eye;
	
	void create_frustum();
	void draw(glm::mat4 ortho_projection_matrix);
};

/**
 * @brief      Class for storing scene data.
 */
class Scene {
public:

	std::vector<WorldModel> model_list;
	WorldCamera cam;
	
	GLuint vao = 0;	
	GLuint vPosition, vColor, uModelViewMatrix;

	/**
	 * @brief      load scene from a file
	 *
	 * @return     true if successfully loaded
	 */
	bool load();

	void draw(glm::mat4 ortho_projection_matrix);

	void toVCS();

	void toCCS();

	void toNDCS();

	void toDCS();
};

#include "../main.hpp"

#endif
