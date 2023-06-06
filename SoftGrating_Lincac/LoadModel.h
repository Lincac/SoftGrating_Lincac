#ifndef _MODEL_H_
#define _MODEL_H_

#include <vector>
#include <string>
#include <sstream>
#include<fstream>
#include"Texture.h"
#include "vec.h"

class Model {
public:
    Model(const char* path);
	~Model() {}

	std::vector<vec3> vertex;
	std::vector<int> Vindex;
	std::vector<vec3> normal;
	std::vector<int> Nindex;
	std::vector<vec2> texcoord;
	std::vector<int> Tindex;

	std::shared_ptr<Texture> Albedo;
	std::shared_ptr<Texture> Normal;
	std::shared_ptr<Texture> Specular;
	std::shared_ptr<Texture> Roughness;
	std::shared_ptr<Texture> Metallic;
	std::shared_ptr<Texture> Ao;
private:
	void load_Texture(const char* path);
};

Model::Model(const char* path) {
	std::ifstream file(path);
	std::string line;
	if (!file.is_open())
	{
		std::cerr << path << " can not open : \n";
		exit(-1);
	}

	float maxX = -INF, maxY = -INF, maxZ = -INF;
	float minX = INF, minY = INF, minZ = INF;

	while (std::getline(file, line))
	{
		std::istringstream str(line);
		std::string type;

		float x, y, z;
		int index1, index2, index3;

		str >> type;
		if (type == "v")
		{
			str >> x >> y >> z;
			maxX = x > maxX ? x : maxX;
			maxY = y > maxY ? y : maxY;
			maxZ = z > maxZ ? z : maxZ;

			minX = x < minX ? x : minX;
			minY = y < minY ? y : minY;
			minZ = z < minZ ? z : minZ;

			vertex.push_back(vec3(x, y, z));
		}
		else if (type == "vt") {
			str >> x >> y >> z;
			texcoord.push_back(vec2(x, y));
		}
		else if (type == "vn") {
			str >> x >> y >> z;
			normal.push_back(vec3(x, y, z));
		}
		else if (type == "f") {
			if (line.find('/') != std::string::npos) {
				std::istringstream temp(line.c_str());
				char trash;
				int f, t, n;
				temp >> trash;
				while (temp >> f >> trash >> t >> trash >> n)
				{
					Vindex.push_back(f - 1);
					Tindex.push_back(t - 1);
					Nindex.push_back(n - 1);
				}
			}
			else {
				str >> index1 >> index2 >> index3;
				
				Vindex.push_back(index1 - 1);
				Vindex.push_back(index2 - 1);
				Vindex.push_back(index3 - 1);
			}
		}
	}
	file.close();

	float deltax = (maxX - minX) == 0 ? 1 : maxX - minX;
	float deltay = (maxY - minY) == 0 ? 1 : maxY - minY;
	float deltaz = (maxZ - minZ) == 0 ? 1 : maxZ - minZ;

	vec3 center;
	center.x = (maxX + minX) / 2.0;
	center.y = (maxY + minY) / 2.0;
	center.z = (maxZ + minZ) / 2.0;

	for (auto& temp : vertex) {
		temp = temp - center;
		temp.x /= deltax;
		temp.y /= deltay;
		temp.z /= deltaz;
	}

	load_Texture(path);
}

void Model::load_Texture(const char* path) {
	std::string name;
	std::string dir = path;
	dir = dir.substr(0, dir.find_last_of('/'));
	name = dir.substr(dir.find_last_of('/') + 1, dir.length());

	std::string albedoPath = dir + "/" + name + "_diffuse.tga";
	std::string normalPath = dir + "/" + name + "_nm_tangent.tga";
	std::string specular = dir + "/" + name + "_spec.tga";

	Albedo = std::make_shared<Texture>();
	Albedo->Load(albedoPath.c_str());
	Normal = std::make_shared<Texture>();
	Normal->Load(normalPath.c_str());
	Specular = std::make_shared<Texture>();
	Specular->Load(specular.c_str());
}

#endif