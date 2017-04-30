//
// Created by fatih on 4/21/17.
//

#include <texturing/sampler.hpp>
#include <memory>

namespace rtr
{
namespace texturing
{
    template <class channel_t, int num_channels>
    class tex2d : public sampler2d
    {
        std::unique_ptr<channel_t[]> m_data;
        int w, h;
        float scaling = 1;

        sampling_mode mode = sampling_mode::nearest_neighbour;

    public:

        tex2d(const channel_t*, int width, int height, float scaling = 1, sampling_mode m = sampling_mode::nearest_neighbour);
        ~tex2d() override;

        glm::vec3 sample(int x, int y) const;
        glm::vec3 sample(const glm::vec3& uv) const override;
        void set_sampling_mode(sampling_mode mode) override;

        int get_width() const { return w; }
        int get_height() const { return h; }
    };
}
}
