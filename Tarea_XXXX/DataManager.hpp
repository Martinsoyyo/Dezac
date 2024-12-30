#pragma once
#include "Libraries.hpp"

#include "XML.hpp"
#include "Json.hpp"
#include "Log.hpp"
#include "FileManager.hpp"


class DataManager {
public:
	DataManager(std::string_view);

	void Export(std::string_view filename, std::string_view privilege) const;
	void Import(std::string_view filename, std::string_view privilege);
	void Store(std::string_view filename) const;

	const Value& GetJSON(std::string_view filename, std::initializer_list<std::string_view> location = {}) const;
	const std::string GetString(std::string_view filename, std::initializer_list<std::string_view> location) const;
	float GetValue(std::string_view filename, std::initializer_list<std::string_view> location) const;

	const Document& operator[](std::string_view filename) const { return Find_JSON_By_Name(filename); };


private:
	std::vector < std::tuple<std::string, Document >> jsonfiles_;


private:
	void Read_Config_And_Parse_Files(const Value& doc);
    Document Create_JSON_From_String(std::string_view);
	const Document& Find_JSON_By_Name(std::string_view filename) const;
};

////////////////////////////////////////////////////////////////
inline DataManager DATAMANAGER("Config.json"); /// OBJETO GLOBAL
////////////////////////////////////////////////////////////////
