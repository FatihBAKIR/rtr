//
// Created by Mehmet Fatih BAKIR on 05/03/2017.
//

#include <lights.hpp>
#include <geometry.hpp>

#include <xml_parse.hpp>
#include <glm/vec3.hpp>
#include <sstream>
#include <camera.hpp>
#include <scene.hpp>
#include <iostream>
#include <vector>
#include <queue>
#include <map>

#include <materials/rt_mat.hpp>
#include <tinyxml2.h>
#include <unordered_map>
#include <materials/normal_mat.hpp>
#include <materials/material.hpp>
#include <materials/toon_shading.hpp>
#include <map>
#include <stack>
#include <materials/mirror_material.h>
#include <materials/glass.h>
#include <materials/metal.hpp>
#include <materials/bump.hpp>
#include <materials/skybox.hpp>

#include <boost/gil/extension/io/jpeg_io.hpp>
#include <boost/gil/extension/io/png_io.hpp>
#include <boost/gil/image_view.hpp>

#include <texturing/tex2d.hpp>
#include <texturing/perlin2d.hpp>
#include <brdf/brdf_common.hpp>
#include <brdf/phong_brdf.hpp>
#include <brdf/blinn_phong_brdf.hpp>
#include <brdf/torrance_sparrow.hpp>
#include <materials/brdf_mat.hpp>
#include <materials/illuminating.hpp>
#include <fstream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace xml = tinyxml2;
namespace {

    rtr::camera read_camera(const xml::XMLElement *elem) {
        auto get_text = [&](const char *name) {
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
        rtr::camera cam{pos, up, gaze, plane, elem->FirstChildElement("ImageName")->GetText()};

        if (elem->Attribute("type") == std::string("aperture"))
        {
            float focus_dist, aperture_size;
            iss = std::istringstream(get_text("FocusDistance"));
            iss >> focus_dist;
            iss = std::istringstream(get_text("ApertureSize"));
            iss >> aperture_size;
            cam.set_aperture(focus_dist, aperture_size);
        }

        int sample_num;
        iss = std::istringstream(get_text("NumSamples"));
        iss >> sample_num;

        iss = std::istringstream(get_text("Flags"));
        std::string flag;
        while (iss >> flag)
        {
            cam.add_flag(flag);
        }

        cam.set_samples(sample_num);

        return cam;
    }

    glm::mat4 parse_transform(const xml::XMLElement *elem, const std::map<std::string, glm::mat4> &ts) {
        std::queue<glm::mat4> transs;
        glm::mat4 full_trans(1.f);

        if (elem->GetText()) {
            std::istringstream iss(elem->GetText());
            for (std::string s; iss >> s;) {
                transs.push(ts.find(s)->second);
            }
        }

        while (!transs.empty()) {
            full_trans = transs.front() * full_trans;
            transs.pop();
        }

        return full_trans;
    }


    rtr::geometry::sphere
    read_sphere(
            const xml::XMLElement *elem,
            const std::unordered_map<long, rtr::material *> &mats,
            const std::map<std::string, glm::mat4> &transformations) {
        long mat_id;
        float radius;
        glm::vec3 center;

        radius = elem->FirstChildElement("Radius")->FloatText();
        auto iss = std::istringstream(elem->FirstChildElement("Center")->GetText());
        iss >> center[0] >> center[1] >> center[2];

        mat_id = elem->FirstChildElement("Material")->Int64Text();

        glm::mat4 full_trans = parse_transform(elem->FirstChildElement("Transformations"), transformations);

        auto mat_it = mats.find(mat_id);
        return rtr::geometry::sphere(center, radius, mat_it->second, full_trans);
    }


    rtr::geometry::mesh
    read_mesh(const xml::XMLElement *elem,
              const std::map<short, rtr::bvector<glm::vec3>> &vs,
              const std::map<short, rtr::bvector<glm::vec2>> &uvs,
              const std::map<short, rtr::bvector<int>> &indices,
              const std::map<std::string, glm::mat4> &transformations,
              const std::unordered_map<long, rtr::material *> &mats) {
        auto mat_id = elem->FirstChildElement("Material")->Int64Text();
        auto v_id = elem->FirstChildElement("VertexBuffer")->Int64Attribute("id");
        auto uv_id_elem = elem->FirstChildElement("UvBuffer");

        int uv_id = -1;
        if (uv_id_elem)
        {
            uv_id = uv_id_elem->Int64Attribute("id");
            // no uvs
        }

        auto i_id = elem->FirstChildElement("IndexBuffer")->Int64Attribute("id");

        rtr::bvector<rtr::geometry::triangle> faces;

        glm::mat4 full_trans = parse_transform(elem->FirstChildElement("Transformations"), transformations);

        auto &verts = vs.find(v_id)->second;
        auto &inds = indices.find(i_id)->second;
        for (std::size_t i = 0; i < inds.size(); i += 3) {
            faces.emplace_back(std::array<glm::vec3, 3>{
                    glm::vec3(full_trans * glm::vec4(verts[inds[i]], 1)),
                    glm::vec3(full_trans * glm::vec4(verts[inds[i + 1]], 1)),
                    glm::vec3(full_trans * glm::vec4(verts[inds[i + 2]], 1)),
            });
        }

        auto mat_it = mats.find(mat_id);
        rtr::geometry::mesh m(std::move(faces), inds, (uv_id >= 0) ? uvs.find(uv_id)->second : rtr::bvector<glm::vec2>(), mat_it->second);
        if (elem->Attribute("shadingMode") && elem->Attribute("shadingMode") == std::string("smooth")) {
            m.smooth_normals();
        }
        return m;
    }

    void read_objects(
            const xml::XMLElement *elem,
            const std::map<short, rtr::bvector<glm::vec3>> &verts,
            const std::map<short, rtr::bvector<glm::vec2>> &uvs,
            const std::map<short, rtr::bvector<int>> &indices,
            const std::map<std::string, glm::mat4> &transformations,
            const std::unordered_map<long, rtr::material *> &mats,
            rtr::scene &sc
    ) {
        glm::vec3 scene_min, scene_max;
        float max_radius = 0;

        for (auto s = elem->FirstChildElement(); s; s = s->NextSiblingElement()) {
            auto id = s->IntAttribute("id");
            if (s->Name() == std::string("Mesh") || s->Name() == std::string("Triangle")) {
                auto&& res = read_mesh(s, verts, uvs, indices, transformations, mats);
                auto is_local = s->Attribute("local_scene") == std::string("True");
                res.set_id(id);
                if (is_local)
                {
                    res.set_local_scene();
                }
                sc.insert(std::move(res));
            } else if (s->Name() == std::string("Sphere")) {
                auto &&sphere = read_sphere(s, mats, transformations);
                sphere.set_id(id);
                if (sphere.get_radius() > max_radius) {
                    auto growth = sphere.get_radius() - max_radius;
                    max_radius = sphere.get_radius();
                    sc.resize(sc.get_box().position, sc.get_box().extent + glm::vec3(growth, growth, growth) * 2.f);
                }
                sc.insert(sphere);
            } else {
                abort();
            }
        }
    }

    rtr::texturing::sampler2d* read_hdr(const xml::XMLElement* elem)
    {
        static_assert(RTR_OPENCV_SUPPORT, "need opencv for hdr reading");
        auto mat = cv::imread(elem->FirstChildElement("Path")->GetText(), cv::IMREAD_COLOR | cv::IMREAD_ANYDEPTH);

        cv::cvtColor(mat, mat, CV_BGR2RGB);

        {
            cv::GaussianBlur(mat, mat, cv::Size(3, 3), 0, 0);
        }

        float scaling = elem->FirstChildElement("Scaling")->FloatText(255);
        rtr::texturing::sampling_mode m = rtr::texturing::sampling_mode::nearest_neighbour;

        auto app_elem = elem->FirstChildElement("Sampling")->GetText();
        if (app_elem == std::string("0")) m = rtr::texturing::sampling_mode::nearest_neighbour;
        else if (app_elem == std::string("1")) m = rtr::texturing::sampling_mode::bilinear;

        return new rtr::texturing::tex2d<float, 3>((const float*)(mat.data), mat.cols, mat.rows, scaling, m);
    }

    rtr::texturing::sampler2d* read_image(const xml::XMLElement* elem)
    {
        if (elem->Attribute("is_hdr") == std::string("True"))
        {
            return read_hdr(elem);
        }

        boost::gil::rgb8_image_t im;
        auto p = elem->FirstChildElement("Path")->GetText();
        float scaling = elem->FirstChildElement("Scaling")->FloatText(255);
        rtr::texturing::sampling_mode m = rtr::texturing::sampling_mode::nearest_neighbour;

        auto app_elem = elem->FirstChildElement("Sampling")->GetText();
        if (app_elem == std::string("0")) m = rtr::texturing::sampling_mode::nearest_neighbour;
        else if (app_elem == std::string("1")) m = rtr::texturing::sampling_mode::bilinear;

        boost::gil::jpeg_read_image(p, im);
        auto view = boost::gil::view (im);

        assert(view.is_1d_traversable());

        return new rtr::texturing::tex2d<uint8_t, 3>((const uint8_t*)(&view[0][0]), view.width(), view.height(), scaling, m);
    }

    rtr::texturing::sampler2d* read_perlin(const xml::XMLElement* elem)
    {
        rtr::texturing::perlin_appearance appearance = rtr::texturing::perlin_appearance::vein;
        float scaling;

        auto app_elem = elem->FirstChildElement("Appearance")->GetText();
        if (app_elem == std::string("0")) appearance = rtr::texturing::perlin_appearance::vein;
        else if (app_elem == std::string("1")) appearance = rtr::texturing::perlin_appearance::patch;

        scaling = elem->FirstChildElement("ScalingFactor")->FloatText();

        return new rtr::texturing::perlin2d(appearance, scaling);
    }

    rtr::lights::ambient_light read_ambient(const xml::XMLElement *elem) {
        std::istringstream iss(elem->GetText());
        glm::vec3 color;
        iss >> color[0] >> color[1] >> color[2];
        return {color};
    }

    rtr::lights::point_light read_point(const xml::XMLElement *elem) {
        glm::vec3 pos, inte;

        std::istringstream iss(elem->FirstChildElement("Position")->GetText());
        iss >> pos[0] >> pos[1] >> pos[2];

        iss = std::istringstream(elem->FirstChildElement("Intensity")->GetText());
        iss >> inte[0] >> inte[1] >> inte[2];

        return {pos, inte};
    }

    rtr::lights::area_light read_area(const xml::XMLElement *elem) {
        glm::vec3 pos, inte, e1, e2;
        int samples = 6;

        std::istringstream iss(elem->FirstChildElement("Position")->GetText());
        iss >> pos[0] >> pos[1] >> pos[2];

        iss = std::istringstream(elem->FirstChildElement("Intensity")->GetText());
        iss >> inte[0] >> inte[1] >> inte[2];

        iss = std::istringstream(elem->FirstChildElement("EdgeVector1")->GetText());
        iss >> e1[0] >> e1[1] >> e1[2];

        iss = std::istringstream(elem->FirstChildElement("EdgeVector2")->GetText());
        iss >> e2[0] >> e2[1] >> e2[2];

        return {pos, inte, e1, e2, samples};
    }

    rtr::lights::directional_light read_directional(const xml::XMLElement *elem) {
        glm::vec3 dir, radi;

        std::istringstream iss(elem->FirstChildElement("Direction")->GetText());
        iss >> dir[0] >> dir[1] >> dir[2];

        iss = std::istringstream(elem->FirstChildElement("Radiance")->GetText());
        iss >> radi[0] >> radi[1] >> radi[2];

        return {dir, radi};
    }

    rtr::lights::spot_light read_spot(const xml::XMLElement* elem)
    {
        glm::vec3 pos, inte, dir;
        float perfect, fall_off;

        std::istringstream iss(elem->FirstChildElement("Position")->GetText());
        iss >> pos[0] >> pos[1] >> pos[2];

        iss = std::istringstream(elem->FirstChildElement("Intensity")->GetText());
        iss >> inte[0] >> inte[1] >> inte[2];

        iss = std::istringstream(elem->FirstChildElement("Direction")->GetText());
        iss >> dir[0] >> dir[1] >> dir[2];

        fall_off = elem->FirstChildElement("FalloffAngle")->FloatText();
        perfect = elem->FirstChildElement("CoverageAngle")->FloatText();

        return {pos, inte, glm::normalize(dir), perfect, fall_off};
    }

    void read_lights(const xml::XMLElement *elem, rtr::scene &sc) {
        for (auto l = elem->FirstChildElement(); l; l = l->NextSiblingElement()) {
            if (l->Name() == std::string("AmbientLight")) {
                sc.insert(read_ambient(l));
            } else if (l->Name() == std::string("PointLight")) {
                sc.insert(read_point(l));
            } else if (l->Name() == std::string("SpotLight")) {
                sc.insert(read_spot(l));
            } else if (l->Name() == std::string("AreaLight")) {
                sc.insert(read_area(l));
            } else if (l->Name() == std::string("DirectionalLight")) {
                sc.insert(read_directional(l));
            } else {
                abort();
            }
        }
    }

    rtr::rt_mat read_rt_material(const xml::XMLElement *elem, const std::map<uint16_t, rtr::texturing::sampler2d*> texs) {
        auto get_text = [&](const char *name) {
            return elem->FirstChildElement(name)->GetText();
        };

        glm::vec3 diffuse, specular, ambient;
        std::istringstream iss(get_text("AmbientReflectance"));
        iss >> ambient[0] >> ambient[1] >> ambient[2];

        iss = std::istringstream(get_text("SpecularReflectance"));
        iss >> specular[0] >> specular[1] >> specular[2];

        float phong = 0;
        if (elem->FirstChildElement("PhongExponent"))
        {
            phong = elem->FirstChildElement("PhongExponent")->FloatText(0);
        }

        auto dif_elem = elem->FirstChildElement("DiffuseReflectance");
        int id;
        if ((id = dif_elem->IntAttribute("tex_id", -1)) != -1)
        {
            rtr::rt_mat::decal_mode m = rtr::rt_mat::decal_mode::replace;

            auto mo = dif_elem->Attribute("tex_mode");
            if (mo == std::string("0")) m = rtr::rt_mat::decal_mode::blend;
            else if (mo == std::string("1")) m = rtr::rt_mat::decal_mode::coeff;
            else if (mo == std::string("2")) m = rtr::rt_mat::decal_mode::replace;

            if (m == rtr::rt_mat::decal_mode::blend)
            {
                iss = std::istringstream(get_text("DiffuseReflectance"));
                iss >> diffuse[0] >> diffuse[1] >> diffuse[2];
                return {texs.find(id)->second, diffuse, specular, ambient, phong};
            }
            return {texs.find(id)->second, m, specular, ambient, phong};
        }
        else
        {
            iss = std::istringstream(get_text("DiffuseReflectance"));
            iss >> diffuse[0] >> diffuse[1] >> diffuse[2];
            return {diffuse, specular, ambient, phong};
        }
    }

    rtr::shading::mirror_material read_mirror_mat(const xml::XMLElement* elem, const std::map<uint16_t, rtr::texturing::sampler2d*> texs)
    {
        auto get_text = [&](const char *name) {
            return elem->FirstChildElement(name)->GetText();
        };

        auto base_mat = read_rt_material(elem, texs);

        glm::vec3 ref;
        auto iss = std::istringstream(get_text("MirrorReflectance"));
        iss >> ref[0] >> ref[1] >> ref[2];

        return { base_mat, ref };
    }

    rtr::shading::bump read_bump_mat(const xml::XMLElement* elem, const std::map<uint16_t, rtr::texturing::sampler2d*> texs)
    {
        auto get_text = [&](const char *name) {
            return elem->FirstChildElement(name)->GetText();
        };

        auto base_mat = read_rt_material(elem, texs);

        return { new rtr::rt_mat(base_mat), base_mat.get_sampler() };
    }

    rtr::shading::metal read_metal_mat(const xml::XMLElement* elem, const std::map<uint16_t, rtr::texturing::sampler2d*> texs)
    {
        auto get_text = [&](const char *name) {
            return elem->FirstChildElement(name)->GetText();
        };

        auto base_mat = read_rt_material(elem, texs);

        glm::vec3 ref;
        auto iss = std::istringstream(get_text("MirrorReflectance"));
        iss >> ref[0] >> ref[1] >> ref[2];

        float r = elem->FirstChildElement("Roughness")->FloatText();

        return { base_mat, ref, r };
    }

    rtr::shading::glass read_glass_mat(const xml::XMLElement* elem)
    {
        float index;

        auto get_text = [&](const char *name) {
            return elem->FirstChildElement(name)->GetText();
        };

        glm::vec3 ref;
        auto iss = std::istringstream(get_text("Transparency"));
        iss >> ref[0] >> ref[1] >> ref[2];

        index = elem->FirstChildElement("RefractionIndex")->FloatText();

        return { ref, index };
    }

    rtr::shading::illuminating read_illum_data(const xml::XMLElement* elem)
    {
        float index;

        auto get_text = [&](const char *name) {
            return elem->FirstChildElement(name)->GetText();
        };

        glm::vec3 ref;
        auto iss = std::istringstream(get_text("Radiance"));
        iss >> ref[0] >> ref[1] >> ref[2];

        return { ref };
    }

    rtr::shading::skybox read_skybox_mat(const xml::XMLElement* elem, const std::map<uint16_t, rtr::texturing::sampler2d*> texs)
    {
        auto id = elem->FirstChildElement("TextureId")->IntText();
        auto scale = elem->FirstChildElement("Scaling")->FloatText();
        return { texs.find(id)->second, scale };
    }

    rtr::brdf::brdf_data parse_data(const xml::XMLElement* elem)
    {
        auto get_text = [&](const char *name) {
            return elem->FirstChildElement(name)->GetText();
        };

        glm::vec3 diffuse, specular, ambient;
        std::istringstream iss(get_text("AmbientReflectance"));
        iss >> ambient[0] >> ambient[1] >> ambient[2];

        iss = std::istringstream(get_text("DiffuseReflectance"));
        iss >> diffuse[0] >> diffuse[1] >> diffuse[2];

        iss = std::istringstream(get_text("SpecularReflectance"));
        iss >> specular[0] >> specular[1] >> specular[2];

        return {diffuse, specular, ambient};
    }

    namespace parser
    {
        namespace brdf
        {
            using namespace rtr::brdf;
            phong read_phong(const xml::XMLElement* elem)
            {
                auto phong = elem->FirstChildElement("Exponent")->FloatText(0);
                return {phong};
            }

            phong_modified read_phong_modified(const xml::XMLElement* elem)
            {
                auto phong = elem->FirstChildElement("Exponent")->FloatText(0);
                return {phong};
            }

            phong_modified_normalized read_phong_modified_norm(const xml::XMLElement* elem)
            {
                auto phong = elem->FirstChildElement("Exponent")->FloatText(0);
                return {phong};
            }

            blinn_phong read_blinn_phong(const xml::XMLElement* elem)
            {
                auto phong = elem->FirstChildElement("Exponent")->FloatText(0);
                return {phong};
            }

            blinn_phong_modified read_blinn_phong_modified(const xml::XMLElement* elem)
            {
                auto phong = elem->FirstChildElement("Exponent")->FloatText(0);
                return {phong};
            }

            blinn_phong_modified_normalized read_blinn_phong_modified_norm(const xml::XMLElement* elem)
            {
                auto phong = elem->FirstChildElement("Exponent")->FloatText(0);
                return {phong};
            }

            torrance_sparrow read_torrence(const xml::XMLElement* elem)
            {
                auto phong = elem->FirstChildElement("Exponent")->FloatText(0);
                return {phong};
            }
        }
    }

    rtr::material* read_brdf_mat(const xml::XMLElement* elem)
    {
        auto data = parse_data(elem);

        if (elem->Attribute("brdf") == std::string("OriginalPhong"))
        {
            return rtr::shading::make_brdf(data, parser::brdf::read_phong(elem));
        }

        if (elem->Attribute("brdf") == std::string("ModifiedPhong"))
        {
            return rtr::shading::make_brdf(data, parser::brdf::read_phong_modified(elem));
        }

        if (elem->Attribute("brdf") == std::string("ModifiedPhongNorm"))
        {
            return rtr::shading::make_brdf(data, parser::brdf::read_phong_modified_norm(elem));
        }

        if (elem->Attribute("brdf") == std::string("OriginalBlinnPhong"))
        {
            return rtr::shading::make_brdf(data, parser::brdf::read_blinn_phong(elem));
        }

        if (elem->Attribute("brdf") == std::string("ModifiedBlinnPhong"))
        {
            return rtr::shading::make_brdf(data, parser::brdf::read_blinn_phong_modified(elem));
        }

        if (elem->Attribute("brdf") == std::string("ModifiedBlinnPhongNorm"))
        {
            return rtr::shading::make_brdf(data, parser::brdf::read_blinn_phong_modified_norm(elem));
        }

        if (elem->Attribute("brdf") == std::string("TorranceSparrow"))
        {
            return rtr::shading::make_brdf(data, parser::brdf::read_torrence(elem));
        }

        throw std::runtime_error("brdf not supported");
    }

    rtr::material *read_material(const xml::XMLElement *elem,
            const std::unordered_map<long, rtr::material *> &mats,
            const std::map<uint16_t, rtr::texturing::sampler2d*> samplers) {
        if (elem->Attribute("shader") == nullptr || elem->Attribute("shader") == std::string("ceng795")) {
            auto m = new rtr::rt_mat(read_rt_material(elem, samplers));
            m->id = elem->Int64Attribute("id");
            return m;
        } else if (elem->Attribute("shader") == std::string("normal_mat")) {
            auto m = new rtr::normal_mat;
            m->id = elem->Int64Attribute("id");
            return m;
        } else if (elem->Attribute("shader") == std::string("toon_shader")) {
            auto m = new rtr::shading::toon_shader;
            m->id = elem->Int64Attribute("id");
            return m;
        } else if (elem->Attribute("shader") == std::string("mirror"))
        {
            auto ret = new rtr::shading::mirror_material(read_mirror_mat(elem, samplers));
            ret->id = elem->Int64Attribute("id");
            return ret;
        } else if (elem->Attribute("shader") == std::string("glass"))
        {
            auto ret = new rtr::shading::glass(read_glass_mat(elem));
            ret->id = elem->Int64Attribute("id");
            return ret;
        } else if (elem->Attribute("shader") == std::string("metal"))
        {
            auto ret = new rtr::shading::metal(read_metal_mat(elem, samplers));
            ret->id = elem->Int64Attribute("id");
            return ret;
        } else if (elem->Attribute("shader") == std::string("bump"))
        {
            auto ret = new rtr::shading::bump(read_bump_mat(elem, samplers));
            ret->id = elem->Int64Attribute("id");
            return ret;
        } else if (elem->Attribute("shader") == std::string("brdf"))
        {
            auto ret = read_brdf_mat(elem);
            ret->id = elem->Int64Attribute("id");
            return ret;
        } else if (elem->Attribute("shader") == std::string("illuminating"))
        {
            auto ret = new rtr::shading::illuminating(read_illum_data(elem));
            ret->id = elem->Int64Attribute("id");
            return ret;
        } else if (elem->Attribute("shader") == std::string("skybox"))
        {
            auto ret = new rtr::shading::skybox(read_skybox_mat(elem, samplers));
            ret->id = elem->Int64Attribute("id");
            return ret;
        }
        throw std::runtime_error("shader not supported: " + std::string(elem->Attribute("shader")));
    }

    rtr::bvector<glm::vec3> parse_vector_buffer(const xml::XMLElement *elem) {
        rtr::bvector<glm::vec3> vert_pos;
        auto bin_file = elem->Attribute("binaryFile");
        if (bin_file)
        {
            std::ifstream inf(bin_file, std::ios::binary);

            assert(inf.good());

            uint32_t len;
            inf.read(reinterpret_cast<char*>(&len), sizeof(len));

            std::cout << len << '\n';

            for (int i = 0; i < len; ++i)
            {
                glm::vec3 v;
                inf.read(reinterpret_cast<char*>(&v[0]), sizeof(v[0]));
                inf.read(reinterpret_cast<char*>(&v[1]), sizeof(v[1]));
                inf.read(reinterpret_cast<char*>(&v[2]), sizeof(v[2]));
                vert_pos.push_back(v);
            }

            return vert_pos;
        }

        auto vert_text = elem->GetText();
        auto iss = std::istringstream(vert_text);
        for (glm::vec3 v; iss >> v[0] >> v[1] >> v[2];) {
            vert_pos.push_back(v);
        }
        return vert_pos;
    }

    rtr::bvector<glm::vec2> parse_uvector_buffer(const xml::XMLElement *elem) {
        rtr::bvector<glm::vec2> vert_pos;

        auto bin_file = elem->Attribute("binaryFile");
        if (bin_file)
        {
            std::ifstream inf(bin_file, std::ios::binary);

            assert(inf.good());

            uint32_t len;
            inf.read(reinterpret_cast<char*>(&len), sizeof(len));

            std::cout << len << '\n';

            for (int i = 0; i < len; ++i)
            {
                glm::vec2 v;
                inf.read(reinterpret_cast<char*>(&v[0]), sizeof(v[0]));
                inf.read(reinterpret_cast<char*>(&v[1]), sizeof(v[1]));
                vert_pos.push_back(v);
            }

            return vert_pos;
        }

        auto vert_text = elem->GetText();
        auto iss = std::istringstream(vert_text);
        for (glm::vec2 v; iss >> v[0] >> v[1];) {
            vert_pos.push_back(v);
        }
        return vert_pos;
    }

    rtr::bvector<int> parse_index_buffer(const xml::XMLElement *elem) {
        rtr::bvector<int> vert_pos;

        auto bin_file = elem->Attribute("binaryFile");
        if (bin_file)
        {
            std::ifstream inf(bin_file, std::ios::binary);

            assert(inf.good());

            uint32_t len;
            inf.read(reinterpret_cast<char*>(&len), sizeof(len));

            std::cout << len << '\n';

            for (int i = 0; i < len; ++i)
            {
                int v[3];
                inf.read(reinterpret_cast<char*>(&v[0]), sizeof(v[0]));
                inf.read(reinterpret_cast<char*>(&v[1]), sizeof(v[1]));
                inf.read(reinterpret_cast<char*>(&v[2]), sizeof(v[2]));
                vert_pos.insert(vert_pos.end(), std::begin(v), std::end(v));
            }

            return vert_pos;
        }

        auto vert_text = elem->GetText();
        auto iss = std::istringstream(vert_text);
        for (int v; iss >> v;) {
            vert_pos.push_back(v);
        }
        return vert_pos;
    }

    auto read_transformation(const xml::XMLElement *elem) {
        std::istringstream iss(elem->GetText());

        if (elem->Name() == std::string("Translation")) {
            glm::vec3 t;
            iss >> t[0] >> t[1] >> t[2];
            return std::make_pair(std::string("t") + elem->Attribute("id"), rtr::transform::translate(t));
        } else if (elem->Name() == std::string("Scaling")) {
            glm::vec3 t;
            iss >> t[0] >> t[1] >> t[2];
            return std::make_pair(std::string("s") + elem->Attribute("id"), rtr::transform::scale(t));
        } else if (elem->Name() == std::string("Rotation")) {
            float amount;
            glm::vec3 t;
            iss >> amount >> t[0] >> t[1] >> t[2];
            return std::make_pair(std::string("r") + elem->Attribute("id"), rtr::transform::rotate(amount, t));
        }

        throw std::runtime_error("can't happen");
    }
}

struct parser {
    glm::vec3 bg;
    float ray_epsilon, intersect_epsilon;

    std::vector<rtr::camera> cams;
    std::unordered_map<long, rtr::material *> mats;
    std::unordered_map<std::string, glm::mat4> transformations;

    std::map<short, rtr::bvector<glm::vec3>> v_buffers;
    std::map<short, rtr::bvector<int>> i_buffers;

    glm::vec3 center, extent;
};

namespace rtr {
    namespace xml {
        std::pair<rtr::scene, std::vector<rtr::camera>> read_scene(const std::string &xml_data) {
            namespace xml = tinyxml2;
            xml::XMLDocument doc;
            doc.Parse(xml_data.c_str(), xml_data.size());

            glm::vec3 bg;
            float ray_epsilon = 0.001, intersect_epsilon = 0.001;
            std::uint8_t rtl;

            auto root = doc.FirstChildElement("Scene");

            std::istringstream iss{root->FirstChildElement("BackgroundColor")->GetText()};
            iss >> bg[0] >> bg[1] >> bg[2];

            ray_epsilon = root->FirstChildElement("ShadowRayEpsilon")->FloatText(0);
            rtl = root->FirstChildElement("MaxRecursionDepth")->Int64Text(1);
            intersect_epsilon = root->FirstChildElement("IntersectionTestEpsilon")->FloatText(0);

            std::vector<rtr::camera> cams;
            auto cameras = root->FirstChildElement("Cameras");
            for (auto c = cameras->FirstChildElement(); c; c = c->NextSiblingElement()) {
                cams.push_back(read_camera(c));
            }

            std::map<std::string, glm::mat4> transformations;
            auto transs = root->FirstChildElement("Transformations");
            for (auto t = transs->FirstChildElement(); t; t = t->NextSiblingElement()) {
                transformations.insert(read_transformation(t));
            }

            std::map<short, bvector<glm::vec3>> v_buffers;
            std::map<short, bvector<glm::vec2>> uv_buffers;
            std::map<short, bvector<int>> i_buffers;

            auto buffers = root->FirstChildElement("Buffers");
            for (auto b = buffers->FirstChildElement(); b; b = b->NextSiblingElement()) {
                if (b->Name() == std::string("VertexBuffer")) {
                    v_buffers.emplace(b->Int64Attribute("id"), parse_vector_buffer(b));
                } else if (b->Name() == std::string("UVertexBuffer")) {
                    uv_buffers.emplace(b->Int64Attribute("id"), parse_uvector_buffer(b));
                } else if (b->Name() == std::string("IndexBuffer")) {
                    i_buffers.emplace(b->Int64Attribute("id"), parse_index_buffer(b));
                }
            }

            std::map<uint16_t, rtr::texturing::sampler2d*> textures;
            auto textures_elem = root->FirstChildElement("Textures");
            for (auto t = textures_elem->FirstChildElement(); t; t = t->NextSiblingElement())
            {
                if (t->Name() == std::string("Image"))
                {
                    textures.emplace(t->Int64Attribute("id"), read_image(t));
                }
                else if (t->Name() == std::string("Perlin"))
                {
                    textures.emplace(t->Int64Attribute("id"), read_perlin(t));
                }
            }

            std::unordered_map<long, rtr::material *> mats;
            auto materials = root->FirstChildElement("Materials");
            for (auto c = materials->FirstChildElement(); c; c = c->NextSiblingElement()) {
                auto m = read_material(c, mats, textures);
                mats.emplace(m->id, m);
            }
            glm::vec3 center, extent;
            iss = std::istringstream(root->Attribute("center"));
            iss >> center[0] >> center[1] >> center[2];
            iss = std::istringstream(root->Attribute("extent"));
            iss >> extent[0] >> extent[1] >> extent[2];
            rtr::scene s{center, extent * 2.0f, std::move(mats)};

            s.m_shadow_epsilon = ray_epsilon;
            s.m_test_epsilon = intersect_epsilon;
            s.m_background = bg;
            s.set_rtl(rtl);

            auto objs_root = root->FirstChildElement("Objects");
            auto lights = root->FirstChildElement("Lights");

            read_objects(objs_root, v_buffers, uv_buffers, i_buffers, transformations, s.materials(), s);
            read_lights(lights, s);

            s.set_samplers(std::move(textures));

            return std::make_pair(std::move(s), std::move(cams));
        }
    }
}

