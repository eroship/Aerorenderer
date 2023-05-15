#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cmath>

//复习一下template：此为一个类模板，C++中struct类似class
template <class t> struct Vec2{
	t x,y;

    Vec2<t>(): x(t()),y(t()){}
    Vec2<t>(t _u, t _v) : x(_u),y(_v) {}
	Vec2<t>(t *vec):x(vec[0]),y(vec[1]){}
	Vec2<t>(const Vec2<t> &v):x(t()),y(t()){ *this = v; }
	Vec2<t> & operator =(const Vec2<t> &v){
		if(this != &v){
			x = v.x;
			y = v.y;
		}
		return *this;
	}

    inline Vec2<t> operator +(const Vec2<t> &V) const { return Vec2<t>(x+V.x, y+V.y); }
	inline Vec2<t> operator -(const Vec2<t> &V) const { return Vec2<t>(x-V.x, y-V.y); }
	inline Vec2<t> operator *(float f)          const { return Vec2<t>(x*f, y*f); }
	t & operator [](const int i){ if(x<=0) return x; else return y;}

    template <class > friend std::ostream& operator<<(std::ostream& s, Vec2<t>& v);
};

template <class t> struct Vec3 {
	t x,y,z;

	Vec3<t>() : x(t()), y(t()), z(t()) {}
	Vec3<t>(t _x, t _y, t _z) : x(_x),y(_y),z(_z) {}
	Vec3<t>(Matrix m);

	template <class u> Vec3<t>(const Vec3<u> &v);
	Vec3<t>(t* array):x(array[0]),y(array[1]),z(array[2]){}
	Vec3<t>(const Vec3<t> &v): x(t()),y(t()),z(t()){ *this=v; }
	Vec3<t> & operator =(const Vec3<t> &v){
		if(this != &v){
			x = v.x;
			y = v.y;
			z = v.z;
		}
		return *this;
	}
	
	inline Vec3<t> operator +(const Vec3<t> &v) const { return Vec3<t>(x+v.x, y+v.y, z+v.z); }
	inline Vec3<t> operator -(const Vec3<t> &v) const { return Vec3<t>(x-v.x, y-v.y, z-v.z); }
	inline Vec3<t> operator *(float f)          const { return Vec3<t>(x*f, y*f, z*f); }
	inline t       operator *(const Vec3<t> &v) const { return x*v.x + y*v.y + z*v.z; }
	float norm () const { return std::sqrt(x*x+y*y+z*z); }
	Vec3<t> & normalize(t l=1) { *this = (*this)*(l/norm()); return *this; }
	inline Vec3<t> crossproduct(const Vec3<t> &v){
		return Vec3<t>(y*v.x-z*v.y, -(x*v.z-z*v.x), x*v.y-y*v.x);
	}
	t& operator [](const int i){
		if(i<=0) return x;
		else if(i==1) return y;
		else return z;
	}

	template <class > friend std::ostream& operator<<(std::ostream& s, Vec3<t>& v);
};

typedef Vec2<float> Vec2f;
typedef Vec2<int>   Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int>   Vec3i;

//这两句完全不知道为什么，它声明了两个函数，但是却写了两个template
template <> template <> Vec3<int>::Vec3(const Vec3<float> &v);
template <> template <> Vec3<float>::Vec3(const Vec3<int> &v);

template <class t> std::ostream& operator<<(std::ostream& s, Vec2<t>& v) {
	s << "(" << v.x << ", " << v.y << ")\n";
	return s;
}

template <class t> std::ostream& operator<<(std::ostream& s, Vec3<t>& v) {
	s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
	return s;
}

/////////////////////2023-5-15///////////////////////
//perspective transformation：实现一个矩阵运算库
const int DEFAULT_ALLOC = 4;
class Matrix{
	std::vector<std::vector<float>> m;
	int rows, cols;
public:
	Matrix(int r=DEFAULT_ALLOC, int c=DEFAULT_ALLOC);
	inline int nrows(){return rows;}
	inline int ncols(){return cols;}

	static Matrix identity(int dimension); //生成单位矩阵
	std::vector<float>& operator [](const int i);
	Matrix operator *(const Matrix& a);
	Matrix transpose();
	Matrix inverse();

	friend std::ostream& operator<<(std::ostream& s, Matrix& m);
};

#endif