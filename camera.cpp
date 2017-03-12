//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//

#include <physics/ray.hpp>
#include <camera.hpp>
#include <scene.hpp>
#include <rtr_config.hpp>

#include <ImfRgba.h>
#include <gil_extension/exr/exr_io.hpp>
#include <materials/shading_ctx.hpp>

template <class>
struct print;

#if RTR_SPDLOG_SUPPORT
#include <spdlog/spdlog.h>
#endif

template<class> struct print;

namespace rtr
{
    typename render_config::render_traits<camera::render_type>::image_type
    camera::render(const scene& scene) const
    {
        static auto id = 0;
#if RTR_SPDLOG_SUPPORT
        auto logger = spdlog::stderr_logger_st("camera " + std::to_string(++id));
        logger->info("Rendering with configuration \"{0}\"", render_type::name);
        logger->info("Output file: {0}", m_output);
#endif

        using namespace physics;

        const auto one_down = - plane.pix_h * t.up;
        const auto one_right = plane.pix_w * t.right;
        glm::vec3 pix_pos = plane.get_top_left(t) + 0.5f * plane.pix_w * t.right - 0.5f * plane.pix_h * t.up;

        using im_type = typename rtr::render_config::render_traits<render_type>::image_type;
        using pix_type = im_type::value_type;
        using c_type = boost::gil::channel_type<pix_type>::type;

        im_type img(plane.width, plane.height);
        auto v = view(img);

        logger->info("Image Size: ({0}, {1})", plane.width, plane.height);

        auto begin = std::chrono::high_resolution_clock::now();

        for (int row = 0; row<plane.height; ++row) {
            auto row_pos = pix_pos;
            for (int col = 0; col<plane.width; ++col) {
                ray r (t.position, glm::normalize(row_pos - t.position));

                auto res = scene.ray_cast(r);
                if (res)
                {
                    const auto& c = render_type::process(res->mat->shade(shading_ctx{scene, -r.dir, *res}));
                    v(col, row) = pix_type(c_type(c[0]), c_type(c[1]), c_type(c[2]));
                }
                else
                {
                    const auto& c = render_type::process(scene.m_background);
                    v(col, row) = pix_type(c_type(c[0]), c_type(c[1]), c_type(c[2]));
                }

                row_pos += one_right;
            }
            pix_pos += one_down;
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

        logger->info("Rendering took {0} seconds", millis / 1000.f);

        return img;
    }
}

