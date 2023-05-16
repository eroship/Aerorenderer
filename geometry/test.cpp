#include<iostream>
#include "geometry_painting.h"
using namespace std;
int main()
{
    // TGAImage image(200,200, TGAImage::RGB);
    // Vec2i t0[3] = {Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80)};
    // Vec2i t1[3] = {Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180)};
    // Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};

    // triangle2(t0, image, red);
    // triangle2(t1, image, white);
    // triangle2(t2, image, green);
    TGAImage image(800,800, TGAImage::RGB);
    frame_orthogonal("../obj/african_head/african_head.obj", image);

    image.flip_vertically(); 
    image.write_tga_file("output.tga");
    
    return 0;
}