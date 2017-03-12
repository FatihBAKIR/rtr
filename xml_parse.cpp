//
// Created by Mehmet Fatih BAKIR on 05/03/2017.
//

#include <lights.hpp>
#include <shapes.hpp>

#include <xml_parse.hpp>
#include <glm/vec3.hpp>
#include <sstream>
#include <camera.hpp>
#include <scene.hpp>
#include <iostream>
#include <vector>

#include <materials/rt_mat.hpp>
#include <tinyxml2.h>
#include <unordered_map>
#include <materials/normal_mat.hpp>
#include <materials/material.hpp>

namespace xml = tinyxml2;
namespace
{

rtr::camera read_camera(const xml::XMLElement* elem)
{
    auto get_text = [&](const char* name) {
        return elem->FirstChildElement(name)->GetText();
    };

    glm::vec3 pos, up, gaze;

    std::istringstream iss(get_text("Position"));
    iss >> pos[0] >> pos[1] >> pos[2];

    iss = std::istringstream(get_text("Up"));
    iss >> up[0] >> up[1] >> up[2];

    iss = std::istringstream(get_text("Gaze"));
    iss >> gaze[0] >> gaze[1] >> gaze[2];

    float left, right, top, bottom;
    float dist;
    long width, height;

    iss = std::istringstream(get_text("NearPlane"));
    iss >> left >> right >> bottom >> top;

    dist = elem->FirstChildElement("NearDistance")->FloatText(0);

    iss = std::istringstream(get_text("ImageResolution"));
    iss >> width >> height;

    rtr::im_plane plane{left, right, top, bottom, dist, width, height,};
    return {pos, up, gaze, plane, elem->FirstChildElement("ImageName")->GetText()};
}

rtr::shapes::sphere read_sphere(const xml::XMLElement* elem, gsl::span<glm::vec3> verts,
        const std::unordered_map<long, rtr::material*>& mats)
{
    long mat_id;
    long vert_id;
    float radius;

    radius = elem->FirstChildElement("Radius")->FloatText();
    vert_id = elem->FirstChildElement("Center")->Int64Text();
    mat_id = elem->FirstChildElement("Material")->Int64Text();

    auto mat_it = mats.find(mat_id);
    return rtr::shapes::sphere(verts[vert_id], radius, mat_it->second);
}

rtr::shapes::mesh
read_mesh(const xml::XMLElement* elem, gsl::span<glm::vec3> verts, const std::unordered_map<long, rtr::material*>& mats)
{
    long mat_id;

    mat_id = elem->FirstChildElement("Material")->Int64Text();

    boost::container::vector<long> face_indices;
    boost::container::vector<rtr::shapes::triangle> faces;

    auto vert_text = elem->FirstChildElement("Faces")->GetText();
    std::istringstream iss(vert_text);
    for (std::array<long, 3> v; iss >> v[0] >> v[1] >> v[2];) {
        face_indices.push_back(v[0]);
        face_indices.push_back(v[1]);
        face_indices.push_back(v[2]);

        faces.emplace_back(std::array<glm::vec3, 3>{verts[v[0]], verts[v[1]], verts[v[2]]});
    }

    auto mat_it = mats.find(mat_id);
    rtr::shapes::mesh m {std::move(faces), std::move(face_indices), mat_it->second};
    if (elem->Attribute("shadingMode") && elem->Attribute("shadingMode") == std::string("smooth"))
    {
        m.smooth_normals();
    }
    return m;
}

void read_objects(const xml::XMLElement* elem, gsl::span<glm::vec3> verts,
        const std::unordered_map<long, rtr::material*>& mats, rtr::scene& sc)
{
    glm::vec3 scene_min, scene_max;
    float max_radius = 0;

    for (auto s = elem->FirstChildElement(); s; s = s->NextSiblingElement()) {
        if (s->Name()==std::string("Mesh") || s->Name()==std::string("Triangle")) {
            sc.insert(read_mesh(s, verts, mats));
        }
        else if (s->Name()==std::string("Sphere")) {
            auto&& sphere = read_sphere(s, verts, mats);
            if (sphere.get_radius()>max_radius) {
                auto growth = sphere.get_radius()-max_radius;
                max_radius = sphere.get_radius();
                sc.resize(sc.get_box().position, sc.get_box().extent+glm::vec3(growth, growth, growth)*2.f);
            }
            sc.insert(sphere);
        }
        else {
            abort();
        }
    }
}

rtr::lights::ambient_light read_ambient(const xml::XMLElement* elem)
{
    std::istringstream iss(elem->GetText());
    glm::vec3 color;
    iss >> color[0] >> color[1] >> color[2];
    return {color};
}

rtr::lights::point_light read_point(const xml::XMLElement* elem)
{
    glm::vec3 pos, inte;

    std::istringstream iss(elem->FirstChildElement("Position")->GetText());
    iss >> pos[0] >> pos[1] >> pos[2];

    iss = std::istringstream(elem->FirstChildElement("Intensity")->GetText());
    iss >> inte[0] >> inte[1] >> inte[2];

    return {pos, inte};
}

void read_lights(const xml::XMLElement* elem, rtr::scene& sc)
{
    for (auto l = elem->FirstChildElement(); l; l = l->NextSiblingElement()) {
        if (l->Name()==std::string("AmbientLight")) {
            sc.insert(read_ambient(l));
        }
        else if (l->Name()==std::string("PointLight")) {
            sc.insert(read_point(l));
        }
        else {
            abort();
        }
    }
}

rtr::rt_mat read_rt_material(const xml::XMLElement* elem)
{
    auto get_text = [&](const char* name) {
        return elem->FirstChildElement(name)->GetText();
    };

    rtr::rt_mat m;

    m.id = elem->Int64Attribute("id");

    std::istringstream iss(get_text("AmbientReflectance"));
    iss >> m.ambient[0] >> m.ambient[1] >> m.ambient[2];

    iss = std::istringstream(get_text("DiffuseReflectance"));
    iss >> m.diffuse[0] >> m.diffuse[1] >> m.diffuse[2];

    iss = std::istringstream(get_text("SpecularReflectance"));
    iss >> m.specular[0] >> m.specular[1] >> m.specular[2];

    m.phong = elem->FirstChildElement("PhongExponent")->FloatText();

    return m;
}
    rtr::material* read_material(const xml::XMLElement* elem)
    {
        if (elem->Attribute("shader") == nullptr)
        {
            return new rtr::rt_mat(read_rt_material(elem));
        }
        else if (elem->Attribute("shader") == "normal_mat")
        {
            auto m = new rtr::normal_mat;
            m->id = elem->Int64Attribute("id");
            return m;
        }
    }

}

namespace rtr {
    namespace xml {
        std::pair<rtr::scene, std::vector<rtr::camera>> read_scene(const std::string& path)
        {
            namespace xml = tinyxml2;

            xml::XMLDocument doc;
            doc.LoadFile(path.c_str());

            glm::vec3 bg;
            float ray_epsilon = 0.001, intersect_epsilon = 0.001;

            auto root = doc.FirstChildElement("Scene");
            std::istringstream iss {root->FirstChildElement("BackgroundColor")->GetText()};
            iss >> bg[0] >> bg[1] >> bg[2];

            if (root->FirstChildElement("ShadowRayEpsilon")) {
                ray_epsilon = root->FirstChildElement("ShadowRayEpsilon")->FloatText(0);
            }
            if (root->FirstChildElement("IntersectionTestEpsilon")) {
                intersect_epsilon = root->FirstChildElement("IntersectionTestEpsilon")->FloatText(0);
            }

            std::vector<rtr::camera> cams;
            auto cameras = root->FirstChildElement("Cameras");
            for (auto c = cameras->FirstChildElement(); c; c = c->NextSiblingElement()) {
                cams.push_back(read_camera(c));
            }

            std::unordered_map<long, rtr::material*> mats;
            auto materials = root->FirstChildElement("Materials");
            for (auto c = materials->FirstChildElement(); c; c = c->NextSiblingElement()) {
                auto m = read_material(c);
                mats.emplace(m->id, m);
            }

            constexpr auto inf = std::numeric_limits<float>::infinity();

            glm::vec3 min = {inf, inf, inf}, max = {-inf, -inf, -inf};

            auto up_min_max = [&](const glm::vec3& vert) {
                for (int j = 0; j<3; ++j) {
                    if (min[j]>vert[j]) {
                        min[j] = vert[j];
                    }
                    if (max[j]<vert[j]) {
                        max[j] = vert[j];
                    }
                }
            };

            std::vector<glm::vec3> vert_pos(1);
            auto vert_text = root->FirstChildElement("VertexData")->GetText();
            iss = std::istringstream(vert_text);
            for (glm::vec3 v; iss >> v[0] >> v[1] >> v[2];) {
//                v = transform::translate(glm::vec3{0.1, 0, 0}) * transform::rotate(45, glm::vec3{0, 1, 0}) * transform::scale(glm::vec3{1.5, 1.5, 1.5}) * glm::vec4(v, 1);
                vert_pos.push_back(v);
                up_min_max(v);
            }

            glm::vec3 center = (min+max)*0.5f;
            glm::vec3 ext = max-min;

            rtr::scene s{center, ext, std::move(mats)};

            s.m_shadow_epsilon = ray_epsilon;
            s.m_test_epsilon = intersect_epsilon;
            s.m_background = bg;

            auto objs_root = root->FirstChildElement("Objects");
            auto lights = root->FirstChildElement("Lights");

            read_objects(objs_root, vert_pos, s.materials(), s);
            read_lights(lights, s);

            return std::make_pair(std::move(s), std::move(cams));
        }
    }
}

