#ifndef __MODEL_H__
#define __MODEL_H__
#include <vector>
#include "geometry.h"
#include "tgaimage.h"

class Model {
private:
    std::vector<Vec3f> verts_; //n*3,所有顶点
    std::vector<std::vector<Vec3i> > faces_; 
    //每个face包含其3个顶点的编号、纹理、顶点法向量
    std::vector<Vec3f> norms_;
    std::vector<Vec2f> uv_;
    TGAImage diffusemap_;
    void load_texture(std::string filename, const char *suffix, TGAImage &img);
public:
    Model(const char *filename);
    ~Model();
    int nverts();
    int nfaces();
    Vec3f norm(int iface, int nvert);
    Vec3f vert(int i);
    Vec2i uv(int iface, int nvert);
    TGAColor diffuse(Vec2i uv);
    std::vector<int> face(int idx);
};

#endif //__MODEL_H__