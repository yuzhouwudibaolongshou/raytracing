#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"

class material {
  public:
    virtual ~material() = default;

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered  
    ) const {
        return false;
    }
};
/*const xx&是常引用，可以在小函数中完成对大函数的常数的访问，但不能进行任何修改，如果传比较复杂的类型，需要用到传常引用
  xx 传值会先复制一份，此时可以对其中的数据进行修改，但不会修改调用该函数的函数中的变量值，一般用于简单的类型
  xx& 传引用，允许调用函数时修改外层函数的变量，当需要多个返回值的时候，一般使用该方式。比如上述函数不但返回了是否散射，也返回了散射视线与反射率*/

class lambertian : public material {
  public:
    lambertian(const color& albedo) : albedo(albedo) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
    const override {
        auto scatter_direction = rec.normal + random_unit_vector();

        // Catch degenerate scatter direction
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

  private:
    color albedo;
};

class metal : public material {
  public:
    //metal(const color& albedo) : albedo(albedo) {}
    metal(const color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}//毛金属材质

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
    const override {
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        reflected = unit_vector(reflected) + (fuzz * random_unit_vector());//毛金属随机散射
        scattered = ray(rec.p, reflected);
        attenuation = albedo;
        //return true;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

  private:
    color albedo;
    double fuzz;
};

class dielectric : public material {
  public:
    dielectric(double refraction_index) : refraction_index(refraction_index) {}//？

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
    const override {
        attenuation = color(1.0, 1.0, 1.0);
        double ri = rec.front_face ? (1.0/refraction_index) : refraction_index;

        vec3 unit_direction = unit_vector(r_in.direction());
        //vec3 refracted = refract(unit_direction, rec.normal, ri);//这个步骤需要判定光线是否折射，因为材料的反射率与观察角度有关，即与ray和法线夹角有关。
        double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);

        bool cannot_refract = ri * sin_theta > 1.0;
        vec3 direction;

        if (cannot_refract || reflectance(cos_theta, ri) > random_double())
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, ri);

        //scattered = ray(rec.p, refracted);//那么原本的散射关系也需要变化
        scattered = ray(rec.p/*记录下的hit点*/, direction);

        
        return true;
    }

  private:
    // Refractive index in vacuum or air, or the ratio of the material's refractive index over
    // the refractive index of the enclosing media
    double refraction_index;

    static double reflectance(double cosine, double refraction_index) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0*r0;
        return r0 + (1-r0)*std::pow((1 - cosine),5);
    }//Schlick 近似
};

#endif