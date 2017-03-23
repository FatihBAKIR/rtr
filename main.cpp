#include <iostream>

#include <lights.hpp>
#include <geometry.hpp>

#include <physics/ray.hpp>

#include <glm/glm.hpp>
#include <vector>
#include <fstream>

#include <scene.hpp>
#include <camera.hpp>

#include <materials/rt_mat.hpp>

#include <xml_parse.hpp>
#include "rtr_config.hpp"
#include "utility.hpp"
#include "assimp_import.hpp"

#if RTR_SPDLOG_SUPPORT
    #include <spdlog/spdlog.h>
#endif

#include <boost/program_options.hpp>

int main(int ac, char** av)
{
    auto logger = spdlog::stderr_logger_st("general");
    logger->info("rtr version {0}", rtr::config::version_str);
    logger->info("Build Type: {0}", rtr::config::build_type);
    logger->info("rtr threading support: {0}", rtr::config::thread_support);

    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("stdin", "read scene from standard input")
            ("file", po::value<std::string>()->default_value(""), "read scene from file")
            ("assimp", po::value<std::string>()->default_value(""), "read scene from file")
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
    } else if (vm.count("file")) {
        auto f_name = vm["file"].as<std::string>();
        logger->info("Reading input from file {0}", f_name);
        std::ifstream file(f_name);
        file >> std::noskipws;
        std::istream_iterator<char> it(file);
        std::istream_iterator<char> end;
        scene_file = std::string(it, end);
    }

    auto r = rtr::assimp::read_scene(vm["file"].as<std::string>());
    r.first.finalize();

    auto writer = make_lambda_visitor<void>(
#if RTR_OPENEXR_SUPPORT
        [](boost::gil::rgb16f_image_t& img, const std::string& output)
        {
            boost::gil::exr_write_view(output.c_str(), boost::gil::view(img));
        },
#endif
#if RTR_PNG_SUPPORT
        [](boost::gil::rgb8_image_t& img, const std::string& output)
        {
            boost::gil::png_write_view(output.c_str(), boost::gil::view(img));
        }
#endif
    );

    for (auto& cam : r.second)
    {
        auto ima = cam.render(r.first);
        writer(ima, cam.get_output());
    }
}