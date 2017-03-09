//
// Created by Mehmet Fatih BAKIR on 05/03/2017.
//

#include <shapes/sphere.hpp>
#include <shapes/triangle.hpp>
#include <shapes/mesh.hpp>
#include <lights/ambient_light.hpp>

#include <xml_parse.hpp>
#include <glm/vec3.hpp>
#include <sstream>
#include <camera.hpp>
#include <scene.hpp>
#include <iostream>
#include <vector>

#include <material.hpp>
#include <3rd_party/tinyxml2/tinyxml2.h>
#include <unordered_map>

namespace xml = tinyxml2;

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

    rtr::im_plane plane{left, right, top, bottom, dist, width, height};
    return {pos, up, gaze, plane};
}

rtr::shapes::sphere read_sphere(const xml::XMLElement* elem, gsl::span<glm::vec3> verts, const std::unordered_map<long, rtr::material>& mats)
{
    long mat_id;
    long vert_id;
    float radius;

    radius = elem->FirstChildElement("Radius")->FloatText();
    vert_id = elem->FirstChildElement("Center")->Int64Text();
    mat_id = elem->FirstChildElement("Material")->Int64Text();

    auto mat_it = mats.find(mat_id);
    return rtr::shapes::sphere(verts[vert_id], radius, &(*mat_it).second);
}

rtr::shapes::mesh read_mesh(const xml::XMLElement* elem, gsl::span<glm::vec3> verts, const std::unordered_map<long, rtr::material>& mats)
{
    long mat_id;

    mat_id = elem->FirstChildElement("Material")->Int64Text();
    boost::container::vector<rtr::shapes::triangle> faces;

    auto vert_text = elem->FirstChildElement("Faces")->GetText();
    std::istringstream iss(vert_text);
    for (std::array<long, 3> v; iss >> v[0] >> v[1] >> v[2];) {
        faces.emplace_back(std::array<glm::vec3, 3>{verts[v[0]], verts[v[1]], verts[v[2]]});
    }

    auto mat_it = mats.find(mat_id);
    return { std::move(faces), &(*mat_it).second };
}

void read_objects(const xml::XMLElement* elem, gsl::span<glm::vec3> verts, const std::unordered_map<long, rtr::material>& mats, rtr::scene& sc)
{
    using namespace rtr::shapes;

    glm::vec3 scene_min, scene_max;

    for (auto s = elem->FirstChildElement(); s; s = s->NextSiblingElement()) {
        if (s->Name()==std::string("Mesh") || s->Name() == std::string("Triangle")) {
            sc.insert(read_mesh(s, verts, mats));
        }
        else if (s->Name()==std::string("Sphere")) {
            sc.insert(read_sphere(s, verts, mats));
        }
    }
}

rtr::material read_material(const xml::XMLElement* elem)
{
    auto get_text = [&](const char* name) {
        return elem->FirstChildElement(name)->GetText();
    };

    rtr::material m;

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

std::pair<rtr::scene, std::vector<rtr::camera>> read_scene(const std::string& path)
{
    namespace xml = tinyxml2;

    xml::XMLDocument doc;
    doc.LoadFile(path.c_str());

    glm::vec3 bg;
    float ray_epsilon, intersect_epsilon;

    auto root = doc.FirstChildElement("Scene");
    std::cout << root->FirstChildElement("BackgroundColor")->GetText() << '\n';

    if (root->FirstChildElement("ShadowRayEpsilon"))
    {
        ray_epsilon = root->FirstChildElement("ShadowRayEpsilon")->FloatText(0);
    }
    if (root->FirstChildElement("IntersectionTestEpsilon"))
    {
        intersect_epsilon = root->FirstChildElement("IntersectionTestEpsilon")->FloatText(0);
    }

    std::vector<rtr::camera> cams;
    auto cameras = root->FirstChildElement("Cameras");
    for (auto c = cameras->FirstChildElement(); c; c = c->NextSiblingElement()) {
        cams.push_back(read_camera(c));
    }

    std::unordered_map<long, rtr::material> mats;
    auto materials = root->FirstChildElement("Materials");
    for (auto c = materials->FirstChildElement(); c; c = c->NextSiblingElement()) {
        auto&& m = read_material(c);
        mats.emplace(m.id, std::move(m));
    }

    glm::vec3 min = { 10000, 10000, 10000 }, max = { -10000, -10000, -10000 };

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

    std::vector<glm::vec3> vert_pos (1);
    auto vert_text = root->FirstChildElement("VertexData")->GetText();
    std::istringstream iss(vert_text);
    for (glm::vec3 v; iss >> v[0] >> v[1] >> v[2];) {
        vert_pos.push_back(v);
        up_min_max(v);
    }

    glm::vec3 center = (min + max) * 0.5f;
    glm::vec3 ext = max - min;

    rtr::scene s {center, ext, std::move(mats)};

    auto objs_root = root->FirstChildElement("Objects");

    read_objects(objs_root, vert_pos, s.materials(), s);

    return std::make_pair(std::move(s), std::move(cams));
}
