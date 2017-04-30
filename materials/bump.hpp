//
// Created by fatih on 4/29/17.
//

#include "material.hpp"

namespace rtr
{
namespace shading
{
    class bump : public material
    {
        const material* base;
        const texturing::sampler2d* bump_map;

    public:

        bump(const material* b, const texturing::sampler2d* bm) :
            base(b), bump_map(bm) {}

        glm::vec3 shade(const shading_ctx& ctx) const override;
    };
}
}


