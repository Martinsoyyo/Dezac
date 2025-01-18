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

	const Value& Get_JSON(std::string_view filename, std::initializer_list<std::string_view> location = {}) const;
	const float Get_Value(std::string_view filename, std::initializer_list<std::string_view> location) const;
	const std::string Get_String(std::string_view filename, std::initializer_list<std::string_view> location) const;
	const Document& operator[](std::string_view filename) const { return Find_JSON_By_Name(filename); };


private:
	std::vector < std::tuple<std::string, Document >> jsonfiles_;


private:
	static Document Create_JSON_From_String(std::string_view);
	void Read_Config_And_Parse_Files(const Value& doc);
	const Document& Find_JSON_By_Name(std::string_view filename) const;
};

////////////////////////////////////////////////////////////////
inline DataManager DATAMANAGER("Config.json"); /// OBJETO GLOBAL
////////////////////////////////////////////////////////////////
