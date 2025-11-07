#include "io/JsonSceneLoader.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

#include <nlohmann/json.hpp>
#include "core/Camera.hpp"
#include "core/Scene.hpp"
#include "entities/Plane.hpp"
#include "entities/Sphere.hpp"
#include "entities/Triangle.hpp"
#include "io/Logger.hpp"
#include "materials/Checker.hpp"
#include "materials/Dielectric.hpp"
#include "materials/Lambertian.hpp"
#include "materials/Material.hpp"
#include "materials/Metal.hpp"
#include "math/Color.hpp"
#include "math/Vec3.hpp"

using json = nlohmann::json;

namespace {

// --- small utilities ---------------------------------------------------------

inline std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

template <typename T>
T get_or(const json& j, const char* key, const T& def) {
    if (j.contains(key))
        return j.at(key).get<T>();
    return def;
}

io::Vec3f vec3_from(const json& arr, const char* ctx) {
    if (!arr.is_array() || arr.size() != 3)
        throw std::runtime_error(std::string("Expected 3-length array for Vec3 at ") + ctx);
    return io::Vec3f{arr.at(0).get<float>(), arr.at(1).get<float>(), arr.at(2).get<float>()};
}

io::Color3f color3_from(const json& arr, const char* ctx) {
    auto v = vec3_from(arr, ctx);
    return io::Color3f{v.x, v.y, v.z};
}

io::MaterialType parse_material_type(const std::string& s) {
    auto t = to_lower(s);
    if (t == "lambertian" || t == "diffuse")
        return io::MaterialType::Lambertian;
    if (t == "metal")
        return io::MaterialType::Metal;
    if (t == "dielectric" || t == "glass")
        return io::MaterialType::Dielectric;
    if (t == "checker")
        return io::MaterialType::Checker;
    throw std::runtime_error("Unknown material type: " + s);
}

io::ObjectType parse_object_type(const std::string& s) {
    auto t = to_lower(s);
    if (t == "sphere")
        return io::ObjectType::Sphere;
    if (t == "plane")
        return io::ObjectType::Plane;
    throw std::runtime_error("Unknown object type: " + s);
}

io::LightType parse_light_type(const std::string& s) {
    auto t = to_lower(s);
    if (t == "point" || t == "pointlight")
        return io::LightType::Point;
    throw std::runtime_error("Unknown light type: " + s);
}

std::shared_ptr<Material> parse_material_inline(const json& m) {
    if (!m.contains("type")) {
        return nullptr;
    }

    std::string type = m["type"].get<std::string>();

    if (type == "lambertian") {
        if (m.contains("albedo")) {
            auto albedo = color3_from(m["albedo"], "albedo");
            return std::make_shared<Lambertian>(Color(albedo.r, albedo.g, albedo.b));
        }
        return std::make_shared<Lambertian>(Color(0.8f, 0.8f, 0.8f));
    } else if (type == "metal") {
        auto albedo = m.contains("albedo") ? color3_from(m["albedo"], "albedo")
                                           : io::Color3f{0.8f, 0.8f, 0.8f};
        float fuzz = m.contains("fuzz") ? m["fuzz"].get<float>() : 0.0f;
        return std::make_shared<Metal>(Color(albedo.r, albedo.g, albedo.b), fuzz);
    } else if (type == "checker") {
        auto c1 = m.contains("color1") ? color3_from(m["color1"], "color1")
                                       : io::Color3f{0.8f, 0.8f, 0.8f};
        auto c2 = m.contains("color2") ? color3_from(m["color2"], "color2")
                                       : io::Color3f{0.2f, 0.2f, 0.2f};
        float scale = m.contains("scale") ? m["scale"].get<float>() : 1.0f;
        return std::make_shared<Checker>(Color(c1.r, c1.g, c1.b), Color(c2.r, c2.g, c2.b), scale);
    } else if (type == "dielectric" || type == "glass") {
        float ior = m.contains("ior") ? m["ior"].get<float>() : 1.5f;
        return std::make_shared<Dielectric>(ior);
    }

    return nullptr;
}

}  // anonymous namespace

namespace io {

SceneDTO JsonSceneLoader::load_from_file(const std::string& path) {
    std::ifstream ifs(path);
    if (!ifs) {
        Logger::error("Failed to open scene file: " + path);
        throw std::runtime_error("Cannot open file: " + path);
    }
    std::ostringstream oss;
    oss << ifs.rdbuf();
    const auto text = oss.str();
    return parse_json_string(text, path);
}

SceneDTO JsonSceneLoader::parse_json_string(const std::string& json_text,
                                            const std::string& origin_hint) {
    json j;
    try {
        j = json::parse(json_text);
    } catch (const std::exception& e) {
        Logger::error(std::string("JSON parse error (") + origin_hint + "): " + e.what());
        throw;
    }

    SceneDTO scene;

    // ---- image ----
    if (j.contains("image")) {
        const auto& ji = j.at("image");
        scene.image.width = get_or<int>(ji, "width", 800);
        scene.image.height = get_or<int>(ji, "height", 450);
        scene.image.samples = get_or<int>(ji, "samples", 1);
        scene.image.max_depth = get_or<int>(ji, "max_depth", 4);
        scene.image.output_path = get_or<std::string>(ji, "output", "output/render.png");

        if (scene.image.width <= 0 || scene.image.height <= 0)
            throw std::runtime_error("Image width/height must be > 0");
        if (scene.image.samples <= 0) {
            Logger::warn("Image.samples <= 0; clamping to 1");
            scene.image.samples = 1;
        }
        if (scene.image.max_depth < 0) {
            Logger::warn("Image.max_depth < 0; clamping to 0");
            scene.image.max_depth = 0;
        }
    } else {
        Logger::warn("Missing 'image' block; using defaults.");
    }

    // ---- camera ----
    if (j.contains("camera")) {
        const auto& jc = j.at("camera");
        // Support both "look_from" and "position" for camera position
        if (jc.contains("look_from"))
            scene.camera.look_from = vec3_from(jc.at("look_from"), "camera.look_from");
        else if (jc.contains("position"))
            scene.camera.look_from = vec3_from(jc.at("position"), "camera.position");
        if (jc.contains("look_at"))
            scene.camera.look_at = vec3_from(jc.at("look_at"), "camera.look_at");
        if (jc.contains("up"))
            scene.camera.up = vec3_from(jc.at("up"), "camera.up");
        // Support both "vfov" and "fov" for field of view
        if (jc.contains("vfov"))
            scene.camera.vfov_deg = jc.at("vfov").get<float>();
        else if (jc.contains("fov"))
            scene.camera.vfov_deg = jc.at("fov").get<float>();
        scene.camera.aperture = get_or<float>(jc, "aperture", 0.f);
        scene.camera.focus_dist = get_or<float>(jc, "focus_dist", 1.f);
        if (scene.camera.vfov_deg <= 1.f || scene.camera.vfov_deg >= 179.f) {
            Logger::warn("Camera.vfov out of typical range; check degrees value.");
        }
    } else {
        Logger::warn("Missing 'camera' block; using defaults.");
    }

    // ---- materials ----
    if (j.contains("materials")) {
        const auto& jm = j.at("materials");
        if (!jm.is_object())
            throw std::runtime_error("'materials' must be an object (id -> material)");

        for (auto it = jm.begin(); it != jm.end(); ++it) {
            const std::string id = it.key();
            const auto& m = it.value();
            if (!m.contains("type"))
                throw std::runtime_error("Material '" + id + "' is missing field 'type'");

            MaterialDTO md;
            md.id = id;
            md.type = parse_material_type(m.at("type").get<std::string>());
            if (md.type == MaterialType::Lambertian || md.type == MaterialType::Metal) {
                if (m.contains("albedo"))
                    md.albedo =
                        color3_from(m.at("albedo"), ("materials." + id + ".albedo").c_str());
            }
            if (md.type == MaterialType::Metal) {
                md.roughness = get_or<float>(m, "roughness", 0.0f);
                if (md.roughness < 0.f)
                    md.roughness = 0.f;
                if (md.roughness > 1.f)
                    md.roughness = 1.f;
            }
            if (md.type == MaterialType::Dielectric) {
                md.ior = get_or<float>(m, "ior", 1.5f);
                if (md.ior < 1.0f) {
                    Logger::warn("Dielectric ior < 1.0; clamping to 1.0 for material '" + id + "'");
                    md.ior = 1.0f;
                }
            }
            scene.materials.emplace(id, md);
        }
    } else {
        Logger::warn("No 'materials' block; objects must reference built-ins or will fail later.");
    }

    // ---- objects ----
    if (j.contains("objects")) {
        const auto& jo = j.at("objects");
        if (!jo.is_array())
            throw std::runtime_error("'objects' must be an array");
        scene.objects.reserve(jo.size());

        for (const auto& o : jo) {
            if (!o.contains("type"))
                throw std::runtime_error("Object missing 'type' field");
            const auto type = parse_object_type(o.at("type").get<std::string>());

            ObjectDTO obj;
            obj.type = type;

            switch (type) {
                case ObjectType::Sphere: {
                    if (!o.contains("center") || !o.contains("radius") || !o.contains("material"))
                        throw std::runtime_error("Sphere requires 'center', 'radius', 'material'");
                    obj.sphere.center = vec3_from(o.at("center"), "objects[*].center");
                    obj.sphere.radius = o.at("radius").get<float>();
                    // Support both string (material_id) and object (inline material)
                    if (o.at("material").is_string()) {
                        obj.sphere.material_id = o.at("material").get<std::string>();
                    } else if (o.at("material").is_object()) {
                        // Inline material: create a unique ID and add to materials map
                        std::string inline_id = "__inline_" + std::to_string(scene.objects.size());
                        const auto& mat_obj = o.at("material");
                        MaterialDTO md;
                        md.id = inline_id;
                        md.type = parse_material_type(mat_obj.at("type").get<std::string>());
                        if (mat_obj.contains("albedo")) {
                            md.albedo = color3_from(mat_obj.at("albedo"), "albedo");
                        }
                        if (md.type == MaterialType::Metal && mat_obj.contains("fuzz")) {
                            md.roughness = mat_obj.at("fuzz").get<float>();
                        } else if (md.type == MaterialType::Metal &&
                                   mat_obj.contains("roughness")) {
                            md.roughness = mat_obj.at("roughness").get<float>();
                        }
                        if (md.type == MaterialType::Dielectric && mat_obj.contains("ior")) {
                            md.ior = mat_obj.at("ior").get<float>();
                        }
                        if (md.type == MaterialType::Checker) {
                            if (mat_obj.contains("color1")) {
                                md.albedo = color3_from(mat_obj.at("color1"), "color1");
                            }
                            if (mat_obj.contains("color2")) {
                                md.color2 = color3_from(mat_obj.at("color2"), "color2");
                            }
                            if (mat_obj.contains("scale")) {
                                md.scale = mat_obj.at("scale").get<float>();
                            }
                        }
                        scene.materials.emplace(inline_id, md);
                        obj.sphere.material_id = inline_id;
                    } else {
                        throw std::runtime_error(
                            "Sphere.material must be a string (id) or object (inline)");
                    }
                    if (obj.sphere.radius <= 0.f)
                        throw std::runtime_error("Sphere.radius must be > 0");
                } break;
                case ObjectType::Plane: {
                    if (!o.contains("point") || !o.contains("normal") || !o.contains("material"))
                        throw std::runtime_error("Plane requires 'point', 'normal', 'material'");
                    obj.plane.point = vec3_from(o.at("point"), "objects[*].point");
                    obj.plane.normal = vec3_from(o.at("normal"), "objects[*].normal");
                    // Support both string (material_id) and object (inline material)
                    if (o.at("material").is_string()) {
                        obj.plane.material_id = o.at("material").get<std::string>();
                    } else if (o.at("material").is_object()) {
                        // Inline material: create a unique ID and add to materials map
                        std::string inline_id = "__inline_" + std::to_string(scene.objects.size());
                        const auto& mat_obj = o.at("material");
                        MaterialDTO md;
                        md.id = inline_id;
                        md.type = parse_material_type(mat_obj.at("type").get<std::string>());
                        if (mat_obj.contains("albedo")) {
                            md.albedo = color3_from(mat_obj.at("albedo"), "albedo");
                        }
                        if (md.type == MaterialType::Metal && mat_obj.contains("fuzz")) {
                            md.roughness = mat_obj.at("fuzz").get<float>();
                        } else if (md.type == MaterialType::Metal &&
                                   mat_obj.contains("roughness")) {
                            md.roughness = mat_obj.at("roughness").get<float>();
                        }
                        if (md.type == MaterialType::Dielectric && mat_obj.contains("ior")) {
                            md.ior = mat_obj.at("ior").get<float>();
                        }
                        if (md.type == MaterialType::Checker) {
                            if (mat_obj.contains("color1")) {
                                md.albedo = color3_from(mat_obj.at("color1"), "color1");
                            }
                            if (mat_obj.contains("color2")) {
                                md.color2 = color3_from(mat_obj.at("color2"), "color2");
                            }
                            if (mat_obj.contains("scale")) {
                                md.scale = mat_obj.at("scale").get<float>();
                            }
                        }
                        scene.materials.emplace(inline_id, md);
                        obj.plane.material_id = inline_id;
                    } else {
                        throw std::runtime_error(
                            "Plane.material must be a string (id) or object (inline)");
                    }
                } break;
            }

            // Soft-check material existence to help users early (not fatal: some pipelines add built-ins).
            if (!obj.sphere.material_id.empty()) {
                if (!scene.materials.count(obj.sphere.material_id)) {
                    Logger::warn("Object references unknown material id: " +
                                 obj.sphere.material_id);
                }
            }
            if (!obj.plane.material_id.empty()) {
                if (!scene.materials.count(obj.plane.material_id)) {
                    Logger::warn("Object references unknown material id: " + obj.plane.material_id);
                }
            }

            scene.objects.push_back(std::move(obj));
        }
    } else {
        Logger::warn("No 'objects' block; scene will be empty.");
    }

    // ---- lights ----
    if (j.contains("lights")) {
        const auto& jl = j.at("lights");
        if (!jl.is_array())
            throw std::runtime_error("'lights' must be an array");
        scene.lights.reserve(jl.size());

        for (const auto& l : jl) {
            if (!l.contains("type"))
                throw std::runtime_error("Light missing 'type' field");
            const auto type = parse_light_type(l.at("type").get<std::string>());

            LightDTO ld;
            ld.type = type;

            switch (type) {
                case LightType::Point: {
                    if (!l.contains("position"))
                        throw std::runtime_error("PointLight requires 'position'");
                    ld.point.position = vec3_from(l.at("position"), "lights[*].position");
                    if (l.contains("intensity"))
                        ld.point.intensity = color3_from(l.at("intensity"), "lights[*].intensity");
                } break;
            }

            scene.lights.push_back(ld);
        }
    } else {
        Logger::warn("No 'lights' block; only ambient/emit materials may contribute if supported.");
    }

    Logger::info("Loaded scene from: " + origin_hint);
    return scene;
}

// Compatibility methods for old API
bool JsonSceneLoader::loadFromString(const std::string& json_text, ::Scene& scene,
                                     std::string* err) {
    try {
        json j = json::parse(json_text);

        // Parse objects
        if (j.contains("objects") && j["objects"].is_array()) {
            for (const auto& obj : j["objects"]) {
                if (!obj.contains("type"))
                    continue;
                std::string type = obj["type"].get<std::string>();

                std::shared_ptr<Material> mat = nullptr;
                if (obj.contains("material")) {
                    mat = parse_material_inline(obj["material"]);
                }

                if (type == "plane") {
                    if (obj.contains("point") && obj.contains("normal")) {
                        auto point = vec3_from(obj["point"], "point");
                        auto normal = vec3_from(obj["normal"], "normal");
                        auto plane = std::make_shared<Plane>(Point3(point.x, point.y, point.z),
                                                             Vec3(normal.x, normal.y, normal.z));
                        if (mat) {
                            scene.add(plane, mat);
                        } else {
                            scene.add(plane);
                        }
                    }
                } else if (type == "sphere") {
                    if (obj.contains("center") && obj.contains("radius")) {
                        auto center = vec3_from(obj["center"], "center");
                        float radius = obj["radius"].get<float>();
                        auto sphere =
                            std::make_shared<Sphere>(Point3(center.x, center.y, center.z), radius);
                        if (mat) {
                            scene.add(sphere, mat);
                        } else {
                            scene.add(sphere);
                        }
                    }
                } else if (type == "triangle") {
                    if (obj.contains("a") && obj.contains("b") && obj.contains("c")) {
                        auto a = vec3_from(obj["a"], "a");
                        auto b = vec3_from(obj["b"], "b");
                        auto c = vec3_from(obj["c"], "c");
                        auto tri = std::make_shared<Triangle>(
                            Point3(a.x, a.y, a.z), Point3(b.x, b.y, b.z), Point3(c.x, c.y, c.z));
                        if (mat) {
                            scene.add(tri, mat);
                        } else {
                            scene.add(tri);
                        }
                    }
                }
            }
        }
        return true;
    } catch (const std::exception& e) {
        if (err)
            *err = e.what();
        return false;
    }
}

bool JsonSceneLoader::loadFromString(const std::string& json_text, ::Scene& scene, ::Camera& camera,
                                     std::string* err) {
    try {
        json j = json::parse(json_text);

        // Parse camera
        if (j.contains("camera")) {
            const auto& cam = j["camera"];
            if (cam.contains("position")) {
                auto pos = vec3_from(cam["position"], "camera.position");
                camera.position = Point3(pos.x, pos.y, pos.z);
            }
            if (cam.contains("look_at")) {
                auto look = vec3_from(cam["look_at"], "camera.look_at");
                camera.look_at = Point3(look.x, look.y, look.z);
            }
            if (cam.contains("up")) {
                auto up_vec = vec3_from(cam["up"], "camera.up");
                camera.up = Vec3(up_vec.x, up_vec.y, up_vec.z);
            }
            if (cam.contains("fov")) {
                camera.fov = cam["fov"].get<float>();
            }
            if (cam.contains("aspect_ratio")) {
                camera.aspect_ratio = cam["aspect_ratio"].get<float>();
            }
        }

        // Parse objects (reuse the other method)
        return loadFromString(json_text, scene, err);
    } catch (const std::exception& e) {
        if (err)
            *err = e.what();
        return false;
    }
}

void JsonSceneLoader::populateScene(const SceneDTO& dto, ::Scene& scene, ::Camera& camera) {
    // Populate camera
    camera.position =
        Point3(dto.camera.look_from.x, dto.camera.look_from.y, dto.camera.look_from.z);
    camera.look_at = Point3(dto.camera.look_at.x, dto.camera.look_at.y, dto.camera.look_at.z);
    camera.up = Vec3(dto.camera.up.x, dto.camera.up.y, dto.camera.up.z);
    camera.fov = dto.camera.vfov_deg;
    camera.aspect_ratio =
        static_cast<float>(dto.image.width) / static_cast<float>(dto.image.height);
    camera.initialize();

    // Create materials map
    std::unordered_map<std::string, std::shared_ptr<Material>> material_map;
    for (const auto& [id, md] : dto.materials) {
        std::shared_ptr<Material> mat;
        switch (md.type) {
            case MaterialType::Lambertian:
                mat = std::make_shared<Lambertian>(Color(md.albedo.r, md.albedo.g, md.albedo.b));
                break;
            case MaterialType::Metal:
                mat = std::make_shared<Metal>(Color(md.albedo.r, md.albedo.g, md.albedo.b),
                                              md.roughness);
                break;
            case MaterialType::Dielectric:
                mat = std::make_shared<Dielectric>(md.ior);
                break;
            case MaterialType::Checker:
                mat = std::make_shared<Checker>(Color(md.albedo.r, md.albedo.g, md.albedo.b),
                                                Color(md.color2.r, md.color2.g, md.color2.b),
                                                md.scale);
                break;
        }
        material_map[id] = mat;
    }

    // Populate scene with objects
    for (const auto& obj : dto.objects) {
        std::shared_ptr<Material> mat = nullptr;
        if (!obj.sphere.material_id.empty()) {
            auto it = material_map.find(obj.sphere.material_id);
            if (it != material_map.end()) {
                mat = it->second;
            }
        } else if (!obj.plane.material_id.empty()) {
            auto it = material_map.find(obj.plane.material_id);
            if (it != material_map.end()) {
                mat = it->second;
            }
        }

        switch (obj.type) {
            case ObjectType::Sphere: {
                Point3 center(obj.sphere.center.x, obj.sphere.center.y, obj.sphere.center.z);
                scene.add(std::make_shared<Sphere>(center, obj.sphere.radius), mat);
            } break;
            case ObjectType::Plane: {
                Point3 point(obj.plane.point.x, obj.plane.point.y, obj.plane.point.z);
                Vec3 normal(obj.plane.normal.x, obj.plane.normal.y, obj.plane.normal.z);
                scene.add(std::make_shared<Plane>(point, normal), mat);
            } break;
        }
    }
}

}  // namespace io
