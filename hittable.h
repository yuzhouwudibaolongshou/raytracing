#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"

class hit_record {
  public:
    point3 p;
    vec3 normal;
    double t;
    bool front_face;

    void set_face_normal(const ray& r, const vec3& outward_normal) {
        // Sets the hit record normal vector.
        // NOTE: the parameter `outward_normal` is assumed to have unit length.

        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;//这个布尔值表示交点是否位于物体的“正面”。
    }
};

class hittable {
  public:
    virtual ~hittable() = default;//这是一个虚析构函数，确保在销毁派生类对象时正确调用析构函数，防止内存泄漏。

    virtual bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const = 0;
    //这是一个纯虚函数，所有继承自 hittable 类的派生类必须实现这个函数。

    //r：射线（ray）对象，表示当前要进行相交测试的射线。
    //ray_tmin 和 ray_tmax表示射线有效的 t 范围，即射线的最小和最大有效距离。通常用来避免不必要的计算，或者在场景中有多个物体时确保只与某些物体交互。
    //rec：是一个 hit_record 对象，用来保存交点信息。如果射线与物体相交，函数应该填写 rec，并返回 true；如果没有相交，返回 false。
};

#endif