#include "DataManager.hpp"
#include "XML.hpp"
#include "Json.hpp"
#include "FileManager.hpp"
#include "App.hpp"

void DataManager::Init(const Value& doc)
{
	LOG("DataManager::Init(...)\n");
	Read_Config_And_Parse_Files(doc);

	// Datatype.json is SPECIAL, we need to pass to <Json class> in Lazy Initialization.
	const Value& datatype = Find_JSON_By_Name("Datatypes.json");
	Json::Datatype(datatype);
}

Document DataManager::Create_JSON_From_String(std::string_view str)
{
	Document doc;
	doc.Parse(str.data());
	if (doc.HasParseError()) {
		LOG("[ERROR] Semantics Errors in JSON.\n");
		return {};
	}

	if (doc.IsNull()) {
		LOG("[ERROR] Uninitialized Json document.\n");
		return {};
	}

	if (doc.IsObject() && doc.ObjectEmpty()) {
		LOG("[ERROR] Config.json Document is empty.\n");
		return {};
	}

	return doc;  // Named Return Value Optimization (NRVO)
}

void DataManager::Read_Config_And_Parse_Files(const Value& doc)
{
	if (doc.HasMember("Files") && doc["Files"].IsArray())
	{
		const Value& filesArray = doc["Files"];
		jsonfiles_.reserve(filesArray.Size());

		for (SizeType i = 0; i < filesArray.Size(); ++i)
		{
			if (filesArray[i].IsString())
			{
				auto filename = filesArray[i].GetString();
				LOG("Loading JSON file %s\n", filename);

				auto str = APP::FILEMANAGER.Read(filename);
				jsonfiles_.emplace_back(filename, Create_JSON_From_String(str.value().data()));
			}
		}
	}
	else {
		LOG("[ERROR] The files could not be loaded from the JSON.\n");
		return;
	}
}

const Document& DataManager::Find_JSON_By_Name(std::string_view filename) const
{
	for (const auto& [file, doc] : jsonfiles_) {
		if (file == filename) {
			return doc;
		}
	}

	static Document doc;
	LOG("[ERROR] Filename %s not found. \n", filename);
	return doc;
}

const Value& DataManager::Get_JSON(std::string_view filename, std::initializer_list<std::string_view> location) const
{
	const Value* currentValue = &Find_JSON_By_Name(filename);

	for (const auto& key : location) {
		if (currentValue->IsObject() && currentValue->HasMember(key.data())) {
			currentValue = &(*currentValue)[key.data()];
		}
		else {
			LOG("[ERROR] The node does not exist.", location);
		}
	}

	return *currentValue;
}

const std::string DataManager::Get_String(std::string_view filename, std::initializer_list<std::string_view> location = {}) const
{
	const Value& subJSON = Get_JSON(filename, location);
	if (!subJSON.IsString()) {
		LOG("[ERROR] The STRING does not exist.", location);
		return {};
	}

	return subJSON.GetString();
}

const float DataManager::Get_Value(std::string_view filename, std::initializer_list<std::string_view> location) const
{
	const Value& subJSON = Get_JSON(filename, location);
	if (Json::Get_Variable_Type(subJSON) != VariableType::Number)
	{
		LOG("[ERROR] Type mismatch: expected numeric", location);
		return {};
	}

	auto num = std::stof(subJSON["Value"].GetString());

	auto [type, subtype] = Json::Extract_Type_And_Subtype(subJSON["Type"].GetString());
	auto scale = (subtype == "") ?
		Get_JSON("Datatypes.json", { type, "Scale" }).GetFloat() :
		Get_JSON("Datatypes.json", { type, subtype, "Scale" }).GetFloat();

	return num * scale;
}

void DataManager::Export(std::string_view filename, std::string_view privilege) const
{
	std::string file = Get_String("Config.json", { "DataManager",filename ,"ViewPrivileges" ,privilege ,"File" });
	std::string fileToXML = Get_String("Config.json", { "DataManager",filename ,"Path", "AdressToXML" }) + file + ".xml";
	std::string fileToXSD = Get_String("Config.json", { "DataManager",filename ,"Path", "AdressToXSD" }) + file + ".xsd";

	Document& doc = const_cast<Document&>(Find_JSON_By_Name(filename));
	Json obj(doc, privilege);
	obj.Export(fileToXML, fileToXSD);
}

void DataManager::Import(std::string_view filename, std::string_view privilege)
{
	std::string file = Get_String("Config.json", { "DataManager",filename ,"ViewPrivileges" ,privilege ,"File" });
	std::string fileFromXML = Get_String("Config.json", { "DataManager",filename ,"Path", "AdressToXML" }) + file + ".xml";

	Document& doc = const_cast<Document&>(Find_JSON_By_Name(filename));
	Json obj(doc, privilege);
	obj.Import(fileFromXML);
}
