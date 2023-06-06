#pragma once

#ifndef _TEXTURE_
#define _TEXTURE_

#include<iostream>
#include<stb_image.h>
#include<stb_image_write.h>
#include <string>
#include <sstream>
#include<fstream>
#include"Vec.h"

enum COLOR
{
	RED = 1,
	RGB = 3,
	RGBA = 4
};

class Texture {
public:
	Texture() : data(nullptr), width(0), height(0), channels(0) {};
	~Texture() { if (data) delete[] data; };

	bool Load(const char* path);
	void Write();
	void generate_Texture(int w, int h, int n, float fill = 0);
	float* get_data() { return data; }
	int get_width() { return width; }
	int get_height() { return height; }
	int get_channels() { return channels; }
	bool check_texture() { return data ? true : false; }
	bool set_pixel(int x, int y, const vec3& color);
	bool set_pixel(int x, int y, float t);
	float get_pixel(int x, int y);
	vec3 get_Color(int x, int y);
	vec3 sample(float x, float y);
	void clean() { if (data) delete[] data; }
	void reset(float fill);
	void show_data();
private:
	float* data;
	int width,height,channels;
};

bool Texture::Load(const char* path) {
	stbi_set_flip_vertically_on_load(true);
	if (data) delete[] data;
	data = stbi_loadf(path, &width, &height, &channels,0);
	if (!data) {
		std::cout << "error to load texture : " << path << std::endl;
		return false;
	}
	return true;
}

void Texture::Write() {

}

void Texture::generate_Texture(int w, int h, int n,float fill) {
	if (data) delete[] data;
	width = w;
	height = h;
	channels = n;
	unsigned long nbytes = width * height * channels;
	data = new float[nbytes];
	std::fill(data, data + nbytes, fill);
}

bool Texture::set_pixel(int x, int y, const vec3& color) {
	if (!data || x < 0 || x >= width || y < 0 || y >= height) {
		return false;
	}
	memcpy(data + (x + y * width) * channels, color.data, channels * sizeof(float));
	return true;
}

bool Texture::set_pixel(int x,int y,float t){
	if (!data || x < 0 || x >= width || y < 0 || y >= height) {
		return false;
	}
	data[(x + y * width) * channels] = t;
	return true;
}

float Texture::get_pixel(int x, int y) {
	if (!data || x < 0 || x >= width || y < 0 || y >= height) {
		return -INF;
	}
	return data[(x + y * width) * channels];
}

vec3 Texture::get_Color(int x, int y) {
	if (!data || x < 0 || x >= width || y < 0 || y >= height) {
		return vec3(0);
	}
	if (channels == 1) {
		float temp;
		memcpy(&temp, data + (x + y * width) * channels, channels * sizeof(float));
		return vec3(temp);
	}
	else if (channels == 2) {
		float temp[2];
		memcpy(temp, data + (x + y * width) * channels, channels * sizeof(float));
		return vec3(temp[0], temp[1],0.0);
	}
	else if (channels == 3) {
		float temp[3];
		memcpy(temp, data + (x + y * width) * channels, channels * sizeof(float));
		return vec3(temp[0], temp[1], temp[2]);
	}
}

vec3 Texture::sample(float x, float y) {
	return get_Color(x * width, y * height);
}

void Texture::reset(float fill) {
	unsigned long nbytes = width * height * channels;
	std::fill(data, data + nbytes, fill);
}

void Texture::show_data() {
	if (!data) {
		std::cerr << "this data is null" << std::endl;
		exit(-1);
	}
	for (long i = 0; i < width * height * channels; i++)
		std::cout << data[i] << ' ';
}


#endif // !_TEXTURE_
