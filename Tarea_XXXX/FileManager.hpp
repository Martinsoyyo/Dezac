#pragma once
#include "Libraries.hpp"

#include "XML.hpp"
#include "Json.hpp"
#include "Log.hpp"

class FileManager {
public:
	FileManager();

	static std::optional<std::vector<char>> Load(std::string_view filename);
	static bool Store(std::string_view filename, std::string_view content);
	static bool SendToMicroSD(std::string_view filename, std::string_view content);
};

/////////////////////////////////////////////////
inline FileManager FILEMANAGER; /// OBJETO GLOBAL
/////////////////////////////////////////////////
