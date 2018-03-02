/*
* edit from
*      https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Load_OBJ
*/

#ifndef LOADOBJ_HPP
#define LOADOBJ_HPP

#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <string>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

#include <glm/glm.hpp>

void LoadObj(
	const char* filename,
	std::vector<glm::vec4> &vertices,
	std::vector<glm::vec3> &normals,
	std::vector<GLushort> &elements) {

	std::ifstream in(filename, std::ios::in);
	if (!in) {
		std::cerr << "Cannot open " << filename << std::endl; exit(1);
	}

	std::string line;
	while (getline(in, line)) {
		if (line.substr(0, 2) == "v ") {
			std::istringstream s(line.substr(2));
			glm::vec4 v; s >> v.x; s >> v.y; s >> v.z; v.w = 1.0f;
			vertices.push_back(v);
		}
		else if (line.substr(0, 2) == "f ")
		{
			std::istringstream s(line.substr(2));
			GLushort a, b, c;
			s >> a; s >> b; s >> c;
			a--; b--; c--;
			elements.push_back(a); elements.push_back(b); elements.push_back(c);
		}
		else if (line[0] == '#') {
			/* ignoring this line */
		}
		else {
			/* ignoring this line */
		}
	}

	normals.resize(elements.size(), glm::vec3(0.0, 0.0, 0.0));
	for (int i = 0; i < static_cast<int>(elements.size()); i += 3)
	{
		GLushort ia = elements[i];
		GLushort ib = elements[i + 1];
		GLushort ic = elements[i + 2];
		glm::vec3 normal = glm::normalize(glm::cross(
			glm::vec3(vertices[ib]) - glm::vec3(vertices[ia]),
			glm::vec3(vertices[ic]) - glm::vec3(vertices[ia])
		));
		normals[ia] = normals[ib] = normals[ic] = normal;

	}
}

#endif // !LOADOBJ_HPP