#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include "interval.h"

#include <iostream>

using color = vec3;

inline double linear_to_gamma(double linear_component)//gamma转换使得颜色产生变化而变得“不准确暗”，这个函数使得颜色从线性空间转化到gamma空间时取了一个gamma空间到线性空间转换时采用的运算的逆运算，以此来抵消这部分变化。gamma空间存储有诸多好处，只是此处需要显示准确的颜色。
{
    if (linear_component > 0)
    {
        // std::cout << linear_component << ">0" << std::endl;
        return std::sqrt(linear_component);
    }
        
    // std::cout << linear_component << "<0" << std::endl;
    return 0;
}


void write_color(std::ostream& out, const color& pixel_color) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // std::cout << "A:" << r << " " << g << " " << b << std::endl;

    // Apply a linear to gamma transform for gamma 2
    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    // std::cout << "B:" << r << " " << g << " " << b << std::endl;

    // Translate the [0,1] component values to the byte range [0,255].
    static const interval intensity(0.000, 0.999);
    int rbyte = int(256 * intensity.clamp(r));
    int gbyte = int(256 * intensity.clamp(g));
    int bbyte = int(256 * intensity.clamp(b));

    // std::cout << "C:" << rbyte << " " << gbyte << " " << bbyte << std::endl;

    // Write out the pixel color components.
    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

#endif