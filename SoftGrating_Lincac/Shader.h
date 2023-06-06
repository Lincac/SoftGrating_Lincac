#pragma once

#ifndef _SHADER_
#define _SHADER_

#include"CPU.h"
#include"FrameBuffer.h"
#include"Mat.h"

struct out_vertex
{
	vec3 position[3]; // 裁剪空间

	vec3 worldPos[3];
	vec2 uv[3];
	vec3 normal[3];

	vec3 culling_normal;
};

struct out_fragment
{
	vec3 color;
};

class Shader
{
public:
	Shader() {}
	~Shader() {}

	out_vertex excute_vertexShader(const vec3 &v0,const vec3 &v1,const vec3 &v2);
	void excute_geometryShader();
	out_fragment excute_fragmentShader(out_vertex &vert,vec3 &bc_screen);
	mat4 mvp;

	vec3 lightDir;
	vec3 lightCol;
	vec3 camPos;
	vec3 Ambient;
	float Gloss;

	std::shared_ptr<Texture> albedo;
	std::shared_ptr<Texture> normal;
	std::shared_ptr<Texture> specular;
};

out_vertex Shader::excute_vertexShader(const vec3& v0, const vec3& v1, const vec3& v2) {
	out_vertex temp;

	vec4 tempv0 = mvp * vec4(v0, 1);
	vec4 tempv1 = mvp * vec4(v1, 1);
	vec4 tempv2 = mvp * vec4(v2, 1); // MVP变换

	temp.position[0] = vec3(tempv0.x, tempv0.y, tempv0.z) / tempv0.w;
	temp.position[1] = vec3(tempv1.x, tempv1.y, tempv1.z) / tempv1.w;
	temp.position[2] = vec3(tempv2.x, tempv2.y, tempv2.z) / tempv2.w; // 透视除法

	temp.worldPos[0] = v0;
	temp.worldPos[1] = v1;
	temp.worldPos[2] = v2;

	temp.culling_normal = cross(v1 - v0, v2 - v0);
	temp.culling_normal.normalize();

	return temp;
}

out_fragment Shader::excute_fragmentShader(out_vertex& vert, vec3& bc_screen) {
	// 插值运算
	vec3 worldPos;
	worldPos.x = bc_screen.x * vert.worldPos[0].x + bc_screen.y * vert.worldPos[1].x + bc_screen.z * vert.worldPos[2].x;
	worldPos.y = bc_screen.x * vert.worldPos[0].y + bc_screen.y * vert.worldPos[1].y + bc_screen.z * vert.worldPos[2].y;
	worldPos.z = bc_screen.x * vert.worldPos[0].z + bc_screen.y * vert.worldPos[1].z + bc_screen.z * vert.worldPos[2].z;

	vec3 worldNormal;
	worldNormal.x = bc_screen.x * vert.normal[0].x + bc_screen.y * vert.normal[1].x + bc_screen.z * vert.normal[2].x;
	worldNormal.y = bc_screen.x * vert.normal[0].y + bc_screen.y * vert.normal[1].y + bc_screen.z * vert.normal[2].y;
	worldNormal.z = bc_screen.x * vert.normal[0].z + bc_screen.y * vert.normal[1].z + bc_screen.z * vert.normal[2].z;

	vec3 Bitangent, Tangent; // 计算TBN
	vec3 helper = vec3(1, 0, 0);
	if (abs(worldNormal.x) > 0.999) helper = vec3(0, 0, 1);
	Bitangent = cross(worldNormal, helper);
	Bitangent.normalize();
	Tangent = cross(worldNormal, Bitangent);
	Tangent.normalize();

	vec2 uv;
	uv.x = bc_screen.x * vert.uv[0].x + bc_screen.y * vert.uv[1].x + bc_screen.z * vert.uv[2].x;
	uv.y = bc_screen.x * vert.uv[0].y + bc_screen.y * vert.uv[1].y + bc_screen.z * vert.uv[2].y;

	out_fragment temp;

	vec3 Albedo = albedo->sample(uv.x,uv.y);
	vec3 tempNormal = normal->sample(uv.x,uv.y);
	vec3 Normal = vec3(dot(vec3(Tangent.x, Bitangent.x, worldNormal.x), tempNormal), dot(vec3(Tangent.y, Bitangent.y, worldNormal.y), tempNormal), dot(vec3(Tangent.z, Bitangent.z, worldNormal.z), tempNormal));
	Normal.normalize();
	vec3 Specular = specular->sample(uv.x, uv.y);

	vec3 viewDir = camPos - worldPos;
	viewDir.normalize();
	vec3 halfwayDir = lightDir + viewDir;
	halfwayDir.normalize();

	vec3 ambient = Ambient * Albedo;

	float diff = fmax(dot(Normal, lightDir), 0.0);
	vec3 diffuse = lightCol * Albedo * diff;

	float spec = pow(fmax(dot(Normal,halfwayDir), 0.0), Gloss);
	vec3 specular = lightCol * vec3(Specular.x) * spec;

	temp.color = ambient + diffuse + specular;

	return temp;
}

#endif // !_SHADER_
