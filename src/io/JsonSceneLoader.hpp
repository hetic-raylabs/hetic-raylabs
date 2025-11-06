#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

// Forward declarations
class Scene;
class Camera;

namespace io {

struct Vec3f {
    float x = 0.f, y = 0.f, z = 0.f;
};

struct Color3f {
    float r = 0.f, g = 0.f, b = 0.f;
};

struct CameraDTO {
    Vec3f look_from{0, 0, 0};
    Vec3f look_at{0, 0, -1};
    Vec3f up{0, 1, 0};
    float vfov_deg = 45.f;
    float aperture = 0.f;
    float focus_dist = 1.f;
};

struct ImageDTO {
    int width = 800;
    int height = 450;
    int samples = 1;
    int max_depth = 4;
    std::string output_path = "output/render.png";
};

enum class MaterialType { Lambertian, Metal, Dielectric };

struct MaterialDTO {
    std::string id;  // user-defined handle
    MaterialType type = MaterialType::Lambertian;
    Color3f albedo{0.8f, 0.8f, 0.8f};  // for Lambertian/Metal
    float roughness = 0.0f;            // for Metal [0..1]
    float ior = 1.5f;                  // for Dielectric
};

enum class ObjectType { Sphere, Plane };

struct SphereDTO {
    Vec3f center{0, 0, 0};
    float radius = 1.f;
    std::string material_id;
};

struct PlaneDTO {
    Vec3f point{0, 0, 0};
    Vec3f normal{0, 1, 0};  // should be normalized by the renderer if needed
    std::string material_id;
};

struct ObjectDTO {
    ObjectType type = ObjectType::Sphere;
    SphereDTO sphere;  // valid if type==Sphere
    PlaneDTO plane;    // valid if type==Plane
};

enum class LightType { Point };

struct PointLightDTO {
    Vec3f position{0, 0, 0};
    Color3f intensity{1, 1, 1};  // radiometric-ish RGB
};

struct LightDTO {
    LightType type = LightType::Point;
    PointLightDTO point;
};

struct SceneDTO {
    CameraDTO camera;
    ImageDTO image;
    std::unordered_map<std::string, MaterialDTO> materials;  // by id
    std::vector<ObjectDTO> objects;
    std::vector<LightDTO> lights;
};

class JsonSceneLoader {
   public:
    /// Parse a JSON scene file into strongly-typed DTOs.
    /// Throws std::runtime_error on invalid input.
    static SceneDTO load_from_file(const std::string& path);

    /// Parse a JSON string into strongly-typed DTOs.
    /// Throws std::runtime_error on invalid input.
    static SceneDTO parse_json_string(const std::string& json_text, const std::string& origin_hint = "string");

    // Compatibility methods for old API (for tests)
    /// Legacy: Load from string and populate Scene (for tests)
    static bool loadFromString(const std::string& json_text, ::Scene& scene, std::string* err = nullptr);
    /// Legacy: Load from string and populate Scene and Camera (for tests)
    static bool loadFromString(const std::string& json_text, ::Scene& scene, ::Camera& camera, std::string* err = nullptr);

   private:
    // disallow instance
    JsonSceneLoader() = delete;
};

}  // namespace io
