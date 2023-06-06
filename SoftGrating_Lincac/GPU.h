#pragma once

#ifndef _GPU_
#define _GPU_

#include"Shader.h"
#include"FrameBuffer.h"
#include"CPU.h"

class GPU
{
public:
	GPU() {}
	~GPU() {}

	void set_CPU_Data(std::shared_ptr<CPU> data) { CPU_Data = data; }
	void set_MSAA(bool is_open) { open_MSAA = is_open; }
	void set_Shader(std::shared_ptr<Shader> sha) { shader = sha; }
	void set_BindFramebuffer(std::shared_ptr<FrameBuffer> buffer) { framebuffer = buffer; }

	std::shared_ptr<Shader> get_Shader() { return shader; }

	std::shared_ptr<Texture> get_ColorBuffer() { return framebuffer->get_ColorBuffer(); }

	void excute();
	void reset() { framebuffer->reset(); }
private:
	std::shared_ptr<CPU> CPU_Data;
	std::shared_ptr<Shader> shader;
	std::shared_ptr<FrameBuffer> framebuffer;

	bool open_MSAA;

	void viewport_transform(out_vertex& vert, int width, int height);
	void viewport_Culling(out_vertex& vert);
	bool backFace_Culling(out_vertex& vert,vec3 camPos);

	vec3 barycentric(vec2 A, vec2 B, vec2 C, vec2 P);
	void triangle(vec2 v1, vec2 v2, vec2 v3, out_vertex& vert);
};

void GPU::excute() {
	if (CPU_Data->vertex.empty() || CPU_Data->Vindex.empty()) {
		std::cerr << "vertex/index data is empty" << std::endl;
		return;
	}

	for (int i = 0; i < CPU_Data->Vindex.size(); i += 3) {
		vec3 v0 = CPU_Data->vertex[CPU_Data->Vindex[i + 0]];
		vec3 v1 = CPU_Data->vertex[CPU_Data->Vindex[i + 1]];
		vec3 v2 = CPU_Data->vertex[CPU_Data->Vindex[i + 2]];

		// 几何阶段
		out_vertex vert = shader->excute_vertexShader(v0,v1,v2);
		if (!CPU_Data->texcoord.empty()) {
			vert.uv[0] = CPU_Data->texcoord[CPU_Data->Tindex[i + 0]];
			vert.uv[1] = CPU_Data->texcoord[CPU_Data->Tindex[i + 1]];
			vert.uv[2] = CPU_Data->texcoord[CPU_Data->Tindex[i + 2]];
		}
		if (!CPU_Data->normal.empty()) {
			vert.normal[0] = CPU_Data->normal[CPU_Data->Nindex[i + 0]];
			vert.normal[1] = CPU_Data->normal[CPU_Data->Nindex[i + 1]];
			vert.normal[2] = CPU_Data->normal[CPU_Data->Nindex[i + 2]];
		}

		// 视口变换
		viewport_transform(vert, framebuffer->get_width(), framebuffer->get_height());

		//视口剔除
		viewport_Culling(vert);

		// 背面剔除
		if (backFace_Culling(vert, shader->camPos)) continue; // 优化 ：提前到光栅化之前

		// 光栅化
		triangle(vec2(vert.position[0].x, vert.position[0].y), vec2(vert.position[1].x, vert.position[1].y), vec2(vert.position[2].x, vert.position[2].y),vert);
	}

}

void GPU::viewport_transform(out_vertex& vert,int width,int height) {
	vec3 vp0 = vec3(int((vert.position[0].x + 1.0) * width / 2), int((vert.position[0].y + 1.0) * height / 2), (vert.position[0].z + 1) / 2);
	vec3 vp1 = vec3(int((vert.position[1].x + 1.0) * width / 2), int((vert.position[1].y + 1.0) * height / 2), (vert.position[1].z + 1) / 2);
	vec3 vp2 = vec3(int((vert.position[2].x + 1.0) * width / 2), int((vert.position[2].y + 1.0) * height / 2), (vert.position[2].z + 1) / 2);

	vert.position[0] = vp0;
	vert.position[1] = vp1;
	vert.position[2] = vp2;
}

void GPU::viewport_Culling(out_vertex& vert) {
	vert.position[0].x = Math::clamp(0.0, framebuffer->get_width(), vert.position[0].x);
	vert.position[0].y = Math::clamp(0.0, framebuffer->get_height(), vert.position[0].y);

	vert.position[1].x = Math::clamp(0.0, framebuffer->get_width(), vert.position[1].x);
	vert.position[1].y = Math::clamp(0.0, framebuffer->get_height(), vert.position[1].y);

	vert.position[2].x = Math::clamp(0.0, framebuffer->get_width(), vert.position[2].x);
	vert.position[2].y = Math::clamp(0.0, framebuffer->get_height(), vert.position[2].y);
}

bool GPU::backFace_Culling(out_vertex& vert, vec3 camPos) {
	vec3 viewDir = camPos - vert.worldPos[0];
	viewDir.normalize();
	float temp = dot(vert.culling_normal, viewDir);
	return temp < 0;
}

vec3 GPU::barycentric(vec2 A, vec2 B, vec2 C, vec2 P) {
	vec3 t1 = vec3(C.x - A.x, B.x - A.x, A.x - P.x);
	vec3 t2 = vec3(C.y - A.y, B.y - A.y, A.y - P.y);

	vec3 u = cross(t1, t2);
	if (std::abs(u.z) > 1e-2)
		return vec3(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
	return vec3(-1, 1, 1);
}

void GPU::triangle(vec2 v1, vec2 v2, vec2 v3, out_vertex& vert) {

	vec2 maxBox = vecMax(v1, vecMax(v2, v3));
	vec2 minBox = vecMin(v1, vecMin(v2, v3));

	maxBox = vecMin(vec2(framebuffer->get_width() - 1, framebuffer->get_height() - 1), maxBox);
	minBox = vecMax(vec2(0), minBox);

	// 光栅化
	vec2 tempPos;
	for (tempPos.y = minBox.y; tempPos.y <= maxBox.y; tempPos.y++)
	{
		for (tempPos.x = minBox.x; tempPos.x <= maxBox.x; tempPos.x++)
		{
			vec3 bc_screen = barycentric(v1, v2, v3, tempPos);
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;

			float screenz = framebuffer->get_Depth(tempPos.x,tempPos.y);

			float currentz = 0.0;
			currentz += bc_screen.x * vert.position[0].z;
			currentz += bc_screen.y * vert.position[1].z;
			currentz += bc_screen.z * vert.position[2].z;

			if (currentz >= screenz) continue; // early-z

			out_fragment frag = shader->excute_fragmentShader(vert,bc_screen);
			vec3 color = Gamma_correction(frag.color); // 屏幕颜色差值

			framebuffer->set_Color(int(tempPos.x), int(tempPos.y), frag.color);
			framebuffer->set_Depth(int(tempPos.x), int(tempPos.y), currentz);
		}
	}
}

#endif // !_GPU_
