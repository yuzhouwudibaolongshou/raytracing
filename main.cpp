#include "rtweekend.h"

#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"

#include <fstream>

// 

color ray_color(const ray& r, const hittable& world/*这是一个抽象的接口，代表场景中的所有物体集合（hittable_list类）*/) {
    hit_record rec;//交点坐标、法向量normal、t值
    if (world.hit(r, 0, infinity, rec)/*指定t范围内判定是否相交的函数*/) {
        return 0.5 * (rec.normal/*单位法向量*/ + color(1,1,1));
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5*(unit_direction.y() + 1.0);
    return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
}

int main() {


    // Image

    auto aspect_ratio = 16.0 / 9.0;
    int image_width = 400;

    // Calculate the image height, and ensure that it's at least 1.
    int image_height = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    // World

    hittable_list world;

    world.add(make_shared<sphere>(point3(0,0,-1), 0.5));
    world.add(make_shared<sphere>(point3(0,-100.5,-1), 100));
    ////由于sphere是继承了hittable类型的，因此sphere类型也能实现定义了hittable的add函数。在这个过程中，它只被当初hittable用，但如果这个函数是在hittable中定义的虚函数，则sphere/triangle可以依照其定义的不同方式实现。

    // Camera

    auto focal_length = 1.0;
    auto viewport_height = 2.0;
    auto viewport_width = viewport_height * (double(image_width)/image_height);
    auto camera_center = point3(0, 0, 0);

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    auto viewport_u = vec3(viewport_width, 0, 0);
    auto viewport_v = vec3(0, -viewport_height, 0);

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    auto pixel_delta_u = viewport_u / image_width;
    auto pixel_delta_v = viewport_v / image_height;

    // Calculate the location of the upper left pixel.
    auto viewport_upper_left = camera_center
                             - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
    auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // Render

    //std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";
    std::ofstream ppm;
    ppm.open("./build/image3.0.ppm");//在第一次向文件输出之前打开文件
    ppm << "P3" << std::endl << image_width << ' ' << image_height << std::endl << "255" << std::endl;
    for (int j = 0; j < image_height; j++) {
        std::cout << "Scanlines remaining: " << (image_height - j) << std::endl;
        for (int i = 0; i < image_width; i++) {
            auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            auto ray_direction = pixel_center - camera_center;
            ray r(camera_center, ray_direction);

            color pixel_color = ray_color(r, world);
            write_color(ppm, pixel_color);
        }
    }

    std::cout << "Done." << std::endl;
}