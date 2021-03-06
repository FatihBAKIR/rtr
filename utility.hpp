//
// Created by Mehmet Fatih BAKIR on 08/03/2017.
//

#pragma once

#include <glm/vec3.hpp>
#include <iosfwd>
#include <boost/variant/static_visitor.hpp>
#include <array>

namespace glm
{
    std::ostream& operator<<(std::ostream& os, const vec3& v);
}

template <typename ReturnType, typename... Lambdas>
struct lambda_visitor;

template <typename ReturnType, typename Lambda1, typename... Lambdas>
struct lambda_visitor< ReturnType, Lambda1 , Lambdas...>
        : public lambda_visitor<ReturnType, Lambdas...>, public Lambda1 {

    using Lambda1::operator();
    using lambda_visitor< ReturnType , Lambdas...>::operator();
    lambda_visitor(Lambda1 l1, Lambdas... lambdas)
            : Lambda1(l1), lambda_visitor< ReturnType , Lambdas...> (lambdas...)
    {}
};


template <typename ReturnType, typename Lambda1>
struct lambda_visitor<ReturnType, Lambda1>
        : public boost::static_visitor<ReturnType>, public Lambda1 {

    using Lambda1::operator();
    lambda_visitor(Lambda1 l1)
            : boost::static_visitor<ReturnType>(), Lambda1(l1)
    {}
};


template <typename ReturnType>
struct lambda_visitor<ReturnType>
        : public boost::static_visitor<ReturnType> {

    lambda_visitor() : boost::static_visitor<ReturnType>() {}
};

template <typename ReturnType, typename... Lambdas>
lambda_visitor<ReturnType, Lambdas...> make_lambda_visitor(Lambdas... lambdas) {
    return { lambdas... };
}

namespace rtr
{
    template <class T>
    auto lerp(const T& a, const T& b, float t)
    {
        return (1 - t) * a + t * b;
    }

    glm::vec3 random_point(const glm::vec3& around, const std::array<glm::vec3, 3>& a, const std::array<float, 3>& ranges);
    glm::vec3 get_normal(const glm::vec3& dir);

    glm::vec3 sample_hemisphere(int ms_id, int max_ms);
    glm::vec3 sample_hemisphere(const glm::vec3& towards,int ms_id, int max_ms);
}