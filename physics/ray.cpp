//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//

#include <physics/ray.hpp>
#include <ostream>


namespace rtr
{
    namespace physics
    {
        static std::ostream& operator<<(std::ostream& os, const glm::vec3& v)
        {
            return os << '(' << v.x << ", " << v.y << ", " << v.z << ')';
        }
        std::ostream& operator<<(std::ostream& os, const ray& r)
        {
            return os << "{ " << r.origin << ", " << r.dir << " }";
        }
    }
}
