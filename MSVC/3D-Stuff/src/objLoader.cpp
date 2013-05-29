#include "objLoader.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>

using namespace std;
using glm::vec3;

// Function to read input data values
template <typename T>
bool readvals(stringstream &s, const int numvals, T* values) {
    for (int i = 0; i < numvals; i++) {
        s >> values[i];

        if (s.fail()) {
            cout << "Failed reading value " << i << " will skip\n";
            return false;
        }
    }

    return true;
}

//string split helper
vector<string> split(string s, char delim) {
    vector<string> ret;
    ret.reserve(s.length());

    auto idx0 = 0u;
    auto idx1 = s.find(delim);
    while (idx1 != string::npos) {
        ret.push_back(s.substr(idx0, idx1 - idx0));
        idx0 = idx1 + 1;
        idx1 = s.find(delim, idx0);
    }
    ret.push_back(s.substr(idx0));

    return ret;
}

// helper to push float members of a vec3 to a vector
void addToVec(std::vector<const GLfloat> &c, const glm::vec3 &v) {
    c.push_back(v.x);
    c.push_back(v.y);
    c.push_back(v.z);
}

void loadObj(string obj_file, Obj &obj) {
    // disable syncing with c stdio functions, we only need c++ streams!
    // improves performance of streams a lot!
    ifstream::sync_with_stdio(false);

    ifstream file(obj_file);

    if (file.is_open()) {
        auto& faces    = obj.faces;
        auto& vertices = obj.vertices;

        faces.clear();
        vertices.clear();

        // read file line by line as long as it's good
        while (file.good()) {
            // read line
            string line;
            getline(file, line);

            if (line.empty())
                continue;

            stringstream ssline(line);

            // extract mode (v, f, vn, ...)
            string mode;
            ssline >> mode;

            // erase mode from input line
            line = string(line.begin()+2, line.end());
            ssline.str(line);

            // interpret mode
            if (mode == "#")
                continue;

            else if (mode == "v") {
                // read vertex data
                vector<float> coords(3, 0.0f);
                if (readvals(ssline, 3, &coords[0])) {
                    vertices.push_back(vec3(coords[0], coords[1], coords[2]));
                }
            }

            else if (mode == "f") {
                // read triangle face, vertices are indexed beginning from 1

                // strip out any normals or texture specification
                auto vertex_indices = split(line, ' ');
                auto stripped_indices = accumulate(begin(vertex_indices),
                                                   end(vertex_indices),
                                                   string(),
                                                   [](string &left, string right) -> string {
                                                       auto spl = split(right, '/');
                                                       return left + " " + spl[0];
                                                   });
                ssline.str(stripped_indices);
                vector<int> idx(3, 0);
                if (readvals(ssline, 3, &idx[0])) {
                    std::transform(begin(idx), end(idx), begin(idx), [](int i) { return --i; });
                    faces.emplace_back(Face(vertices[idx[0]], vertices[idx[1]], vertices[idx[2]]));
                }
            }
        }

        file.close();

        // generate vertex array for fast opengl rendering
        auto& gl_vertices = obj.gl_vertices;
        gl_vertices.clear();
        gl_vertices.reserve(vertices.size());
        for (auto const face : faces) {
            addToVec(gl_vertices, face.v0);
            addToVec(gl_vertices, face.v1);
            addToVec(gl_vertices, face.v2);
        }

    }
    else {
        throw std::exception((string("Unable to open file ") + obj_file).c_str());
    }
}