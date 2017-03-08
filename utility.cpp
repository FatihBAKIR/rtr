//
// Created by Mehmet Fatih BAKIR on 08/03/2017.
//

#include <utility.hpp>
#include <ostream>


namespace glm
{
    std::ostream& operator<<(std::ostream& os, const vec3& v)
    {
        return os << '(' << v.x << ", " << v.y << ", " << v.z << ')';
    }
}
