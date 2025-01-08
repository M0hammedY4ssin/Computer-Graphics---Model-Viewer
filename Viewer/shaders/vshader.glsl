#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

//in  vec2 vTexCoord;
//in  vec2 vPosition;


// The model/view/projection matrices
uniform mat4 inversmat;
uniform mat4 Tmat;
uniform mat4 proj;
uniform mat4 lookAt_mat;
uniform int projection_flag;
uniform int draw;
uniform int draw_model;
uniform mat4 light_trans;
uniform int light_type;

// These outputs will be available in the fragment shader as inputs
out vec3 orig_fragPos;
out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragTexCoords;


void main()
{

	if(draw_model == 1)
	{
		vec4 a1;
		orig_fragPos = vec3(vec4(pos, 1.0f));
		fragPos = vec3( Tmat * vec4(pos, 1.0f));
		fragNormal = mat3( Tmat) * normal ;
	
		gl_Position = proj * lookAt_mat * inversmat * Tmat * vec4(pos, 1.0f);
		fragTexCoords = texCoords;
		fragTexCoords=vec2(orig_fragPos.x, orig_fragPos.y);//planar_texture
	}

	if(draw ==  1)
	{
		if(light_type == 1) //parallel
			gl_Position = proj * lookAt_mat * inversmat * vec4(pos,1.f);
		else
			gl_Position = proj * lookAt_mat * inversmat *  light_trans * vec4(pos, 1.0f);
	}

	
}
