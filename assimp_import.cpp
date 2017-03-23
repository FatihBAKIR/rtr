//
// Created by fatih on 23.03.2017.
//

#include <assimp/Importer.hpp>
#include <assimp_import.hpp>
#include <postprocess.h>
#include <stdexcept>
#include <scene.h>
#include <glm/vec3.hpp>
#include <geometry.hpp>
#include <materials/normal_mat.hpp>
#include <lights.hpp>
#include <camera.hpp>
#include <iostream>


namespace {
    rtr::geometry::mesh
    read_mesh(const struct aiMesh *mesh) {
        rtr::bvector<glm::vec3> pos;
        rtr::bvector<rtr::geometry::triangle> faces;
        rtr::bvector<int> inds;

        for (unsigned int j = 0; j < mesh->mNumVertices; ++j) {
            glm::vec3 tempVertex;
            tempVertex.x = mesh->mVertices[j].x;
            tempVertex.y = mesh->mVertices[j].y;
            tempVertex.z = mesh->mVertices[j].z;
            pos.push_back(tempVertex);
        }

        for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
            aiFace face = mesh->mFaces[j];
            Expects(face.mNumIndices == 3);

            rtr::geometry::triangle t({pos[face.mIndices[0]], pos[face.mIndices[1]], pos[face.mIndices[2]]});
            faces.push_back(t);

            for (unsigned int k = 0; k < face.mNumIndices; ++k) { inds.push_back(face.mIndices[k]); }
        }

        rtr::geometry::mesh m(std::move(faces), std::move(inds), (const rtr::material *) new rtr::normal_mat);
        m.smooth_normals();
        return m;
    }

    rtr::lights::ambient_light read_ambient(const struct aiLight *light) {
        auto a = light->mColorAmbient;
        return glm::vec3{a.r, a.g, a.b};
    }

    rtr::lights::point_light read_point(const struct aiLight *light) {
        return {{light->mPosition.x,     light->mPosition.y,     light->mPosition.z},
                {light->mColorDiffuse.r, light->mColorDiffuse.g, light->mColorDiffuse.b}};
    }

    void read_light(const struct aiLight *light, rtr::scene &s) {
        switch (light->mType) {
            case aiLightSourceType::aiLightSource_AMBIENT:
                s.insert(read_ambient(light));
                break;
            case aiLightSourceType::aiLightSource_POINT:
                s.insert(read_point(light));
                break;
            default:
                std::cerr << "unsupported light\n";
        }
    }

    rtr::camera read_camera(const struct aiCamera *cam) {
    }
}

namespace rtr {
    namespace assimp {
        std::pair<rtr::scene, std::vector<rtr::camera>> read_scene(const std::string &path) {
            Assimp::Importer importer;
            auto scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_PreTransformVertices);
            if ((!scene) || (scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE) || (!scene->mRootNode)) {
                throw std::runtime_error("Error loading mymodel.obj: " + std::string(importer.GetErrorString()));
            }

            std::vector<rtr::geometry::mesh> meshes;
            for (std::size_t i = 0; i < scene->mNumMeshes; ++i) {
                auto mesh = scene->mMeshes[i];
                meshes.push_back(read_mesh(mesh));
            }

            auto box = physics::merge(meshes.begin(), meshes.end());

            rtr::scene s(box.position, box.extent, {});
            for (auto &m : meshes) {
                s.insert(std::move(m));
            }

            for (std::size_t i = 0; i < scene->mNumLights; ++i) {
                auto light = scene->mLights[i];
                read_light(light, s);
            }

            s.insert(rtr::lights::ambient_light(glm::vec3{60, 60, 60}));

            rtr::camera cam({350, 50, 0}, {0, 1, 0}, {-1, 0, -0.16}, {-5, 5, 5, -5, 10, 1024, 1024}, "assimp.png");

            return std::make_pair(std::move(s), std::vector<rtr::camera>{std::move(cam)});
        }
    }
}

