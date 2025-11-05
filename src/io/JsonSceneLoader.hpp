#pragma once

#include <string>

#include "core/Scene.hpp"

namespace io {
	class JsonSceneLoader {
	public:
		static bool loadFromString(const std::string& jsonText, Scene& outScene, std::string* outError = nullptr);
		static bool loadFromFile(const std::string& filePath, Scene& outScene, std::string* outError = nullptr);
	};
}
