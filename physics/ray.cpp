//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//

#include <physics/ray.hpp>
#include <ostream>
#include <utility.hpp>

namespace rtr
{
    namespace physics
    {
        std::ostream& operator<<(std::ostream& os, const ray& r)
        {
            return os << "{ " << r.origin << ", " << r.dir << " }";
        }
    }
}
