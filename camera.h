#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include "color.h"
#include "rtweekend.h"
#include "material.h"

#include <fstream>

class camera {
  public:
    // Image
    double  aspect_ratio = 16.0 / 9.0;
    int     image_width = 400;
    int     samples_per_pixel = 10;   // Count of random samples for each pixel
    int     max_depth         = 10;   // Maximum number of ray bounces into scene
    double  albedo            = 0.5;  // ρ

    double vfov = 90;  // Vertical view angle (field of view)视角
    point3 lookfrom = point3(0,0,0);   // Point camera is looking from
    point3 lookat   = point3(0,0,-1);  // Point camera is looking at
    vec3   vup      = vec3(0,1,0);     // Camera-relative "up" direction用来控制相机在uv平面内的旋转

    double defocus_angle = 0;  // Variation angle of rays through each pixel
    double focus_dist = 10;    // Distance from camera lookfrom point to plane of perfect focus//聚焦距离


    void render(const hittable& world) {
        initialize();

        std::ofstream ppm;
        ppm.open("./build/ball_glass_empty_blur.ppm");//在第一次向文件输出之前打开文件///////////////////////////////////////////////////////////////
        ppm << "P3" << std::endl << image_width << ' ' << image_height << std::endl << "255" << std::endl;
        for (int j = 0; j < image_height; j++) {
            std::cout << "Scanlines remaining: " << (image_height - j) << std::endl;
            for (int i = 0; i < image_width; i++) {
                color pixel_color(0,0,0);
                for (int sample = 0; sample < samples_per_pixel; sample++) {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }
                write_color(ppm, pixel_samples_scale * pixel_color);
        }
    }

    std::cout << "Done." << std::endl;
    }

  private:
    int    image_height;   // Rendered image height
    double pixel_samples_scale;  // Color scale factor for a sum of pixel samples
    point3 center;         // Camera center
    point3 pixel00_loc;    // Location of pixel 0, 0
    vec3   pixel_delta_u;  // Offset to pixel to the right
    vec3   pixel_delta_v;  // Offset to pixel below
    vec3   u, v, w;              // Camera frame basis vectors
    vec3   defocus_disk_u;       // Defocus disk horizontal radius
    vec3   defocus_disk_v;       // Defocus disk vertical radius

    void initialize() {
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        pixel_samples_scale = 1.0 / samples_per_pixel;

        //center = point3(0, 0, 0);
        center = lookfrom;

        // Determine viewport dimensions.
        //auto focal_length = 1.0;
        //auto focal_length = (lookfrom - lookat).length();

        //auto viewport_height = 2.0;
        //我们可以在任意距离构建viewport，只要保证其尺寸与相机的视角宽度成比例。
        auto theta = degrees_to_radians(vfov);
        auto h = std::tan(theta/2);
        //auto viewport_height = 2 * h * focal_length;
        auto viewport_height = 2 * h * focus_dist;//直接在最接近焦距的地方形成viewport
        auto viewport_width = viewport_height * (double(image_width)/image_height);

        // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
        w = unit_vector(lookfrom - lookat); //指向相机
        u = unit_vector(cross(vup, w));     //视角左右
        v = cross(w, u);                    //视角上下

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        //auto viewport_u = vec3(viewport_width, 0, 0);
        //auto viewport_v = vec3(0, -viewport_height, 0);
        vec3 viewport_u = viewport_width * u;    // Vector across viewport horizontal edge
        vec3 viewport_v = viewport_height * -v;  // Vector down viewport vertical edge
        //更新后的uv向量，此时不一定与xy平行

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        //auto viewport_upper_left =center - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
        //auto viewport_upper_left = center - (focal_length * w) - viewport_u/2 - viewport_v/2;//viewport左上角对应的xzy点
        auto viewport_upper_left = center - (focus_dist * w) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);//左上角第一个像素中心点

        // Calculate the camera defocus disk basis vectors.
        auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));//与viewport大小对应的散焦角内部的宽度
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }
    


    ray get_ray(int i, int j) const {
        // Construct a camera ray originating from the origin/ defocus disk and directed at randomly sampled point around the pixel location i, j.

        auto offset = sample_square();
        auto pixel_sample = pixel00_loc
                          + ((i + offset.x()) * pixel_delta_u)
                          + ((j + offset.y()) * pixel_delta_v);

        //auto ray_origin = center;
        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();//当我们需要景深时，就在相机附近的uv平面内取点发射光线，否则则只在相机处发射光线。
        auto ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    vec3 sample_square() const {
        // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    point3 defocus_disk_sample() const {
        // Returns a random point in the camera defocus disk.
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }
    
    color ray_color(const ray& r, int depth, const hittable& world/*这是一个抽象的接口，代表场景中的所有物体集合（hittable_list类）*/) {
        // If we've exceeded the ray bounce limit, no more light is gathered.
        if (depth <= 0)
            return color(0,0,0);
        
        hit_record rec;//交点坐标、法向量normal、t值

        if (world.hit(r, interval(0.001, infinity), rec)/*指定t范围内判定是否相交的函数*/) {
            //vec3 direction = random_on_hemisphere(rec.normal);
            //vec3 direction = rec.normal + random_unit_vector();//朗博反射
            //return albedo * ray_color(ray(rec.p, direction), depth-1, world);//产生灰色的效果（每经历一次反射则收集该点反射的一半光线，并且使得反弹次数-1）
            ray scattered;
            color attenuation;
            if (rec.mat->scatter(r, rec, attenuation, scattered))//？
                return attenuation * ray_color(scattered, depth-1, world);
            return color(0,0,0);
        }

        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5*(unit_direction.y() + 1.0);
        return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
}

};

#endif
