#version 330 core
out vec4 color;

uniform float ambient = 0.1;
vec3 shift;

in float specular_vert;
float specular_frag;
float specular;

in float diffuse_vert;
float diffuse_frag;
float diffuse;

uniform int Phong;
in vec3 normal_frag;
in vec3 light_frag;
in vec3 view_frag;

void main()
{
    
	if (Phong == 1){
		shift =  dot(light_frag, normal_frag) * normal_frag;
		specular_frag = dot(view_frag, shift*2 - light_frag);
		specular_frag = pow(max(specular_frag,0),4);

		diffuse = max(0,-dot(light_frag, normal_frag));
		specular = max(0,specular_frag);
	
	}else{
		diffuse = max(0,diffuse_vert);
		specular = max(0,specular_vert);
	}
	color = vec4(diffuse + specular + ambient, specular + ambient, specular, 1);
	
}
