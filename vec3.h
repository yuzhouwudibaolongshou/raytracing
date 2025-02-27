#ifndef VEC3_H
#define VEC3_H      //definition protection and global definition

#include <cmath>    //math function
#include <iostream> //in/out stream

class vec3 {
  public:
    double e[3];    

    vec3() : e{0,0,0} {}
    vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}
    //define a function: vec3(): use e[3] to regist three element (class double), then initialize e as {0,0,0}, and recieve e0 e1 e2 to refresh e.

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }
    //these three functions are used to access the components(分量) of e.
    //'const' means that the functions won't change variables inside e.

    vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
    //relog: negation operator: -(v)=vec3(-e0,-e1,-e2)
    double operator[](int i) const { return e[i]; }
    double& operator[](int i) { return e[i]; }
    //relog: [] to access or change the components of e

    vec3& operator+=(const vec3&/*取地址，*取指针值，同箭头的用法（解引用）*/ v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    vec3& operator*=(double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    vec3& operator/=(double t) {
        return *this *= 1/t;
    }
    //linear operation of vector.

    double length() const {
        return std::sqrt(length_squared());
    }

    double length_squared() const {
        return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
    }//模平方
    //Modulo arithmetic(取模运算)

    static vec3 random() {
        return vec3(random_double(), random_double(), random_double());
    }

    static vec3 random(double min, double max) {
        return vec3(random_double(min,max), random_double(min,max), random_double(min,max));
    }
};

// point3 is just an alias for vec3, but useful for geometric clarity in the code.
using point3 = vec3;
//use point to refer to a point in 3D space.

// Vector Utility Functions

inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}
//<<: output a element(class vec3) to std::ostream (such as std::cout).it outputs three components and aparts them with " ".

inline vec3 operator*(const vec3& u, const vec3& v) {
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}
// do * in every elements

inline vec3 operator+(const vec3& u, const vec3& v) {
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3& u, const vec3& v) {
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(double t, const vec3& v) {
    return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 operator*(const vec3& v, double t) {
    return t * v;
}

inline vec3 operator/(const vec3& v, double t) {
    return (1/t) * v;
}
//Four arithmetic(四则运算) for vec3 and vec3 or vec3 and number.


inline double dot(const vec3& u, const vec3& v) {
    return u.e[0] * v.e[0]
         + u.e[1] * v.e[1]
         + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3& u, const vec3& v) {
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}
//dot and corss arithmetic

inline vec3 unit_vector(const vec3& v) {
    return v / v.length();
}
//unit vector(lenth=1)

inline vec3 random_unit_vector() {
    while (true) {
        auto p = vec3::random(-1,1);
        auto lensq = p.length_squared();
        if (1e-160 < lensq && lensq <= 1)//防止浮点值过小导致的向量为0
            return p / sqrt(lensq);
    }
}

inline vec3 random_on_hemisphere(const vec3& normal) {
    vec3 on_unit_sphere = random_unit_vector();
    if (dot(on_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
        return on_unit_sphere;
    else
        return -on_unit_sphere;
}

#endif