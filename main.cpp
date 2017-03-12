#include <iostream>

#include <lights.hpp>
#include <shapes.hpp>

#include <physics/ray.hpp>

#include <glm/glm.hpp>
#include <vector>

#include <scene.hpp>
#include <camera.hpp>

#include <materials/rt_mat.hpp>

#include <xml_parse.hpp>
#include "rtr_config.hpp"
#include "utility.hpp"

#if RTR_OPENEXR_SUPPORT
    #include <gil_extension/exr/exr_io.hpp>
#endif

#if RTR_PNG_SUPPORT
    #define int_p_NULL nullptr
    #include <boost/gil/extension/io/png_io.hpp>
#endif

#if RTR_SPDLOG_SUPPORT
    #include <spdlog/spdlog.h>
#endif

static constexpr const char* build_types[] = {
    "Debug",
    "Performance"
};

#include <boost/program_options.hpp>

int main(int ac, char** av)
{
    auto logger = spdlog::stderr_logger_st("general");
    logger->info("rtr version {0}.{1}.{2}", RTR_VERSION_MAJOR, RTR_VERSION_MINOR, RTR_VERSION_PATCH);
    logger->info("Build Type: {0}", build_types[RTR_BUILD_TYPE]);

    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("stdin", "read scene from standard input")
            ("scene-file,I", po::value<std::string>(), "read scene from file")
            ;

    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }

    std::string scene_file;
    if (vm.count("stdin")) {
        logger->info("Reading input from the standard input...");

        std::cin >> std::noskipws;

        std::istream_iterator<char> it(std::cin);
        std::istream_iterator<char> end;
        scene_file = std::string(it, end);
    } else if (vm.count("scene-file")) {
        auto f_name = vm["scene-file"].as<std::string>();
        std::ifstream file(f_name);
        file >> std::noskipws;
        std::istream_iterator<char> it(file);
        std::istream_iterator<char> end;
        scene_file = std::string(it, end);
    }

    auto r = rtr::xml::read_scene(scene_file);
    r.first.finalize();

    auto writer = make_lambda_visitor<void>(
        [](boost::gil::rgb16f_image_t& img, const std::string& output)
        {
            boost::gil::exr_write_view(output.c_str(), boost::gil::view(img));
        },
        [](boost::gil::rgb8_image_t& img, const std::string& output)
        {
            boost::gil::png_write_view(output.c_str(), boost::gil::view(img));
        }
    );

    for (auto& cam : r.second)
    {
        auto ima = cam.render(r.first);
        writer(ima, cam.get_output());
    }
}