//
// Created by fatih on 23.03.2017.
//

#ifndef RTR_ASSIMP_IMPORT_HPP
#define RTR_ASSIMP_IMPORT_HPP

#include <rtr_fwd.hpp>
#include <vector>
#include <string>
#include <scene.hpp>

namespace rtr
{
    namespace assimp
    {
        std::pair<rtr::scene, std::vector<rtr::camera>> read_scene(const std::string& path);
    }
}


#endif //RTR_ASSIMP_IMPORT_HPP
