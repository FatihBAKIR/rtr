//
// Created by Mehmet Fatih BAKIR on 05/03/2017.
//

#include <xml_parse.hpp>
#include <glm/vec3.hpp>
#include <sstream>
#include <camera.hpp>
#include <scene.hpp>
#include <iostream>
#include <vector>

#include <shapes/sphere.hpp>
#include <shapes/triangle.hpp>
#include <shapes/mesh.hpp>

#include <material.hpp>

namespace xml = tinyxml2;
rtr::camera read_camera(const xml::XMLElement* elem)
{
    auto get_text = [&](const char* name)
    {
        return elem->FirstChildElement(name)->GetText();
    };

    glm::vec3 pos, up, gaze;

    std::istringstream iss (get_text("Position"));
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

    rtr::im_plane plane {left, right, top, bottom, dist, width, height};
    return {pos, up, gaze, plane};
}

rtr::shapes::sphere read_sphere(const xml::XMLElement* elem, gsl::span<glm::vec3> verts, gsl::span<rtr::material> mats)
{
    long mat_id;
    long vert_id;
    float radius;

    radius = elem->FirstChildElement("Radius")->FloatText();
    vert_id = elem->FirstChildElement("Center")->Int64Text();
    mat_id = elem->FirstChildElement("Material")->Int64Text();

    return rtr::shapes::sphere(verts[vert_id], radius, &mats[mat_id]);
}

void read_objects(const xml::XMLElement* elem, gsl::span<glm::vec3> verts, rtr::scene& sc)
{
    using namespace rtr::shapes;

    glm::vec3 scene_min, scene_max;

    for (auto s = elem->FirstChildElement(); s; s = s->NextSiblingElement())
    {
        if (s->Name() == std::string("Mesh"))
        {

        }
        else if (s->Name() == std::string("Sphere"))
        {
            sc.insert(read_sphere(s, verts, nullptr));
        }
    }
}

void read_scene(const std::string& path)
{
    namespace xml = tinyxml2;

    xml::XMLDocument doc;
    doc.LoadFile(path.c_str());

    glm::vec3 bg;
    float ray_epsilon, intersect_epsilon;

    auto root = doc.FirstChildElement("Scene");
    std::cout << root->FirstChildElement("BackgroundColor")->GetText() << '\n';

    ray_epsilon = root->FirstChildElement("ShadowRayEpsilon")->FloatText(0);
    intersect_epsilon = root->FirstChildElement("IntersectionTestEpsilon")->FloatText(0);

    std::vector<rtr::camera> cams;
    auto cameras = root->FirstChildElement("Cameras");
    for (auto c = cameras->FirstChildElement(); c; c = c->NextSiblingElement())
    {
        cams.push_back(read_camera(c));
    }

    std::vector<glm::vec3> vert_pos;
    auto vert_text = root->FirstChildElement("VertexData")->GetText();
    std::istringstream iss (vert_text);
    for (glm::vec3 v; iss >> v[0] >> v[1] >> v[2]; vert_pos.push_back(v));
}
