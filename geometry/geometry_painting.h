#ifndef __GEOMETRY_PAINTING__
#define __GEOMETRY_PAINTING__

#include<iostream>
#include "tgaimage.h"
#include <vector>
#include <limits>
#include <cmath>
#include "geometry.h"
#include "model.h"

void line1(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
void line2(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
void line3(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
void line(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color);

void triangle_edge(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color);
void triangle1(Vec2i *pts, TGAImage &image, TGAColor color);
void triangle2(Vec2i *pts, TGAImage &image, TGAColor color);

void frame_orthogonal(const char* obj_name, TGAImage& image);
#endif