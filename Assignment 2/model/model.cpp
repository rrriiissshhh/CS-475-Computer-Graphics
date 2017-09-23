#include "model.hpp"

GLFWwindow* window;

void printmat4(glm::mat4 Awv){
	printf("\n");
	printf("%f, %f, %f, %f \n", Awv[0][0], Awv[1][0], Awv[2][0], Awv[3][0]);
	printf("%f, %f, %f, %f \n", Awv[0][1], Awv[1][1], Awv[2][1], Awv[3][1]);
	printf("%f, %f, %f, %f \n", Awv[0][2], Awv[1][2], Awv[2][2], Awv[3][2]);
	printf("%f, %f, %f, %f \n", Awv[0][3], Awv[1][3], Awv[2][3], Awv[3][3]);
	printf("\n");
	return;
}

/**
 * @brief      Loads the model stored in model.raw by default in  X Y Z R G B format
 */
bool Model::load(char* filename) {
	FILE *fp_input = fopen(filename, "r" );
	if (fp_input ==  NULL) {
		printf("Error opening file %s\n", filename);
		return false;
	}


	vertex_list.resize(0);

	float vx, vy, vz, cx, cy, cz;
	while (fscanf (fp_input, "%f %f %f %f %f %f", &vx, &vy, &vz, &cx, &cy, &cz) != EOF) {
		Vertex v;
		v.position = glm::vec3(vx, vy, vz);
		v.color = glm::vec3(cx, cy, cz);
		vertex_list.push_back(v);
	}

	fclose(fp_input);
	return true;
}

/**
 * @brief      Assigns vertex list to the vertex buffer object and corresponding
 *             shader file attributes.
 *
 * @param      vPosition  The vertex_list positions
 * @param      vColor     The vertex_list colors
 */
void Model::assignBuffer(GLuint &vPosition, GLuint &vColor) {
	size_t size_points = vertex_list.size() * sizeof (glm::vec3);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData (GL_ARRAY_BUFFER, size_points * 2, &vertex_list[0], GL_STATIC_DRAW);
}

void WorldModel::calc_modelling_transformation() {
	glm::mat4 scaled_mtx = glm::scale(glm::mat4(1.0f), scale_vec);

	glm::mat4 rotation_mtx = glm::rotate( glm::mat4(1.0f), rotation_vec.x, glm::vec3(1.0f, 0.0f, 0.0f));
	rotation_mtx = glm::rotate( rotation_mtx, rotation_vec.y, glm::vec3(0.0f, 1.0f, 0.0f));
	rotation_mtx = glm::rotate( rotation_mtx, rotation_vec.z, glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 translation_mtx = glm::translate(glm::mat4(1.0f), position_vec);

	transformation_mtx = translation_mtx * rotation_mtx * scaled_mtx;
}

bool Scene::load() {
	FILE *fp_input = fopen("./binary_models/myscene.scn", "r" );
	if (fp_input ==  NULL) {
		printf("Error opening file ./binary_models/myscene.scn\n");
		return false;
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	uModelViewMatrix = glGetUniformLocation( shaderProgram, "uModelViewMatrix");

	vPosition = glGetAttribLocation( shaderProgram, "vPosition" );
	glEnableVertexAttribArray( vPosition );

	vColor = glGetAttribLocation( shaderProgram, "vColor" );
	glEnableVertexAttribArray( vColor );

	model_list.resize(0);

	for (int i = 0; i < 3; ++i) {
		char model_filename[30];
		fscanf (fp_input, "%s", model_filename);

		WorldModel worldModel;
		worldModel.m.load(model_filename);
		worldModel.m.assignBuffer(vPosition, vColor);

		float a, b, c;
		fscanf (fp_input, "%f %f %f", &a, &b, &c);
		worldModel.scale_vec = glm::vec3(a, b, c);

		fscanf (fp_input, "%f %f %f", &a, &b, &c);
		worldModel.rotation_vec = glm::vec3(a, b, c);

		fscanf (fp_input, "%f %f %f", &a, &b, &c);
		worldModel.position_vec = glm::vec3(a, b, c);

		worldModel.calc_modelling_transformation();
		model_list.push_back(worldModel);
	}

	float a, b, c;
	fscanf (fp_input, "%f %f %f", &a, &b, &c);
	cam.eye_position = glm::vec3(a, b, c);

	fscanf (fp_input, "%f %f %f", &a, &b, &c);
	cam.look_at = glm::vec3(a, b, c);

	fscanf (fp_input, "%f %f %f", &a, &b, &c);
	cam.up = glm::vec3(a, b, c);

	fscanf (fp_input, "%f %f %f %f", &cam.L, &cam.R, &cam.T, &cam.B);
	fscanf (fp_input, "%f %f", &cam.N, &cam.F);

	fclose(fp_input);
	return true;
}

void Scene::draw() {
	glBindVertexArray(vao);

	for (int i = 0; i < 3; ++i) {

		glBindBuffer(GL_ARRAY_BUFFER, model_list[i].m.vbo);
		glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0) );
		glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(sizeof(glm::vec3)) );

		modelview_matrix = model_list[i].transformation_mtx;
		glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));

		glDrawArrays(GL_TRIANGLES, 0, scene.model_list[i].m.vertex_list.size());
	}
}

void Scene::toVCS() {

	glm::vec3 n = -(cam.look_at - cam.up)/glm::length(cam.look_at - cam.up);
	printf("%f, %f, %f \n", n.x, n.y, n.z);
	glm::vec3 u = glm::cross(cam.up, n)/glm::length(glm::cross(cam.up, n));
	printf("%f, %f, %f \n", u.x, u.y, u.z);
	glm::vec3 v = glm::cross(n,u);

	glm::vec4 row1 = glm::vec4(u,(-1.0 * glm::dot(u, cam.eye_position)));
	glm::vec4 row2 = glm::vec4(v,(-1.0 * glm::dot(v, cam.eye_position)));
	glm::vec4 row3 = glm::vec4(n,(-1.0 * glm::dot(n, cam.eye_position)));
	glm::vec4 row4 = glm::vec4(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);

	glm::mat4 Awv = glm::transpose(glm::mat4(row1, row2, row3, row4));

	printmat4(Awv);

	for (int i = 0; i < 3; ++i) {
		model_list[i].transformation_mtx = Awv * model_list[i].transformation_mtx; 
	}
}

void Scene::toCCS(){

	glm::mat4 sh = glm::mat4(1.0f);
	sh[2][0] = (cam.R + cam.L)/ (2.0*cam.N);
	sh[2][1] = (cam.T + cam.B)/ (2.0*cam.N);
	
	glm::mat4 sc = glm::mat4(1.0f);
	sc[0][0] = (2.0*cam.N)/(cam.R - cam.L);
	sc[1][1] = (2.0*cam.N)/(cam.T - cam.B);
	
	glm::mat4 Nm = glm::mat4(1.0f);
	Nm[2][2] = -(cam.F + cam.N)/(cam.F - cam.N);
	Nm[2][3] = -1.0f;
	Nm[3][2] = -(2.0*cam.F*cam.N)/(cam.F - cam.N);
	Nm[3][3] = 0.0f;
	
	glm::mat4 totaltransform = Nm*sc*sh;

	printmat4(totaltransform);
}

void Scene::toNDCS(){

}

void Scene::toDCS(){

}