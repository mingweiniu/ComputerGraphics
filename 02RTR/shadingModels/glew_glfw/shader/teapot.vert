#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 Flat_normal;
layout (location = 2) in vec3 Gouraud_normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


uniform mat4 size_adj = mat4(
	vec3(0.2, 0, 0), 0,
	vec3(0, 0.2, 0), 0,
	vec3(0, 0, 0.2), 0,
	0, -0.3, 0, 1
);

vec3 normal;
vec3 shift;

uniform vec3 light_vert = normalize(vec3(-1,-3,-1));
uniform vec3 view_vert = normalize(vec3(0,0,-1));

out float specular_vert;
out float diffuse_vert;

uniform int Flat;
out vec3 normal_frag;
out vec3 light_frag;
out vec3 view_frag;

void main()
{
    gl_Position = projection * view *  model * vec4(position, 1.0f);
	if(Flat == 1 ){
		normal = Flat_normal;
	}else{
		normal = Gouraud_normal;

		normal_frag = Gouraud_normal;
		light_frag = light_vert;
		view_frag = view_vert;
	}
		
	shift =  dot(light_vert, normal) * normal;
	specular_vert = dot(view_vert, shift*2 - light_vert);
	specular_vert = pow(max(specular_vert,0),4);

	diffuse_vert = -dot(light_vert, normal);

}
