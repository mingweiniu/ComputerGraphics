#version 420
layout(binding = 0) uniform sampler2D appleTex;
out vec4 color;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

flat in vec3 flatNormal;
flat in vec4 light;

in vec4 norm_vert;
in float diffuse_vert;
in float specular_vert;

flat in vec4 light_frag;
flat in vec4 norm_frag;


uniform vec3 viewPos;
uniform int Phong;

void main()
{
    vec4 texColor = texture(appleTex, TexCoord);

	vec3 light_dir = normalize(FragPos - light.xyz);
	vec3 view_dir = normalize(FragPos - viewPos);

	float ambient = 0.1;
	float diffuse_frag;
	float specular_frag;

	if(Phong == 1){
		specular_frag = max(dot(view_dir, normalize(reflect(-light_dir, Normal))),0);
		diffuse_frag = dot(Normal, -light_dir);
	}
	else{
		//specular_frag = max(dot(view_dir, normalize(reflect(-light_dir, flatNormal))),0);
		diffuse_frag = dot(flatNormal, -light_dir);
	}
	

	vec4 lightColor = vec4(
		0.9 * pow(specular_frag,16) + 0.9 * diffuse_frag + 0.1,
		0.9 * pow(specular_frag,16) + 0.9 * diffuse_frag + 0.1,
		0.9 * pow(specular_frag,16) + 0.9 * diffuse_frag + 0.1,
		1);

	color = mix(texColor,lightColor,0.5);
	
}
