#pragma once

#ifndef _FRAMEBUFFER_
#define _FRAMEBUFFER_

#include<iostream>
#include<string>
#include<vector>
#include<map>
#include"Texture.h"

class FrameBuffer
{
public:
	FrameBuffer() {};
	~FrameBuffer() {};

	void add_ColorBuffer(std::shared_ptr<Texture> buffer) { ColorBuffer = buffer; }
	void add_DepthBuffer(std::shared_ptr<Texture> buffer) { DepthBuffer = buffer; }

	int get_width() { return DepthBuffer->get_width(); }
	int get_height() { return DepthBuffer->get_height(); }

	void set_Color(int x, int y, vec3 &color) { ColorBuffer->set_pixel(x, y, color); }

	std::shared_ptr<Texture> get_ColorBuffer() { return ColorBuffer; }

	void set_Depth(int x, int y, float t) { DepthBuffer->set_pixel(x, y, t); }
	float get_Depth(int x, int y) { return DepthBuffer->get_pixel(x, y); }

	bool check_buffer_complete();

	void reset();
private:
	std::shared_ptr<Texture> ColorBuffer;
	std::map<std::string,std::shared_ptr<Texture>> G_Buffer;
	std::shared_ptr<Texture> DepthBuffer;
};

bool FrameBuffer::check_buffer_complete() {
	if (!ColorBuffer->check_texture() || !DepthBuffer->check_texture()) {
		std::cerr << "buffer is not complete" << std::endl;
		return false;
	}

	return true;
}


void FrameBuffer::reset() {
	ColorBuffer->reset(0.0);
	DepthBuffer->reset(1.0);
	for (auto& temp : G_Buffer) {
		temp.second->reset(0.0);
	}
}
#endif // !_FRAMEBUFFER_
