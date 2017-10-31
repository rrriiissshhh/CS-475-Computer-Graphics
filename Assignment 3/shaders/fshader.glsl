#version 400

in vec4 color;
in vec2 tex;
in float spec_shine;
in float light_inten;
in vec4 position_pt;
in mat4 view_matrix;
in mat4 mult_matrix;
flat in int is_texture_present;

uniform sampler2D texture;

out vec4 frag_color;

void main () 
{
  if(is_texture_present == 1){
  	vec4 tex_color = texture2D(texture, tex);
  	vec3 temp_vec3 = vec3(min(tex_color.x,1.0), min(tex_color.y,1.0), min(tex_color.z,1.0));
  	vec4 new_color = vec4(min((temp_vec3*light_inten + vec3(spec_shine, spec_shine, spec_shine)), vec3(1.0)), 1.0);
  	
  	vec3 spotlightpos = vec3(0.0, 0.0, 3.0);
  	mat4 good_matrix = inverse(view_matrix);
    mat4 another_matrix = inverse(mult_matrix);
  	vec4 ori_position = another_matrix*good_matrix*(position_pt);
  	if(ori_position.x < 0.3 && ori_position.x > -0.3 && ori_position.z < 0.5 && ori_position.z > -0.5)
  		frag_color = vec4(new_color.x+0.2, new_color.y+0.2, new_color.z + 0.2, color.w);
  	else
  		frag_color = new_color;

  } else {
  
  	// vec3 spotlightpos = vec3(0.0, 0.0, 3.0);
  	mat4 good_matrix = inverse(view_matrix);
    mat4 another_matrix = inverse(mult_matrix);
  	vec4 ori_position = another_matrix*good_matrix*(position_pt);
  	if(ori_position.x < 0.3 && ori_position.x > -0.3 && ori_position.z < 0.5 && ori_position.z > -0.5)
  		frag_color = vec4(color.x+0.2, color.y+0.2, color.z + 0.2, color.w);
  	else
  		frag_color = color;
  }	
  	
}
