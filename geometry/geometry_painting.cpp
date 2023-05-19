#include "geometry_painting.h"

void line1(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
void line2(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
void line3(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
static Vec3i barycentric(Vec2i *pts, Vec2i P);
static Vec3i barycentric(Vec3i *pts, Vec2i P);

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

//画三角形边框
void triangle_edge(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color){
    line(t0, t1, image, color);
    line(t1, t2, image, color);
    line(t2, t0, image, color);
}

//填色三角形,方法1：扫线
void triangle2D1(Vec2i *pts, TGAImage &image, TGAColor color){
    Vec2i t0 = pts[0];
    Vec2i t1 = pts[1];
    Vec2i t2 = pts[2];
    //t0,t1,t2从低到高
    if (t0.y>t1.y) std::swap(t0, t1); 
    if (t0.y>t2.y) std::swap(t0, t2); 
    if (t1.y>t2.y) std::swap(t1, t2);
    
    int total_height = t2.y - t0.y + 1;
    
    for(int y = t0.y; y<=t1.y; y++){
        int segment_height = t1.y-t0.y+1; //先画三角形的下半部分
        float alpha = (float)(y-t0.y)/total_height; 
        float beta  = (float)(y-t0.y)/segment_height; 
        Vec2i A = t0 + (t2-t0)*alpha; 
        Vec2i B = t0 + (t1-t0)*beta; //A和B分别是左右两条边上同一水平线上的两点
        if (A.x>B.x) std::swap(A, B); 
        for (int j=A.x; j<=B.x; j++) { 
            image.set(j, y, color); // 
        } 
    }

    for (int y=t1.y; y<=t2.y; y++) { 
        int segment_height =  t2.y-t1.y+1; 
        float alpha = (float)(y-t0.y)/total_height; 
        float beta  = (float)(y-t1.y)/segment_height; // be careful with divisions by zero 
        Vec2i A = t0 + (t2-t0)*alpha; 
        Vec2i B = t1 + (t2-t1)*beta; 
        if (A.x>B.x) std::swap(A, B); 
        for (int j=A.x; j<=B.x; j++) { 
            image.set(j, y, color); // attention, due to int casts t0.y+i != A.y 
        } 
    }
}

//填色三角形，方法2：检测点是否在三角形内
void triangle2D2(Vec2i *pts, TGAImage &image, TGAColor color){
    Vec2i boundingbox_min(image.get_width()-1, image.get_height()-1);
    Vec2i boundingbox_max(0,0);
    for(int i=0; i<3; i++){
        boundingbox_max.x = std::max(boundingbox_max.x, pts[i].x);
        boundingbox_max.y = std::max(boundingbox_max.y, pts[i].y);

        boundingbox_min.x = std::min(boundingbox_min.x, pts[i].x);
        boundingbox_min.y = std::min(boundingbox_min.y, pts[i].y);
    }
    boundingbox_max.x = std::min(boundingbox_max.x, image.get_width()-1);
    boundingbox_max.y = std::min(boundingbox_max.y, image.get_height()-1);

    boundingbox_max.x = std::max(boundingbox_max.x, 0);
    boundingbox_max.y = std::max(boundingbox_max.y, 0);

    //将三个顶点的顺序调整为逆时针
    Vec3i l01(pts[1].x - pts[0].x, pts[1].y - pts[0].y, 0);
    Vec3i l02(pts[2].x - pts[0].x, pts[2].y - pts[0].y, 0);
    Vec3i cross12 = l01.crossproduct(l02);
    
    if(cross12.z < 0) std::swap(pts[1],pts[2]);


    for(int px = boundingbox_min.x; px<=boundingbox_max.x; px++){
        for(int py = boundingbox_min.y; py<=boundingbox_max.y; py++){
            Vec3i ifin_vec = ifin_triangle(pts, Vec2i(px,py));
            if(ifin_vec.x<0 || ifin_vec.y<0 || ifin_vec.z<0) continue;
                image.set(px,py,color);
        }
    }
    
}

//判断一个点是否在三角形内：逆时针连接三条边，分别做叉乘
template <typename T>
static Vec3<T> ifin_triangle(Vec2<T> *pts, Vec2<T> P){
    Vec3<T> l01(pts[1].x - pts[0].x, pts[1].y - pts[0].y, 0);
    Vec3<T> l02(pts[2].x - pts[0].x, pts[2].y - pts[0].y, 0);
    Vec3<T> l12 = l02-l01;
    Vec3<T> l20 = Vec3<T>() - l02;

    Vec3<T> ls[3] = {l01,l12,l20};
    int ifin_vec[3];

    for(int i=0; i<3; i++){
        Vec3<T> lp(P.x - pts[i].x, P.y - pts[i].y, 0);
        Vec3<T> c = ls[i].crossproduct(lp);
        ifin_vec[i] = c.z;
    }

    return Vec3i(ifin_vec);
}

template <typename T>
static Vec3<T> ifin_triangle(Vec3<T> *pts, Vec2<T> P){
    Vec3<T> l01(pts[1].x - pts[0].x, pts[1].y - pts[0].y, 0);
    Vec3<T> l02(pts[2].x - pts[0].x, pts[2].y - pts[0].y, 0);
    Vec3<T> l12 = l02-l01;
    Vec3<T> l20 = Vec3<T>() - l02;

    Vec3<T> ls[3] = {l01,l12,l20};
    int ifin_vec[3];

    for(int i=0; i<3; i++){
        Vec3<T> lp(P.x - pts[i].x, P.y - pts[i].y, 0);
        Vec3<T> c = ls[i].crossproduct(lp);
        ifin_vec[i] = c.z;
    }

    return Vec3<T>(ifin_vec);
}

//三角形重心坐标插值法
template <typename T>
static Vec3<T> barycentric(Vec2<T> *pts, Vec2<T> P){
    w1 = (pts[1].y - pts[2].y)*(P.x - pts[2].x) + (pts[2].x - pts[1].x)*(P.y - pts[2].y);
    w2 = (pts[2].y - pts[0].y)*(P.x - pts[2].x) + (pts[0].x - pts[2].x)*(P.y - pts[2].y);
    s = (pts[1].y - pts[2].y)*(pts[0].x - pts[2].x) + (pts[2].x - pts[1].x)*(pts[0].y - pts[2].y);
    return Vec3<T>(w1/s, w2/s, T(1-w1-w2));
}

template <typename T>
static Vec3<T> barycentric(Vec3<T> *pts, Vec3<T> P){
    w1 = (pts[1].y - pts[2].y)*(P.x - pts[2].x) + (pts[2].x - pts[1].x)*(P.y - pts[2].y);
    w2 = (pts[2].y - pts[0].y)*(P.x - pts[2].x) + (pts[0].x - pts[2].x)*(P.y - pts[2].y);
    s = (pts[1].y - pts[2].y)*(pts[0].x - pts[2].x) + (pts[2].x - pts[1].x)*(pts[0].y - pts[2].y);
    return Vec3<T>(w1/s, w2/s, T(1-w1-w2));
}


//给定三角形的屏幕标准三维坐标（屏幕坐标+深度），绘制图像
void triangle3D(Vec3f *pts_f, Vec3f itys, TGAImage &image, TGAColor color, int *zbuffer){
    Vec3i pts_i[3];
    for(int i=0; i<3; i++) pts_i[i] = vec_fl2int(pts_f[i]);

    Vec2i boundingbox_min(image.get_width()-1, image.get_height()-1);
    Vec2i boundingbox_max(0,0);
    for(int i=0; i<3; i++){
        boundingbox_max.x = std::max(boundingbox_max.x, pts_i[i].x);
        boundingbox_max.y = std::max(boundingbox_max.y, pts_i[i].y);

        boundingbox_min.x = std::min(boundingbox_min.x, pts_i[i].x);
        boundingbox_min.y = std::min(boundingbox_min.y, pts_i[i].y);
    }
    boundingbox_max.x = std::min(boundingbox_max.x, image.get_width()-1);
    boundingbox_max.y = std::min(boundingbox_max.y, image.get_height()-1);

    boundingbox_max.x = std::max(boundingbox_max.x, 0);
    boundingbox_max.y = std::max(boundingbox_max.y, 0);

    for(int px = boundingbox_min.x; px<=boundingbox_max.x; px++){
        for(int py = boundingbox_min.y; py<=boundingbox_max.y; py++){
            Vec3f weight = barycentric(pts_f, Vec3f(px,py,0));
            if(weight.x<0 || weight.y<0 || weight.z<0) continue;
                //对intensity进行计算:重心坐标法
                float ity = weight*itys;
                image.set(px,py,color*ity);
        }
    }
}



//根据一个obj文件，画出其中三角面片的所有框框,此处绘制直接舍弃z轴，没有透视投影
void frame_orthogonal(const char* obj_name, TGAImage& image){
    Model *model = NULL;
    const int width = image.get_width();
    const int height = image.get_height();
    model = new Model(obj_name);

    for(int i=0; i<model->nfaces(); i++){
        std::vector<int> face = model->face(i);
        for(int j=0; j<3; j++){
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j+1)%3]);
            
            int x0 = (v0.x+1.)*width/2.;
            int y0 = (v0.y+1.)*height/2.;
            int x1 = (v1.x+1.)*width/2.;
            int y1 = (v1.y+1.)*height/2.;

            line(Vec2i(x0,y0), Vec2i(x1,y1), image, white);
        }
    }

}


//根据一个obj文件，画出其中三角面片的所有框框,包含透视投影和视角移动
//同时还支持改变光源方向，假设光从某方向平行入射，
//面片的亮度由法线和光线方向夹角决定
void frame_perspective(
    const char* obj_name, TGAImage& image, 
    Vec3f origin, Vec3f target, Vec3f up, Vec3f light_dir,
    float fov, float aspect, float near, float far
){

    Model *model = NULL;
    const int width = image.get_width();
    const int height = image.get_height();
    model = new Model(obj_name);

    int *zbuffer = new int[width*height];
    for(int i=0; i<width*height; i++){
        //这个的意思是zbuffer的值先设置为int类型的最小值
        zbuffer[i] = std::numeric_limits<int>::min();
    }

    //绘制模型
    const int depth = 255;
    Matrix ModelView = lookat(origin, target, up);
    Matrix Projection = perspective(near, far, width, height);
    Matrix ViewPort = viewport(width/8, height/8, width*3/4, height*3/4, depth);

    std::cerr << ModelView << std::endl;
    std::cerr << Projection << std::endl;
    std::cerr << ViewPort << std::endl;
    Matrix z = (ViewPort*Projection*ModelView);
    std::cerr << z << std::endl;

    for(int i=0; i<model->nfaces(); i++){
        std::vector<int> face = model->face(i);
        Vec3f screen_coords[3];
        Vec3f world_coords[3];
        float intensity[3];
        for(int j=0; j<3; j++){
            Vec3f v = model->vert(face[j]);
            screen_coords[j] = fromMatrix(z*Matrix(v));
            world_coords[j] = v;
            //第i个面第j个顶点的顶点法向量*光线方向
            intensity[j] = model->norm(i,j)*light_dir;
        }
    }


}