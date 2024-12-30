#include "DataManager.hpp"

DataManager::DataManager(std::string_view filename)
{
	LOG("Create DataManager::DataManager(...)\n");

	auto str = FileManager::Load(filename);
	if (!str.has_value()) {
		LOG("[ERROR] Error loading JSON file : %s\n", filename.data());
		return;
	}

	Document doc = Create_JSON_From_String(str.value().data());

	if (doc.IsNull()) {
		LOG("[ERROR] Uninitialized Json document.\n");
		return;
	}

	if (doc.IsObject() && doc.ObjectEmpty()) {
		LOG("[ERROR] Json Document is empty.\n");
		return;
	}

	if (doc.HasMember("DataManager") && doc["DataManager"].IsObject()) {
		Read_Config_And_Parse_Files(doc["DataManager"]);
	}
	
}

Document DataManager::Create_JSON_From_String(std::string_view str)
{
	Document doc;
	doc.Parse(str.data());
	if (doc.HasParseError()) {
		LOG("[ERROR] Semantics Errors in JSON.\n");
		return {};
	}

	return std::move(doc);
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

				auto str = FileManager::Load(filename);
				if (!str.has_value()) {
					LOG("[ERROR] Error loading JSON file : %s\n", filename);
					return;
				}

				jsonfiles_.emplace_back(filename, Create_JSON_From_String(str.value().data()));
			}
		}
	}
	else {
		LOG("[ERROR] The files could not be loaded from the JSON.\n");
		return;
	}

	// Datatype.json is SPECIAL we need to pass this JSON in Lazy Initialization.
	const Value& datatype = Find_JSON_By_Name("Datatypes.json");
	Json::GetDatatype(datatype);

}

const Document& DataManager::Find_JSON_By_Name(std::string_view filename) const
{
	for (const auto& [file, doc] : jsonfiles_) {
		if (file == filename) {
			return doc;
		}
	}

	LOG("[ERROR] Filename %s not found. \n", filename);
	static Document doc{};
	return doc;
}

const Value& DataManager::GetJSON(std::string_view filename, std::initializer_list<std::string_view> location) const
{
	const Document& doc = Find_JSON_By_Name(filename);
	const Value* currentValue = &doc;

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

const std::string DataManager::GetString(std::string_view filename, std::initializer_list<std::string_view> location = {}) const
{
	const Value& subJSON = DATAMANAGER.GetJSON(filename, location);
	if (!subJSON.IsString()) {
		LOG("[ERROR] The STRING does not exist.", location);
		return {};
	}

	return subJSON.GetString();
}

float DataManager::GetValue(std::string_view filename, std::initializer_list<std::string_view> location) const
{
	const Value& subJSON = DATAMANAGER.GetJSON(filename, location);

	if (Json::DetermineVariableType(subJSON) != VariableType::Number)
	{
		LOG("[ERROR] Type mismatch: expected numeric", location);
		return {};
	}

	auto num = std::stof(subJSON["Value"].GetString());
	auto [type, subtype] = Json::GetTypeAndSubtype(subJSON["Type"].GetString());

	auto scale = (subtype == "") ?
		GetJSON("Datatypes.json", { type, "Scale" }).GetFloat():
		GetJSON("Datatypes.json", { type, subtype, "Scale" }).GetFloat();

	return num * scale;
}

void DataManager::Export(std::string_view filename, std::string_view privilege) const
{
	std::string file = GetString("Config.json", { "DataManager",filename ,"ViewPrivileges" ,privilege ,"File" });
	std::string fileToXML = GetString("Config.json", { "DataManager",filename ,"Path", "AdressToXML" }) + file + ".xml";
	std::string fileToXSD = GetString("Config.json", { "DataManager",filename ,"Path", "AdressToXSD" }) + file + ".xsd";

	Json obj;
	obj.Export(GetJSON(filename), privilege, fileToXML, fileToXSD);
}

void DataManager::Import(std::string_view filename, std::string_view privilege)
{
	std::string file = GetString("Config.json", { "DataManager",filename ,"ViewPrivileges" ,privilege ,"File" });
	std::string fileToXML = GetString("Config.json", { "DataManager",filename ,"Path", "AdressToXML" }) + file + ".xml";
	Document& doc = const_cast<Document&>(Find_JSON_By_Name(filename));

	Json obj;
	obj.Import(doc, privilege, fileToXML);
}

void DataManager::Store(std::string_view filename) const
{
	const Document& doc = Find_JSON_By_Name(filename);
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	doc.Accept(writer);

	FileManager::SendToMicroSD(filename, buffer.GetString());
}