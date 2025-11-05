#include "io/JsonSceneLoader.hpp"

#include <fstream>
#include <sstream>

#include <nlohmann/json.hpp>

#include "entities/Plane.hpp"
#include "entities/Triangle.hpp"

using nlohmann::json;

namespace {
	static bool parseVec3(const json& j, const char* key, Vec3& out) {
		if (!j.contains(key) || !j[key].is_array() || j[key].size() != 3) return false;
		out.x = j[key][0].get<float>();
		out.y = j[key][1].get<float>();
		out.z = j[key][2].get<float>();
		return true;
	}
}

namespace io {
	static bool parseObjects(const json& j, Scene& outScene, std::string* outError) {
		if (!j.contains("objects") || !j["objects"].is_array()) {
			if (outError) *outError = "Missing 'objects' array";
			return false;
		}
		for (const auto& obj : j["objects"]) {
			if (!obj.contains("type") || !obj["type"].is_string()) continue;
			std::string type = obj["type"].get<std::string>();
			if (type == "plane") {
				Vec3 p(0,0,0), n(0,1,0);
				if (!parseVec3(obj, "point", p)) { if (outError) *outError = "plane.point invalid"; return false; }
				if (!parseVec3(obj, "normal", n)) { if (outError) *outError = "plane.normal invalid"; return false; }
				auto plane = std::make_shared<Plane>(p, n);
				outScene.add(plane);
			} else if (type == "triangle") {
				Vec3 a, b, c;
				if (!parseVec3(obj, "a", a)) { if (outError) *outError = "triangle.a invalid"; return false; }
				if (!parseVec3(obj, "b", b)) { if (outError) *outError = "triangle.b invalid"; return false; }
				if (!parseVec3(obj, "c", c)) { if (outError) *outError = "triangle.c invalid"; return false; }
				auto tri = std::make_shared<Triangle>(a, b, c);
				outScene.add(tri);
			}
		}
		return true;
	}

	static bool parseCamera(const json& j, Camera& outCamera, std::string* outError) {
		if (!j.contains("camera") || !j["camera"].is_object()) {
			return true; // optional
		}
		const json& jc = j["camera"];
		Vec3 pos(0,0,0), target(0,0,-1), up(0,1,0);
		float fov = 90.0f;
		float aspect = 16.0f/9.0f;
		if (!parseVec3(jc, "position", pos)) { if (outError) *outError = "camera.position invalid"; return false; }
		if (!parseVec3(jc, "look_at", target)) { if (outError) *outError = "camera.look_at invalid"; return false; }
		if (jc.contains("up")) {
			if (!parseVec3(jc, "up", up)) { if (outError) *outError = "camera.up invalid"; return false; }
		}
		if (jc.contains("fov")) fov = jc["fov"].get<float>();
		if (jc.contains("aspect_ratio")) aspect = jc["aspect_ratio"].get<float>();
		outCamera = Camera(pos, target, up, fov, aspect);
		return true;
	}

	bool JsonSceneLoader::loadFromString(const std::string& jsonText, Scene& outScene, std::string* outError) {
		try {
			json j = json::parse(jsonText);
			if (!parseObjects(j, outScene, outError)) return false;
			return true;
		} catch (const std::exception& e) {
			if (outError) *outError = e.what();
			return false;
		}
	}

	bool JsonSceneLoader::loadFromFile(const std::string& filePath, Scene& outScene, std::string* outError) {
		std::ifstream in(filePath);
		if (!in) {
			if (outError) *outError = "Cannot open file";
			return false;
		}
		std::ostringstream ss; ss << in.rdbuf();
		return loadFromString(ss.str(), outScene, outError);
	}

	bool JsonSceneLoader::loadFromString(const std::string& jsonText, Scene& outScene, Camera& outCamera, std::string* outError) {
		try {
			json j = json::parse(jsonText);
			if (!parseObjects(j, outScene, outError)) return false;
			if (!parseCamera(j, outCamera, outError)) return false;
			return true;
		} catch (const std::exception& e) {
			if (outError) *outError = e.what();
			return false;
		}
	}

	bool JsonSceneLoader::loadFromFile(const std::string& filePath, Scene& outScene, Camera& outCamera, std::string* outError) {
		std::ifstream in(filePath);
		if (!in) {
			if (outError) *outError = "Cannot open file";
			return false;
		}
		std::ostringstream ss; ss << in.rdbuf();
		return loadFromString(ss.str(), outScene, outCamera, outError);
	}
}
