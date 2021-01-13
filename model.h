#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"

class Model{
private:
    std::vector<Vec3f> verts;
    std::vector<std::vector<int>> faces;
public:
    Model(const char *filename);
    ~Model();
    int nb_verts();
    int nb_faces();
    Vec3f vert(int i);
    std::vector<int> face(int idx);
};

#endif //__MODEL_H__
