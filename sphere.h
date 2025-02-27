#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable/*继承，基于模板进行增改，或是采用不同的实现。public表示公用继承，保留其中的public和private。*/ {
  public:/*可以从外面访问的*/
    //sphere(const point3& center, double radius) : center(center), radius(std::fmax(0,radius)) {}
    //sphere(const point3& center, double radius) : center(center), radius(std::fmax(0,radius)) {}
        // TODO: Initialize the material pointer `mat`.
    
    sphere(const point3& center, double radius, shared_ptr<material> mat)
      : center(center), radius(std::fmax(0,radius)), mat(mat) {}//加入材质的初始化

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        vec3 oc = center - r.origin();
        auto a = r.direction().length_squared();
        auto h = dot(r.direction(), oc);
        auto c = oc.length_squared() - radius*radius;

        auto discriminant = h*h - a*c;
        if (discriminant < 0)
            return false;

        auto sqrtd = std::sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (h - sqrtd) / a;
        if (!ray_t.surrounds(root)) {
            root = (h + sqrtd) / a;
            if (!ray_t.surrounds(root))
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        vec3 outward_normal = (rec.p - center) / radius;//这个布尔值表示交点是否位于物体的“正面”。
        rec.set_face_normal(r, outward_normal);
        rec.mat = mat;

        return true;
    }
//protected可以用在公用继承后，自己可以访问，继承类可以访问，外部文件访问不了
  private:/*只在内部提供接口修改，无法在外部文件和继承类文件访问和修改*/
    point3 center;
    double radius;
    shared_ptr<material> mat;
};

#endif