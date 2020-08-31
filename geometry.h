#ifndef GEOMETRY_H_INCLUDED
#define GEOMETRY_H_INCLUDED

#include<cmath>

template <typename T>
class Vec2{
public:
    union{
        struct{T x, y;};
        struct{T u, v;};
    };
    Vec2():x(0), y(0) {};
    Vec2(T x, T y):x(x), y(y) {};

    inline const Vec2<T> operator+(const Vec2 v)const {
        return Vec2<T>(x+v.x, y+v.y);
    }

    inline const Vec2<T> operator-(const Vec2 v)const {
        return Vec2<T>(x-v.x, y-v.y);
    }

    inline const T operator*(const Vec2 v)const {
        return x*v.x + y*v.y;
    }

    inline const Vec2<T> operator*(const T v)const {
        return Vec2<T>(x*v, y*v);
    }

    inline const T normal() {
        return sqrt(x*x + y*y);
    }

    inline const Vec2<T> normalize() {
        T n = this.normal();
        *this = Vec2<T>(x/n, y/n);
        return *this;
    }
};

template <typename T>
class Vec3{
public:
    union{
        struct{T x, y, z;};
        struct{T r, g, b;};
    };
    Vec3():x(0), y(0), z(0){};
    Vec3(T x, T y, T z):x(x), y(y), z(z) {};

    inline const Vec3<T> operator+(const Vec3 v)const {
        return Vec3<T>(x+v.x, y+v.y, z+v.z);
    }

    inline const Vec3<T> operator-(const Vec3 v)const {
        return Vec3<T>(x-v.x, y-v.y, z-v.z);
    }

    inline const T operator*(const Vec3 v)const {
        return x*v.x + y*v.y + z*v.z;
    }

    inline const Vec3<T> operator*(const T v)const {
        return Vec3<T>(x*v, y*v, z*v);
    }

    inline const Vec3<T> operator^(const Vec3 v)const {
        return Vec3<T>(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x);
    }

    inline const T normal() {
        return sqrt(x*x + y*y + z*z);
    }

    inline const Vec3<T> normalize() {
        T n = this->normal();
        *this = Vec3<T>(x/n, y/n, z/n);
        return *this;
    }
};

typedef Vec2<int> Vec2i;
typedef Vec2<float> Vec2f;
typedef Vec3<int> Vec3i;
typedef Vec3<float> Vec3f;

#endif // GEOMETRY_H_INCLUDED
