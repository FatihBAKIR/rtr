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

#include <utility.hpp>

#if RTR_TBB_SUPPORT
#include <tbb/task_scheduler_init.h>
#include <tbb/task_group.h>
#include <tbb/parallel_do.h>
#include <random>

#endif

#include <vector>
#include <glm/glm.hpp>
#include <random>
#include <chrono>
#include <iostream>

#include <geometry.hpp>

namespace rtr {

    thread_local int max_ms;

    struct pix_iterator
            : public std::iterator<std::forward_iterator_tag, pix_iterator>
    {
        bool operator==(const pix_iterator& rhs) const
        {
            return pos == rhs.pos;
        }

        bool operator!=(const pix_iterator& rhs) const
        {
            return pos != rhs.pos;
        }

        pix_iterator& operator++()
        {
            pos++;
            pix_pos += one_right;
            return *this;
        }

        pix_iterator& operator*()
        {
            return *this;
        }

        long pos;
        glm::vec3 pix_pos;
        glm::vec3 one_right;
    };

    void render_scanline(const camera &cam, glm::vec3 row_pos, int row, const glm::vec3& one_down, const glm::vec3& one_right, const scene &scene,
                         typename render_config::render_traits<camera::render_type>::image_type::view_t &v) {
        using namespace physics;

        using im_type = typename rtr::render_config::render_traits<camera::render_type>::image_type;
        using pix_type = im_type::value_type;
        using c_type = boost::gil::channel_type<pix_type>::type;

        pix_iterator beg_i;
        beg_i.pos = 0;
        beg_i.pix_pos = row_pos;
        beg_i.one_right = one_right;

        pix_iterator end_i;
        end_i.pos = cam.plane.width;

        std::vector<std::uint16_t> ms_ids(cam.sample_count);
        std::iota(ms_ids.begin(), ms_ids.end(), 0);
        auto cam_ids = ms_ids;
        auto rng = std::mt19937(0);
        std::shuffle(ms_ids.begin(), ms_ids.end(), rng);
        std::shuffle(cam_ids.begin(), cam_ids.end(), rng);

        auto sample_right = one_right / (float)cam.sample_sqrt;
        auto sample_down = one_down / (float)cam.sample_sqrt;

        auto cam_right = cam.t.right * cam.aperture_size / (float)cam.sample_sqrt;
        auto cam_down = -cam.t.up * cam.aperture_size / (float)cam.sample_sqrt;

        auto cam_w = cam.aperture_size / cam.sample_sqrt;

        auto sample_w = cam.plane.pix_w / cam.sample_sqrt;
        auto sample_h = cam.plane.pix_h / cam.sample_sqrt;

        glm::vec3 cam_top_left = cam.t.position + (-(cam.t.right * cam.aperture_size) + (cam.t.up * cam.aperture_size)
                + cam_right + cam_down) * 0.5f;

        std::array<glm::vec3, 3> cam_basis = {cam_right, cam_down, {}};
        std::array<float, 3> cam_deviate = {cam_w / 2, cam_w / 2, 0};

        auto get_cam_pos = [&](int ms_id) -> glm::vec3
        {
            float x = ms_id % cam.sample_sqrt;
            float y = ms_id / cam.sample_sqrt;

            return cam_top_left + cam_right * x + cam_down * y;
        };

        bool is_mask = cam.m_flags.find("is_mask") != cam.m_flags.end();
        auto render_pix = [&](const pix_iterator& i)
        {
            if (is_mask)
            {
                ray r(cam.t.position, glm::normalize(i.pix_pos - cam.t.position));
                r.rtl = 1;
                r.ms_id = 0;
                r.m_backface_cull = true;

                auto res = scene.ray_cast(r);
                if (res)
                {
                    auto id = boost::apply_visitor([](auto obj)
                    {
                        return obj->get_id();
                    }, res->shape);

                    v(i.pos, row) = pix_type(id, 0, 0);
                }
                (*cam.rendered)++;
                return;
            }

            glm::vec3 top_left = i.pix_pos + (-one_right - one_down + sample_right + sample_down) * 0.5f;

            std::array<glm::vec3, 3> pix_basis = {sample_right, sample_down, {}};
            std::array<float, 3> pix_deviate = {sample_w / 2, sample_h / 2, 0};

            auto get_sample_pos = [&](int ms_id) -> glm::vec3
            {
                float x = ms_id % cam.sample_sqrt;
                float y = ms_id / cam.sample_sqrt;

                return top_left + sample_right * x + sample_down * y;
            };

            glm::vec3 fin_color = {};
            bool any_hit = false;

            for (int j = 0; j < cam.sample_count; ++j)
            {
                auto pos = rtr::random_point(get_cam_pos(cam_ids[j]), cam_basis, cam_deviate);
                ray r(pos, glm::normalize(rtr::random_point(get_sample_pos(ms_ids[j]), pix_basis, pix_deviate) - pos));
                r.rtl = scene.get_rtl();
                r.ms_id = j;
                r.m_backface_cull = true;

                auto res = scene.ray_cast(r);
                if (res)
                {
                    const auto &c = res->mat->shade(shading_ctx{scene, -r.dir, *res});
                    auto col = glm::min(c, 1000.f);
                    fin_color += col;
                    any_hit = true;
                }
            }

            if (any_hit) {
                const auto &c = camera::render_type::process(fin_color/ (float)cam.sample_count);
                v(i.pos, row) = pix_type(c_type(c[0]), c_type(c[1]), c_type(c[2]));
            } else {
                const auto &c = camera::render_type::process(scene.m_background);
                v(i.pos, row) = pix_type(c_type(c[0]), c_type(c[1]), c_type(c[2]));
            }

            (*cam.rendered)++;
        };

        std::for_each(beg_i, end_i, render_pix);
    }

    typename render_config::render_traits<camera::render_type>::image_type
    camera::render(const scene &scene) const {

        static auto id = 0;

        max_ms = this->sample_count;
        auto logger = spdlog::stderr_logger_st("camera " + std::to_string(++id));
        logger->info("Rendering with configuration \"{0}\"", render_type::name);
        logger->info("Output file: {0}", m_output);

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
        logger->info("Using TBB policy with {} threads", tbb::task_scheduler_init::default_num_threads());
#endif

        pix_iterator beg_i;
        beg_i.pos = 0;
        beg_i.pix_pos = plane.get_top_left(t) + 0.5f * one_right + 0.5f * one_down;
        beg_i.one_right = one_down;

        pix_iterator end_i;
        end_i.pos = plane.height;

        auto render_row = [&](const pix_iterator& row)
        {
            render_scanline(*this, row.pix_pos, row.pos, one_down, one_right, scene, v);
        };

        float pix_count = plane.width * plane.height;

        rendered->store(0);

        auto display_thread = std::thread([&]{
            float progress = 0;
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
            while (progress < 1.f)
            {
                auto end = std::chrono::high_resolution_clock::now();
                auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
                float total_time = millis / progress;
                int remaining = std::floor((total_time - millis) / 1000);

                progress = rendered->load() / pix_count;
                int barWidth = 70;
                std::cout << "[";
                int pos = barWidth * progress;
                for (int i = 0; i < barWidth; ++i) {
                    if (i < pos) std::cout << "=";
                    else if (i == pos) std::cout << ">";
                    else std::cout << " ";
                }
                std::cout << "] " << int(progress * 100.0) << "%, Remaining: " << remaining << "             \r";
                std::cout.flush();
                std::this_thread::sleep_for(std::chrono::milliseconds(16));
            }
        });

        for (auto& f : m_flags)
        {
            std::cout << f << '\n';
        }

#if RTR_TBB_SUPPORT && !RTR_NO_THREADING
        tbb::parallel_do(beg_i, end_i, render_row);
#else
        std::for_each(beg_i, end_i, render_row);
#endif

        display_thread.join();

        auto end = std::chrono::high_resolution_clock::now();
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

        logger->info("Rendering took {0} seconds", millis / 1000.f);

        return img;
    }

    void camera::set_aperture(float distance, float aperture_size)
    {
        this->aperture_size = aperture_size;
        float ratio = distance / plane.dist;

        std::cerr << "aperture camera: " << ratio << "\n";

        plane.dist *= ratio;
        plane.left *= ratio;
        plane.right *= ratio;
        plane.top *= ratio;
        plane.bottom *= ratio;

        plane.recalc();
    }

    void camera::set_samples(std::uint16_t samples)
    {
        sample_count = samples;
        sample_sqrt = std::sqrt(samples);
    }

    camera::camera(const glm::vec3& pos, const glm::vec3& up, const glm::vec3& gaze, const im_plane& p,
            const std::string& output)
            :
            t{pos, glm::normalize(up), glm::normalize(-gaze), glm::normalize(glm::cross(t.up, t.forward))}, plane{p}, m_output{output} {
        t.up = glm::normalize(glm::cross(t.right, -t.forward));
        rendered = new std::atomic<uint64_t>;
    }
}

