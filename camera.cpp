//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//

#include <physics/ray.hpp>
#include <camera.hpp>
#include <scene.hpp>
#include <rtr_config.hpp>

#include <ImfRgba.h>
#include <gil_extension/exr/exr_io.hpp>


boost::gil::rgb16f_image_t rtr::camera::render(const scene& scene) const
{
    using namespace physics;
    using namespace boost::gil;

    const auto one_down = - plane.pix_h * t.up;
    const auto one_right = plane.pix_w * t.right;
    glm::vec3 pix_pos = plane.get_top_left(t) + 0.5f * plane.pix_w * t.right - 0.5f * plane.pix_h * t.up;

    rgb16f_image_t img(plane.width, plane.height);
    rgb16f_view_t v = view(img);

    for (int row = 0; row<plane.height; ++row) {
        auto row_pos = pix_pos;
        for (int col = 0; col<plane.width; ++col) {
            ray r (t.position, glm::normalize(row_pos - t.position));

            auto res = scene.ray_cast(r);
            if (res)
            {
                const auto& c = res->mat->calculate_color(scene, -r.dir, res->position, res->normal);
                v(col, row) = rgb16f_pixel_t(half(c[0]), half(c[1]), half(c[2]));
            }

            row_pos += one_right;
        }
        pix_pos += one_down;
    }

    return img;
}
