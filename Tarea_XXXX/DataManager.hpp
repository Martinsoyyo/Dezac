#pragma once
#include "Libraries.hpp"
#include "Log.hpp"

class DataManager {
public:
	void Init(const Value& doc);

	void Export(std::string_view filename, std::string_view privilege) const;
	void Import(std::string_view filename, std::string_view privilege);

	const Value& Get_JSON(std::string_view filename, std::initializer_list<std::string_view> location = {}) const;
	const float Get_Value(std::string_view filename, std::initializer_list<std::string_view> location) const;
	const std::string Get_String(std::string_view filename, std::initializer_list<std::string_view> location) const;
	const Document& operator[](std::string_view filename) const { return Find_JSON_By_Name(filename); };

	static Document Create_JSON_From_String(std::string_view);


private:
	std::vector < std::tuple<std::string, Document >> jsonfiles_;

private:
	void Read_Config_And_Parse_Files(const Value& doc);
	const Document& Find_JSON_By_Name(std::string_view filename) const;
};

