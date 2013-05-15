/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
#include <OpenGLApp.h>

//#include <iostream>

//#include <glm/glm.hpp>
//#include <glm/gtc/quaternion.hpp>
#include <Freeglut/glut.h>

int main(int argc, char* argv[]) {
    OpenGLApp app(&argc, argv);
    return app.Run();

    // Blatt 6 Aufgabe 4b
    // --------------------
    //float half_angle = glm::radians(15.0);
    //float cos = std::cos(half_angle);
    //float sin = std::sin(half_angle);

    //auto q = glm::quat(cos, (2/sqrt(14))*sin, (3/sqrt(14))*sin, (1/sqrt(14))*sin);
    //std::cout << "quaternion norm: " << glm::length(q) << std::endl;

    //auto q_star = glm::inverse(q);

    //auto p = glm::quat(0, 2, 5, 1);

    //auto gedreht = q * p * q_star;

    //std::cout << "gedreht: [" << gedreht.w << ", (" << gedreht.x << ", " << gedreht.y << ", " << gedreht.z << ")]" << std::endl;

    //glm::vec3 vec_gedreht;
    //vec_gedreht.x = gedreht.x;
    //vec_gedreht.y = gedreht.y;
    //vec_gedreht.z = gedreht.z;

    //glm::mat3 R(.904304f, -.076213f, .420031f, .191048f, .952152f, -.238552f, -.381753f, .295970f, .875595f);

    //auto asd = R*vec_gedreht;

    //std::cout << "zurueckgedreht: (" << asd.x << ", " << asd.y << ", " << asd.z << ")" << std::endl;
}