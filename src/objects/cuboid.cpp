#include "cuboid.hh"

#include <algorithm>
#include <cstdlib>

#include "vector.hh"

namespace isim
{
    Vector3 Cuboid::getNormalAt(const Point3 &point) const
    {
        Point3 center;
        center.x = (max_point.x - min_point.x) / 2 + min_point.x;
        center.y = (max_point.y - min_point.y) / 2 + min_point.y;
        center.z = (max_point.z - min_point.z) / 2 + min_point.z;

        Vector3 V = point - center;
        Vector3 local_p = { V.x / (max_point.x - min_point.x) / 2,
                            V.y / (max_point.y - min_point.y) / 2,
                            V.z / (max_point.z - min_point.z) / 2 };

        float abs_x = std::abs(local_p.x);
        float abs_y = std::abs(local_p.y);
        float abs_z = std::abs(local_p.z);

        float max_composante = std::max({ abs_x, abs_y, abs_z });

        Vector3 normal = { 0, 0, 0 };

        if (max_composante == abs_x)
        {
            if (local_p.x > 0)
                normal.x = 1.0f;
            else
                normal.x = -1.0f;
        }
        else if (max_composante == abs_y)
        {
            if (local_p.y > 0)
                normal.y = 1.0f;
            else
                normal.y = -1.0f;
        }
        else
        {
            if (local_p.z > 0)
                normal.z = 1.0f;
            else
                normal.z = -1.0f;
        }

        return normal;
    }

    float Cuboid::intersect(const Ray &ray) const
    {
        // my implementation of the 'slabs algorithm'
        Vector3 inv_dir(1.0f / ray.direction.x, 1.0f / ray.direction.y,
                        1.0f / ray.direction.z);

        float t0x = (min_point.x - ray.origin.x) * inv_dir.x;
        float t1x = (max_point.x - ray.origin.x) * inv_dir.x;
        if (inv_dir.x < 0)
            std::swap(t0x, t1x);

        float t0y = (min_point.y - ray.origin.y) * inv_dir.y;
        float t1y = (max_point.y - ray.origin.y) * inv_dir.y;
        if (inv_dir.y < 0)
            std::swap(t0y, t1y);

        float t_min = std::max(t0x, t0y);
        float t_max = std::min(t1x, t1y);

        // no collision
        if (t_max <= t_min)
            return -1.0f;

        float t0z = (min_point.z - ray.origin.z) * inv_dir.z;
        float t1z = (max_point.z - ray.origin.z) * inv_dir.z;
        if (inv_dir.z < 0)
            std::swap(t0z, t1z);

        t_min = std::max(t_min, t0z);
        t_max = std::min(t_max, t1z);

        // final check
        if (t_max <= t_min)
            return -1.0f;

        // if hit, we return the distance t
        if (t_min > 0.001f)
        {
            return t_min;
        }
        else if (t_max > 0.001f)
        {
            return t_max;
        }

        return -1.0f;
    }

    Texture_properties
    Cuboid::getTextureCoordinatesAt(const Point3 &point) const
    {
        Vector3 normal = getNormalAt(point);
        float u, v;

        if (normal.x != 0)
        {
            u = (point.z - min_point.z) / (max_point.z - min_point.z);
            v = (point.y - min_point.y) / (max_point.y - min_point.y);

            if (normal.x < 0) // revert when neagtive to 'flip' the texture
                u = 1 - u;
        }
        else if (normal.y != 0)
        {
            u = (point.x - min_point.x) / (max_point.x - min_point.x);
            v = (point.z - min_point.z) / (max_point.z - min_point.z);

            if (normal.y < 0) // revert when neagtive to 'flip' the texture
                u = 1 - u;
        }
        else
        {
            u = (point.x - min_point.x) / (max_point.x - min_point.x);
            v = (point.y - min_point.y) / (max_point.y - min_point.y);

            if (normal.z < 0) // revert when neagtive to 'flip' the texture
                u = 1 - u;
        }

        // if no texture is loaded, we return the default material's properties
        if (face_materials.empty())
            return material->getElementsAt(u, v);
        else
        {
            // otherwise, we return the properties of the texture of the face
            // that was hit
            if (normal.x > 0)
                return face_materials[0].getElementsAt(u, v);
            else if (normal.x < 0)
                return face_materials[1].getElementsAt(u, v);
            else if (normal.y > 0)
                return face_materials[2].getElementsAt(u, v);
            else if (normal.y < 0)
                return face_materials[3].getElementsAt(u, v);
            else if (normal.z > 0)
                return face_materials[4].getElementsAt(u, v);
            else
                return face_materials[5].getElementsAt(u, v);
        }
    }

    void Cuboid::load_texture(const char *top_texture_filename,
                              const char *bottom_texture_filename,
                              const char *left_texture_filename,
                              const char *right_texture_filename,
                              const char *front_texture_filename,
                              const char *back_texture_filename)
    {
        face_materials.push_back(Image_texture_material(left_texture_filename));
        face_materials.push_back(
            Image_texture_material(right_texture_filename));
        face_materials.push_back(Image_texture_material(top_texture_filename));

        face_materials.push_back(
            Image_texture_material(bottom_texture_filename));
        face_materials.push_back(
            Image_texture_material(front_texture_filename));

        face_materials.push_back(Image_texture_material(back_texture_filename));
    }

    void Cuboid::load_texture(const char *top_texture_filename,
                              const char *bottom_texture_filename,
                              const char *side_texture_filename)
    {
        load_texture(top_texture_filename, bottom_texture_filename,
                     side_texture_filename, side_texture_filename,
                     side_texture_filename, side_texture_filename);
    }

} // namespace isim