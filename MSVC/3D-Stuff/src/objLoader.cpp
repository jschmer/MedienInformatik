#include "objLoader.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <map>
#include <set>

using namespace std;
using glm::vec3;

// Function to read input data values
bool readvals(stringstream &s, const int numvals, GLfloat* values) {
    for (int i = 0; i < numvals; i++) {
        string value;
        s >> value;

        try {
            auto v = std::stof(value);
            values[i] = v;
        }
        catch (std::exception) {
            cout << "Failed reading value " << i << " will skip\n";
            return false;
        }
    }

    return true;
}

bool readvals(stringstream &s, const int numvals, int* values) {
    for (int i = 0; i < numvals; i++) {
        string value;
        s >> value;

        try {
            auto v = std::stoi(value);
            values[i] = v;
        }
        catch (std::exception) {
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

// helper to calculate averaged vertex normal
void addToVec(std::vector<const GLfloat> &c, const set<Face*> &s) {
    vec3 average_normal;
    for (auto f : s)
        average_normal += f->normal;
    average_normal = glm::normalize(average_normal);
    c.push_back(average_normal.x);
    c.push_back(average_normal.y);
    c.push_back(average_normal.z);
}

void loadObj(string obj_file, Obj &obj) {
    // disable syncing with c stdio functions, we only need c++ streams!
    // improves performance of streams a lot!
    ifstream::sync_with_stdio(false);

    ifstream file(obj_file);

    if (file.is_open()) {
        auto& faces    = obj.faces;
        auto& vertices = obj.vertices;

        auto count = std::count(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), '\n');
        
        // jump back to beginning of file
        file.clear();
        file.seekg(0, ios::beg);

        faces.clear();
        faces.reserve(count);
        vertices.clear();
        vertices.reserve(count);

        // for averaging vertex normals
        map<vec3*, set<Face*>> vtx_triangle_map;

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

                    auto& v1 = vertices[idx[0]];
                    auto& v2 = vertices[idx[1]];
                    auto& v3 = vertices[idx[2]];

                    auto face = Face(v1, v2, v3);
                    faces.emplace_back(face);

                    // save the triangle for each vertex for averaging normals
                    vtx_triangle_map[&v1].insert(&faces.back());
                    vtx_triangle_map[&v2].insert(&faces.back());
                    vtx_triangle_map[&v3].insert(&faces.back());

                    face.normal;
                }
            }
        }

        file.close();

        // generate vertex and normal array for fast opengl rendering
        auto& gl_vertices = obj.gl_vertices;
        auto& gl_normals  = obj.gl_normals;
        auto& gl_normals_average  = obj.gl_normals_average;
        gl_vertices.clear();
        gl_vertices.reserve(vertices.size());
        for (auto& face : faces) {
            addToVec(gl_vertices, face.v0);
            addToVec(gl_vertices, face.v1);
            addToVec(gl_vertices, face.v2);

            addToVec(gl_normals, face.normal);
            addToVec(gl_normals, face.normal);
            addToVec(gl_normals, face.normal);

            // generate averaged vertex normals
            addToVec(gl_normals_average, vtx_triangle_map[&face.v0]);
            addToVec(gl_normals_average, vtx_triangle_map[&face.v1]);
            addToVec(gl_normals_average, vtx_triangle_map[&face.v2]);
        }
    }
    else {
        throw std::exception((string("Unable to open file ") + obj_file).c_str());
    }
}