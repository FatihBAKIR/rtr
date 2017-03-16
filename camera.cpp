//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//

#include <physics/ray.hpp>
#include <camera.hpp>
#include <scene.hpp>
#include <rtr_config.hpp>

#include <materials/shading_ctx.hpp>

#if RTR_SPDLOG_SUPPORT
#include <spdlog/spdlog.h>
#endif

#if RTR_TBB_SUPPORT
#include <tbb/task_scheduler_init.h>
#include <tbb/task_group.h>
#endif

namespace rtr {

#if RTR_TBB_SUPPORT
    struct tbb_threading_policy
    {
        tbb::task_scheduler_init init;
        tbb::task_group g;

        tbb_threading_policy() : init() {}

        template <class T>
        void run_task(const T& task)
        {
            g.run(task);
        }

        void converge()
        {
            g.wait();
        }
    };
#endif

    struct single_thread_policy
    {
        template <class T>
        void run_task(const T& task)
        {
            task();
        }
        void converge(){}
    };

    void render_scanline(const camera &cam, glm::vec3 row_pos, int row, const glm::vec3& one_right, const scene &scene,
                         typename render_config::render_traits<camera::render_type>::image_type::view_t &v) {
        using namespace physics;

        using im_type = typename rtr::render_config::render_traits<camera::render_type>::image_type;
        using pix_type = im_type::value_type;
        using c_type = boost::gil::channel_type<pix_type>::type;

        for (int col = 0; col < cam.plane.width; ++col) {
            ray r(cam.t.position, glm::normalize(row_pos - cam.t.position));

            auto res = scene.ray_cast(r);
            if (res) {
                const auto &c = camera::render_type::process(res->mat->shade(shading_ctx{scene, -r.dir, *res}));
                v(col, row) = pix_type(c_type(c[0]), c_type(c[1]), c_type(c[2]));
            } else {
                const auto &c = camera::render_type::process(scene.m_background);
                v(col, row) = pix_type(c_type(c[0]), c_type(c[1]), c_type(c[2]));
            }

            row_pos += one_right;
        }
    }

    typename render_config::render_traits<camera::render_type>::image_type
    camera::render(const scene &scene) const {
        static auto id = 0;

#if RTR_SPDLOG_SUPPORT
        auto logger = spdlog::stderr_logger_st("camera " + std::to_string(++id));
        logger->info("Rendering with configuration \"{0}\"", render_type::name);
        logger->info("Output file: {0}", m_output);
#endif

        using namespace physics;

        const auto one_down = -plane.pix_h * t.up;
        const auto one_right = plane.pix_w * t.right;

        using im_type = typename rtr::render_config::render_traits<render_type>::image_type;
        using pix_type = im_type::value_type;
        using c_type = boost::gil::channel_type<pix_type>::type;

        im_type img(plane.width, plane.height);
        auto v = view(img);

        logger->info("Image Size: ({0}, {1})", plane.width, plane.height);

        auto begin = std::chrono::high_resolution_clock::now();


#if RTR_TBB_SUPPORT && !RTR_NO_THREADING
        tbb_threading_policy render_policy;
        logger->info("Using TBB policy with {} threads", tbb::task_scheduler_init::default_num_threads());
#else
        single_thread_policy render_policy;
#endif

        glm::vec3 pix_pos = plane.get_top_left(t) + 0.5f * one_right + 0.5f * one_down;
        for (int row = 0; row < plane.height; ++row) {
            render_policy.run_task([pix_pos, this, row, &one_right, &scene, &v]{
                render_scanline(*this, pix_pos, row, one_right, scene, v);
            });
            pix_pos += one_down;
        }

        render_policy.converge();

        auto end = std::chrono::high_resolution_clock::now();
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

        logger->info("Rendering took {0} seconds", millis / 1000.f);

        return img;
    }
}

