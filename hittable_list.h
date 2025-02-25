#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H
//允许将多个物体组合成一个列表进行射线检测。

#include "hittable.h"

#include <memory>
#include <vector>

using std::make_shared;
using std::shared_ptr;

class hittable_list : public hittable {
  public:
    std::vector<shared_ptr<hittable>> objects;
    //用一个objects（std::vector）存储所有可以被射线击中的物体。每个物体都是shared_ptr<hittable>类型，使用shared_ptr管理内存可以避免手动释放内存的问题。

    hittable_list() {}
    hittable_list(shared_ptr<hittable> object) { add(object); }

    void clear() { objects.clear(); }//清空存储的物体

    void add(shared_ptr<hittable> object) {
        objects.push_back(object);
    }//添加对象到objects
    //？如何控制个数？如何存储类型

    bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const override {
        hit_record temp_rec;//保存每次与物体相交的交点信息
        bool hit_anything = false;//是否被击中
        auto closest_so_far = ray_tmax;//记录最远物体的t，用以控制只记录最近物体相交的t值

        for (const auto& object : objects) {
            if (object->hit(r, ray_tmin, closest_so_far, temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }
    //每次检查 objects 中的物体是否与射线相交。
    //如果物体相交，并且交点距离射线起点更近，则更新 closest_so_far 和 rec，记录新的交点信息。
    //如果有物体相交，hit_anything 被设置为 true。
    //最终，函数返回 hit_anything，表示是否有物体被射线击中。
};

#endif