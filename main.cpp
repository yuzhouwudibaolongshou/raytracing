#include "rtweekend.h"

#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "camera.h"

#include <fstream>

// 

int main() {
    hittable_list world;

    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    //auto material_left   = make_shared<metal>(color(0.8, 0.8, 0.8));
    //auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2));
    //auto material_left   = make_shared<dielectric>(1.50);//1.5表示refraction_index折射率之比，即从折射率为1的材质中射入为1.5的材质中
    //auto material_left   = make_shared<dielectric>(1.00 / 1.33);//此时为两个材料的折射率。这会被用来判断是否折射
    auto material_left   = make_shared<dielectric>(1.50);
    auto material_bubble = make_shared<dielectric>(1.00 / 1.50);//在玻璃球内部实现一个空气球，此时界面处的折射率之比为“1/1.5”
    auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);//加上了毛金属系数（可以理解为镜面反射与散射之比，算是这两者的“菲涅尔项”）

    //world.add(make_shared<sphere>(point3(0,0,-1), 0.5));
    //world.add(make_shared<sphere>(point3(0,-100.5,-1), 100));

    world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(point3( 0.0,    0.0, -1.2),   0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.4, material_bubble));
    world.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width  = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.render(world);
}