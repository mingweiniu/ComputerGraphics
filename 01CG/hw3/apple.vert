#version 420
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoord;

flat out vec3 flatNormal;
uniform vec3 light_pos;
flat out vec4 light;



out vec4 norm_vert;
out float diffuse_vert;
out float specular_vert;

flat out vec4 norm_frag;


void main()
{
    gl_Position = Projection * View * Model * vec4(position, 1.0f);

	Normal = normalize(mat3(transpose(inverse(Model))) * normal); 
	FragPos = vec3(Model * vec4(position, 1.0f));
	TexCoord = texCoord;

	flatNormal = normalize(mat3(transpose(inverse(Model))) * normal); 

	light = View * vec4(10,10,0,1);

}
