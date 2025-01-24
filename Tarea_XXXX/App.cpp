#include "App.hpp"
#include "Autogenerated_JSONData.hpp"

void APP::Init()
{
	// helper Lambda
	auto find_value_by_key = [](std::string_view key) -> std::string_view {
		for (const auto& item : json_data) {
			if (std::get<0>(item) == key) {
				return std::get<1>(item);
			}
		}
		LOG("[ERROR] Error AutoGenerated JSON %s , not found\n", key);
		return {};
		};

	const auto str = find_value_by_key("Config.json");
	const Document doc = DataManager::Create_JSON_From_String(str);

	if (!doc.HasMember("FileManager") || !doc["FileManager"].IsObject()) {
		LOG("[ERROR] FileManager not defined in Config.json\n");
		return;
	}
	FILEMANAGER.Init(doc["FileManager"]);


	if (!doc.HasMember("DataManager") || !doc["DataManager"].IsObject()) {
		LOG("[ERROR] DataManager not defined in Config.json\n");
		return;
	}
	DATAMANAGER.Init(doc["DataManager"]);
}

void APP::Run()
{
	DATAMANAGER.Export("LogicCircuit.json", "*");
	DATAMANAGER.Export("LogicCircuit.json", "C");
	DATAMANAGER.Export("LogicCircuit.json", "I");
	DATAMANAGER.Export("LogicCircuit.json", "U");

	DATAMANAGER.Export("Center.json", "*");
	DATAMANAGER.Export("Center.json", "O");
	DATAMANAGER.Export("Center.json", "C");
	DATAMANAGER.Export("Center.json", "A");
	DATAMANAGER.Export("Center.json", "I");
	DATAMANAGER.Export("Center.json", "D");
	DATAMANAGER.Export("Center.json", "X");
	DATAMANAGER.Export("Center.json", "L");
	DATAMANAGER.Export("Center.json", "P");
	DATAMANAGER.Export("Center.json", "T");
	DATAMANAGER.Export("Center.json", "R");
	DATAMANAGER.Export("Center.json", "U");

	//while (1)
	//{
	//	//...
	//}
}