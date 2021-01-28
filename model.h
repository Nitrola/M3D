#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"

class Model{
private:
    std::vector<Vec3f> verts;
    std::vector<Vec3f> vts;
    std::vector<Vec3f> norms;
	std::vector<std::vector<Vec3i> > faces; 
public:
    Model(const char *filename);
    ~Model();
    int nb_verts();
    int nb_verts_texture();
    int nb_faces();
    Vec3f vert(int i);
    Vec3f vert_texture(int i);
    std::vector<int> face_texture(int idx);
    std::vector<int> face(int idx);
};

#endif //__MODEL_H__
