#pragma once

#ifndef _CPU_
#define _CPU_

#include<vector>
#include<iostream>
#include"Vec.h"

class CPU
{
public:
	CPU() {};
	~CPU() {};

	std::vector<vec3> vertex;
	std::vector<int> Vindex;
	std::vector<vec3> normal;
	std::vector<int> Nindex;
	std::vector<vec2> texcoord;
	std::vector<int> Tindex;
};
#endif // !_CPU_
