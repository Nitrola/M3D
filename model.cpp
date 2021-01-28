
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *filename) : verts(), faces() {
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec3f v;
            for (int i=0;i<3;i++) iss >> v.raw[i];
            
            verts.push_back(v);
        }else if (!line.compare(0, 3, "vn ")) {
            iss >> trash >> trash;
            Vec3f n;
            for (int i=0;i<3;i++) iss >> n[i];
            norms.push_back(n);
        }else if(!line.compare(0,3,"vt ")){
            iss >> trash >> trash;
            Vec3f vt;
            for(int i=0;i<3;i++){ 
                iss >> vt.raw[i];
            }
            vts.push_back(vt);
        }
            else if (!line.compare(0, 2, "f ")) {
            std::vector<Vec3i> f;
            Vec3i tmp;
            iss >> trash;
            while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2]) {
                for (int i=0; i<3; i++) tmp[i]--; // in wavefront obj all indices start at 1, not zero
                f.push_back(tmp);
            }
            //std::cout << f.size() << std::endl;
            faces.push_back(f);
        }
    }
    //std::cerr << "# v# " << verts.size() << " f# "  << faces.size() << std::endl;
}

int Model::nb_verts(){
    return verts.size();
}

int Model::nb_verts_texture(){
    return vts.size();
}

int Model::nb_faces(){
    return faces.size();
}

Vec3f Model::vert_texture(int i){
    return vts[i];
}
Vec3f Model::vert(int i){
    return verts[i];
}

std::vector<int> Model::face(int idx) {
    std::vector<int> face;
    for (int i=0; i<(int)faces[idx].size(); i++) face.push_back(faces[idx][i][0]);
    return face;
}
std::vector<int> Model::face_texture(int idx) {
    std::vector<int> face;
    for (int i=0; i<(int)faces[idx].size(); i++) face.push_back(faces[idx][i][1]);
    return face;
}
Model::~Model(){
}
