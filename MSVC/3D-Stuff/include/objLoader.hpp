/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * See COPYING within this package for further information. 
 */
#pragma once

#include <glm/glm.hpp>

#include <vector>
#include <string>

struct Face {
	Face(const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2)
		: v0(v0),
		v1(v1),
		v2(v2),
		normal(glm::normalize(glm::cross(v1 - v0, v2 - v0)))
	{}

	const glm::vec3& v0;
	const glm::vec3& v1;
	const glm::vec3& v2;
	const glm::vec3 normal;
};

struct Obj {
	std::vector<glm::vec3> vertices;
	std::vector<Face> faces;
};

Obj loadObj(std::string);