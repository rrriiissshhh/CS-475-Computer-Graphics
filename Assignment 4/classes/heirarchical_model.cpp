#include "heirarchical_model.hpp"

HeirarchicalModel::HeirarchicalModel() {
}

/**
 * @brief Initialize heirarchical model
 * 
 * @param id_str id of model
 * @param scale_vec initial scaling vector
 * @param translation_vec initial positioning
 */
HeirarchicalModel::HeirarchicalModel(std::string id_str, glm::vec3 scale_vec, glm::vec3 translation_vec) {
	hm_id = id_str;
	scaling_matrix = glm::scale(glm::mat4(1.0f), scale_vec);
	xpos = next_xpos = translation_vec.x;
	ypos = next_ypos = translation_vec.y;
	zpos = next_zpos = translation_vec.z;

}

std::string HeirarchicalModel::get_id() {
	return hm_id;
}

/**
 * @brief Draw this heirarchical model on the screen
 * 
 * @param params OpenGL parameter to give input to shaders
 * @param light_flag light on/off status
 * @param projection_transform  perspective projection
 * @param tpt third person transform (Camera movement)
 * @param interpolation_factor between current and next frame 
 */
void HeirarchicalModel::draw_hm(OpenglParams* params, int light_flag, glm::mat4 projection_transform, glm::mat4 tpt, double interpolation_factor) {
	glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(xpos + interpolation_factor * (next_xpos - xpos), ypos + interpolation_factor * (next_ypos - ypos), zpos + interpolation_factor * (next_zpos - zpos)));
	draw(params, light_flag, glm::mat4(1.0f), projection_transform, tpt, tpt * translation_matrix * scaling_matrix, interpolation_factor);
}

/**
 * @brief save translation frame for this model and rotation frame for its tree
 * 
 * @param fp keyframe file pointer
 */
void HeirarchicalModel::save_keyframe_hm(FILE* fp) {
	fprintf(fp, "%.2f ", xpos);
	fprintf(fp, "%.2f ", ypos);
	fprintf(fp, "%.2f ", zpos);
	save_keyframe(fp);
}

/**
 * @brief load next frame
 * 
 * @param fp keyframe file pointer
 */
void HeirarchicalModel::load_next_keyframe_hm(FILE* fp) {
	xpos = next_xpos;
	ypos = next_ypos;
	zpos = next_zpos;

	fscanf(fp, "%f ", &next_xpos);
	fscanf(fp, "%f ", &next_ypos);
	fscanf(fp, "%f ", &next_zpos);
	load_next_keyframe(fp);
}

/**
 * @brief translate the heirarchical model
 * 
 * @param key_state_translation boolean vector giving status of keys pressed
 */
void HeirarchicalModel::translate(std::vector<bool> key_state_translation) {
	if (key_state_translation[4]) {
		xpos -= TRANS_DELTA;
	}
	else if (key_state_translation[5]) {
		xpos += TRANS_DELTA;
	}

	if (key_state_translation[2]) {
		ypos += TRANS_DELTA;
	}
	else if (key_state_translation[3]) {
		ypos -= TRANS_DELTA;
	}

	if (key_state_translation[0]) {
		zpos += TRANS_DELTA;
	}
	else if (key_state_translation[1]) {
		zpos -= TRANS_DELTA;
	}
}