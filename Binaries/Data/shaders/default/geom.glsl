#version 430 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 _PV;

out float height;
out float Distance;
out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

in DATA
{
	float height;
    vec3 FragPos;
    vec3 Normal;
    float distance;
	vec2 TexCoord;
} data_in[]; 

void main()
{	
	gl_Position = _PV * gl_in[0].gl_Position;
	Normal = data_in[0].Normal;
	height = data_in[0].height;
	Distance = data_in[0].distance;
	TexCoord = data_in[0].TexCoord;
	FragPos = data_in[0].FragPos;
	EmitVertex();

	gl_Position = _PV * gl_in[1].gl_Position;
	height = data_in[1].height;
	Distance = data_in[1].distance;
	Normal = data_in[1].Normal;
	TexCoord = data_in[1].TexCoord;
	FragPos = data_in[1].FragPos;
	EmitVertex();

	gl_Position =  _PV * gl_in[2].gl_Position;
	height = data_in[2].height;
	Distance = data_in[2].distance;
	Normal = data_in[2].Normal;
	TexCoord = data_in[2].TexCoord;
	FragPos = data_in[2].FragPos;
	EmitVertex();

	EndPrimitive();
} 