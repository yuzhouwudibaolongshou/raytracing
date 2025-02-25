#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include "color.h"
#include "rtweekend.h"

#include <fstream>

class camera {
  public:
    // Image
    double aspect_ratio = 16.0 / 9.0;
    int image_width = 400;

    void render(const hittable& world) {
        initialize();

        std::ofstream ppm;
    ppm.open("./build/image6.0.ppm");//在第一次向文件输出之前打开文件
    ppm << "P3" << std::endl << image_width << ' ' << image_height << std::endl << "255" << std::endl;
    for (int j = 0; j < image_height; j++) {
        std::cout << "Scanlines remaining: " << (image_height - j) << std::endl;
        for (int i = 0; i < image_width; i++) {
            auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            auto ray_direction = pixel_center - center;
            ray r(center, ray_direction);

            color pixel_color = ray_color(r, world);
            write_color(ppm, pixel_color);
        }
    }

    std::cout << "Done." << std::endl;
    }

  private:
    int    image_height;   // Rendered image height
    point3 center;         // Camera center
    point3 pixel00_loc;    // Location of pixel 0, 0
    vec3   pixel_delta_u;  // Offset to pixel to the right
    vec3   pixel_delta_v;  // Offset to pixel below

    void initialize() {
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        center = point3(0, 0, 0);

        // Determine viewport dimensions.
        auto focal_length = 1.0;
        auto viewport_height = 2.0;
        auto viewport_width = viewport_height * (double(image_width)/image_height);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        auto viewport_u = vec3(viewport_width, 0, 0);
        auto viewport_v = vec3(0, -viewport_height, 0);

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left =
            center - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }

    color ray_color(const ray& r, const hittable& world/*这是一个抽象的接口，代表场景中的所有物体集合（hittable_list类）*/) {
    hit_record rec;//交点坐标、法向量normal、t值
    if (world.hit(r, interval(0, infinity), rec)/*指定t范围内判定是否相交的函数*/) {
        return 0.5 * (rec.normal/*单位法向量*/ + color(1,1,1));
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5*(unit_direction.y() + 1.0);
    return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
}

};

#endif
