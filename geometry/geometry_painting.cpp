#include "geometry_painting.h"

void line1(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
void line2(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
void line3(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);

//第一种画线方法：指定两个顶点和步长(顶点的坐标是像素坐标)
void line1(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    for (float t=0.; t<1.; t+=.01) {
        int x = x0*(1.-t) + x1*t;
        int y = y0*(1.-t) + y1*t;
        image.set(x, y, color);
    }
}

//第二种画线方法：自动步长以及自动判断直线是否很陡
//因为是像素坐标，所以让x每次前进一格就行，但是这样对于很陡的线就离散了
//所以首先检测一下斜率是否大于1，如果是就xy倒一下
void line2(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color){
    bool steep = false;
    int distx = std::abs(x0-x1);
    int disty = std::abs(y0-y1);
    if(distx<disty){
        std::swap(x0,y0);
        std::swap(x1,y1);
        steep = true;
    }
    if(x0>x1){
        std::swap(x0,x1);
        std::swap(y0,y1);
    }

    for(int x=x0; x<=x1; x++){
        float t = (x-x0)/(float(x1-x0));
        int y = y0*(1.0-t)+y1*t;
        if(steep)
            image.set(y,x,color);
        else 
            image.set(x,y,color);
    }
}

//第三种,不用每步计算除法
//理论上，x每前进一格，y应该变换dy/dx格，但是由于像素坐标是离散的，所以会导致误差
//如果某一格的误差>0.5，说明这一格的y该换行了
//如果严格计算误差e，仍然需要除法：e每次增加dy/dx，大于0.5时换行：
//e+dy/dx>0.5, e=e-1
//做一次归一化避免计算除法：
//2edx+2dy>dx, E=E-2dx(E=2edx)
void line3(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    bool steep = false;
    int distx = std::abs(x0-x1);
    int disty = std::abs(y0-y1);
    if(distx<disty){
        std::swap(x0,y0);
        std::swap(x1,y1);
        std::swap(distx,disty);
        steep = true;
    }
    if(x0>x1){
        std::swap(x0,x1);
        std::swap(y0,y1);
    }

    int dy2 = disty*2;
    int error2 = 0;
    int y = y0;
    for(int x=x0; x<=x1; x++){
        if(steep)
            image.set(y,x,color);
        else 
            image.set(x,y,color);
        
        error2 += dy2; //下一个点的二倍误差
        if(error2 > distx){
            y += (y1>y0?1:-1);
            error2 -= distx*2;
            //std::cout<<error2<<std::endl;
        }
    }
}

void line(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color) {
    bool steep = false;
    if (std::abs(p0.x-p1.x)<std::abs(p0.y-p1.y)) {
        std::swap(p0.x, p0.y);
        std::swap(p1.x, p1.y);
        steep = true;
    }
    if (p0.x>p1.x) {
        std::swap(p0, p1);
    }

    for (int x=p0.x; x<=p1.x; x++) {
        float t = (x-p0.x)/(float)(p1.x-p0.x);
        int y = p0.y*(1.-t) + p1.y*t;
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
    }
}

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color){
    line(t0, t1, image, color);
    line(t1, t2, image, color);
    line(t2, t0, image, color);
}