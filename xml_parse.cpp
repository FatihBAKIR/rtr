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
#include <map>

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

rtr::shapes::sphere read_sphere(const xml::XMLElement* elem, const std::unordered_map<long, rtr::material*>& mats)
{
    long mat_id;
    float radius;
    glm::vec3 center;

    radius = elem->FirstChildElement("Radius")->FloatText();
    auto iss = std::istringstream(elem->FirstChildElement("Center")->GetText());
    iss >> center[0] >> center[1] >> center[2];

    mat_id = elem->FirstChildElement("Material")->Int64Text();

    auto mat_it = mats.find(mat_id);
    return rtr::shapes::sphere(center, radius, mat_it->second);
}

rtr::shapes::mesh
read_mesh(const xml::XMLElement* elem,
          const std::map<short, rtr::bvector<glm::vec3>>& vs,
          const std::map<short, rtr::bvector<int>>& indices,
          const std::unordered_map<long, rtr::material*>& mats)
{
    auto mat_id = elem->FirstChildElement("Material")->Int64Text();
    auto v_id = elem->FirstChildElement("VertexBuffer")->Int64Attribute("id");
    auto i_id = elem->FirstChildElement("IndexBuffer")->Int64Attribute("id");

    rtr::bvector<rtr::shapes::triangle> faces;

    auto& verts = vs.find(v_id)->second;
    auto& inds = indices.find(i_id)->second;
    for (std::size_t i = 0; i < inds.size(); i += 3) {
        faces.emplace_back(std::array<glm::vec3, 3>{verts[inds[i]], verts[inds[i + 1]], verts[inds[i + 2]]});
    }

    auto mat_it = mats.find(mat_id);
    rtr::shapes::mesh m(std::move(faces), inds, mat_it->second);
    if (elem->Attribute("shadingMode") && elem->Attribute("shadingMode") == std::string("smooth"))
    {
        m.smooth_normals();
    }
    return m;
}

void read_objects(
        const xml::XMLElement* elem,
        const std::map<short, rtr::bvector<glm::vec3>>& verts,
        const std::map<short, rtr::bvector<int>>& indices,
        const std::unordered_map<long, rtr::material*>& mats,
        rtr::scene& sc
)
{
    glm::vec3 scene_min, scene_max;
    float max_radius = 0;

    for (auto s = elem->FirstChildElement(); s; s = s->NextSiblingElement()) {
        if (s->Name()==std::string("Mesh") || s->Name()==std::string("Triangle")) {
            sc.insert(read_mesh(s, verts, indices, mats));
        }
        else if (s->Name()==std::string("Sphere")) {
            auto&& sphere = read_sphere(s, mats);
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
        if (elem->Attribute("shader") == nullptr || elem->Attribute("shader") == std::string("ceng795"))
        {
            return new rtr::rt_mat(read_rt_material(elem));
        }
        else if (elem->Attribute("shader") == std::string("normal_mat"))
        {
            auto m = new rtr::normal_mat;
            m->id = elem->Int64Attribute("id");
            return m;
        }
    }

    rtr::bvector<glm::vec3> parse_vector_buffer(const xml::XMLElement* elem)
    {
        rtr::bvector <glm::vec3> vert_pos;
        auto vert_text = elem->GetText();
        auto iss = std::istringstream(vert_text);
        for (glm::vec3 v; iss >> v[0] >> v[1] >> v[2];) {
            vert_pos.push_back(v);
        }
        return vert_pos;
    }

    rtr::bvector<int> parse_index_buffer(const xml::XMLElement* elem)
    {
        rtr::bvector <int> vert_pos;
        auto vert_text = elem->GetText();
        auto iss = std::istringstream(vert_text);
        for (int v; iss >> v;) {
            vert_pos.push_back(v);
        }
        return vert_pos;
    }
}

namespace rtr {
    namespace xml {
        std::pair<rtr::scene, std::vector<rtr::camera>> read_scene(const std::string& xml_data)
        {
            namespace xml = tinyxml2;
            xml::XMLDocument doc;
            doc.Parse(xml_data.c_str(), xml_data.size());

            glm::vec3 bg;
            float ray_epsilon = 0.001, intersect_epsilon = 0.001;

            auto root = doc.FirstChildElement("Scene");

            std::istringstream iss {root->FirstChildElement("BackgroundColor")->GetText()};
            iss >> bg[0] >> bg[1] >> bg[2];

            ray_epsilon = root->FirstChildElement("ShadowRayEpsilon")->FloatText(0);
            intersect_epsilon = root->FirstChildElement("IntersectionTestEpsilon")->FloatText(0);

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

            std::map<short, bvector<glm::vec3>> v_buffers;
            std::map<short, bvector<int>> i_buffers;

            auto buffers = root->FirstChildElement("Buffers");
            for (auto b = buffers->FirstChildElement(); b; b = b->NextSiblingElement())
            {
                if (b->Name() == std::string("VertexBuffer"))
                {
                    v_buffers.emplace(b->Int64Attribute("id"), parse_vector_buffer(b));
                }
                else if (b->Name() == std::string("IndexBuffer"))
                {
                    i_buffers.emplace(b->Int64Attribute("id"), parse_index_buffer(b));
                }
            }

            glm::vec3 center, extent;
            iss = std::istringstream(root->Attribute("center"));
            iss >> center[0] >> center[1] >> center[2];
            iss = std::istringstream(root->Attribute("extent"));
            iss >> extent[0] >> extent[1] >> extent[2];
            rtr::scene s{center, extent, std::move(mats)};

            s.m_shadow_epsilon = ray_epsilon;
            s.m_test_epsilon = intersect_epsilon;
            s.m_background = bg;

            auto objs_root = root->FirstChildElement("Objects");
            auto lights = root->FirstChildElement("Lights");

            read_objects(objs_root, v_buffers, i_buffers, s.materials(), s);
            read_lights(lights, s);

            return std::make_pair(std::move(s), std::move(cams));
        }
    }
}

