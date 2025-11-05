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
	bool JsonSceneLoader::loadFromString(const std::string& jsonText, Scene& outScene, std::string* outError) {
		try {
			json j = json::parse(jsonText);
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
}
