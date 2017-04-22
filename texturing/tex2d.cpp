//
// Created by fatih on 4/21/17.
//

#include "tex2d.hpp"

namespace rtr
{
namespace texturing
{
    template <class channel_t, int num_channels>
    tex2d<channel_t, num_channels>::tex2d(const channel_t* data, int width, int height, float scaling)
    {
        m_data = std::make_unique<channel_t[]>(width * height * num_channels);
        w = width;
        h = height;
        this->scaling = scaling;
        std::copy(data, data + w * h * num_channels, m_data.get());
    }

    template <class channel_t, int num_channels>
    glm::vec3 tex2d<channel_t, num_channels>::sample(int x, int y) const
    {
        x %= w;
        y %= h;
        auto r = m_data[(y * w + x) * num_channels];
        auto g = m_data[(y * w + x) * num_channels + 1];
        auto b = m_data[(y * w + x) * num_channels + 2];
        return glm::vec3(r, g, b) / scaling;
    }

    template <class channel_t, int num_channels>
    glm::vec3 tex2d<channel_t, num_channels>::sample(const glm::vec2& uv_o) const
    {
        auto uv = glm::vec2(std::fmod(uv_o.x, 1.f), std::fmod(uv_o.y, 1.f));
        if (uv.x < 0) uv.x += 1.f;
        if (uv.y < 0) uv.y += 1.f;

        switch (mode)
        {
        case sampling_mode::nearest_neighbour:
            return sample(std::round(uv.x * w), std::round(uv.y * h));
        case sampling_mode::bilinear:
            return {};
        }
    }

    template <class channel_t, int num_channels>
    void tex2d<channel_t, num_channels>::set_sampling_mode(sampling_mode mode)
    {
        this->mode = mode;
    }

    template <class channel_t, int num_channels>
    tex2d<channel_t, num_channels>::~tex2d() = default;

    template class tex2d<std::uint8_t, 3>;
    template class tex2d<float, 3>;
}
}