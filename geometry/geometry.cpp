#include <vector>
#include <cassert>
#include <cmath>
#include <iostream>
#include "geometry.h"

//template <> Vec3<float>::Vec3(Matrix m) : x(m[0][0]/m[3][0]), y(m[1][0]/m[3][0]), z(m[2][0]/m[3][0]) {}
template <> template <> Vec3<int>::Vec3<>(const Vec3<float> &v) : x(int(v.x+.5)), y(int(v.y+.5)), z(int(v.z+.5)) {}
template <> template <> Vec3<float>::Vec3<>(const Vec3<int> &v) : x(v.x), y(v.y), z(v.z) {}

Matrix::Matrix(int r, int c):rows(r), cols(c) {
//vector<f>(a,b)：vector中包含a个b
    m = std::vector<std::vector<float> >(r, std::vector<float>(c, 0.f));
}

Matrix Matrix::identity(int dims){
    Matrix I(dims, dims);
    for(int i=0; i<dims; i++)
        for(int j=0; j<dims; j++)
            I[i][j] = (i==j? 1.f : 0.f);
    return I;
}

std::vector<float>& Matrix::operator [](const int i){
    assert(i>=0 && i<rows);
    return m[i];
}

Matrix Matrix::operator*(const Matrix& a){
    assert(cols == a.rows);
    Matrix result(rows, a.cols);
    for (int i=0; i<rows; i++) {
        for (int j=0; j<a.cols; j++) {
            result.m[i][j] = 0.f;
            for (int k=0; k<cols; k++) {
                result.m[i][j] += m[i][k]*a.m[k][j];
            }
        }
    }
    return result;
}

Matrix Matrix::transpose() {
    Matrix result(cols, rows);
    for(int i=0; i<rows; i++)
        for(int j=0; j<cols; j++)
            result[j][i] = m[i][j];
    return result;
}

Matrix Matrix::inverse() {
    assert(rows==cols);
    // augmenting the square matrix with the identity matrix of the same dimensions a => [ai]
    Matrix result(rows, cols*2);
    for(int i=0; i<rows; i++)
        for(int j=0; j<cols; j++)
            result[i][j] = m[i][j];
    for(int i=0; i<rows; i++)
        result[i][i+cols] = 1;
    // first pass
    for (int i=0; i<rows-1; i++) {
        // normalize the first row
        for(int j=result.cols-1; j>=0; j--)
            result[i][j] /= result[i][i];
        for (int k=i+1; k<rows; k++) {
            float coeff = result[k][i];
            for (int j=0; j<result.cols; j++) {
                result[k][j] -= result[i][j]*coeff;
            }
        }
    }
    // normalize the last row
    for(int j=result.cols-1; j>=rows-1; j--)
        result[rows-1][j] /= result[rows-1][rows-1];
    // second pass
    for (int i=rows-1; i>0; i--) {
        for (int k=i-1; k>=0; k--) {
            float coeff = result[k][i];
            for (int j=0; j<result.cols; j++) {
                result[k][j] -= result[i][j]*coeff;
            }
        }
    }
    // cut the identity matrix back
    Matrix truncate(rows, cols);
    for(int i=0; i<rows; i++)
        for(int j=0; j<cols; j++)
            truncate[i][j] = result[i][j+cols];
    return truncate;
}

std::ostream& operator<<(std::ostream& s, Matrix& m) {
    for (int i=0; i<m.nrows(); i++)  {
        for (int j=0; j<m.ncols(); j++) {
            s << m[i][j];
            if (j<m.ncols()-1) s << "\t";
        }
        s << "\n";
    }
    return s;
}

//对于一个透视中心在远点，方向为z负方向的视锥,将视锥变为平行棱柱
//然后将平行棱柱上所有的点映射到[-1,1]^3
//这里的height和width是视锥近平面的长宽（假设是标准场景，近平面关于原点对称）
// [2n/w, 0,   0,   0,
//  0,   2n/h, 0,   0,
//  0,   0,   -(f+n)/(f-n),  -2nf/(f-n),
//  0,   0,   -1,   0]
Matrix perspective(const float near, const float far, const float width, const float height){
    Matrix m1 = Matrix::identity(4);
    //透视变换，变成长方体并变换到[-1,1]^3
    m1[0][0] = 2*near/width;
    m1[1][1] = 2*near/height;
    m1[2][2] = -(far+near)/(far-near);
    m1[2][3] = -2*near*far/(far-near);
    m1[3][2] = -1;   

    return m1;
}


//一个4*4矩阵：
// [w/2, 0,   0,   x+w/2,
//  0,   h/2, 0,   y+h/2,
//  0,   0,   d/2, d/2,
//  0,   0,   0,   1]
//将一个[-1,1]^3空间内的点映射到[x,x+w]*[y,y+h]*[0,d]
Matrix viewport(const int x, const int y, const int w, const int h, const int depth){
    Matrix m = Matrix::identity(4);
    m[0][3] = x+w/2.f;
    m[1][3] = y+h/2.f;
    m[2][3] = depth/2.f;

    m[0][0] = w/2.f;
    m[1][1] = h/2.f;
    m[2][2] = depth/2.f;
    return m;
}

//lookat变换矩阵
//此矩阵实际上是对物体点的变换，将点的坐标从标准坐标系，变换到以相机为中心的坐标系
//相机的位置origin,看向target，则视轴为target-origin，相机上方为up
//对原坐标系变换：首先平移到origin
//然后旋转，x轴旋转到up*（target-origin）（相机右侧）
//y轴旋转到up，-z旋转到target-origin
Matrix lookat(Vec3f origin, Vec3f target, Vec3f up){
    Matrix transport = Matrix::identity(4);
    transport[0][3] = -origin.x;
    transport[1][3] = -origin.y;
    transport[2][3] = -origin.z;

    Matrix rotate = Matrix::identity(4);
    
    Vec3f forward = target - origin;
    Vec3f right = forward.crossproduct(up);
    
    rotate[0][0] = right.x;
    rotate[1][0] = right.y;
    rotate[2][0] = right.z;
    rotate[0][1] = up.x;
    rotate[1][1] = up.y;
    rotate[2][1] = up.z;
    rotate[0][2] = -forward.x;
    rotate[1][2] = -forward.y;
    rotate[2][2] = -forward.z;

    rotate = rotate.transpose();

    return rotate*transport;
}


Matrix transport(Vec3f v){
    Matrix transport = Matrix::identity(4);
    transport[0][3] = v.x;
    transport[1][3] = v.y;
    transport[2][3] = v.z;
    return transport;
}