#include "PostProcess.hpp"

bool PostProcess::Set_String(std::string_view filename, std::initializer_list<std::string_view> location, std::string_view new_value)
{
	Value& subJSON = const_cast<Value&>(DATAMANAGER.Get_JSON(filename, location));
	if (subJSON.IsString()) {
		std::string old_value = subJSON.GetString();
		if (old_value != new_value) {
			Document& doc = const_cast<Document&>(DATAMANAGER[filename]);
			subJSON.SetString(new_value.data(), doc.GetAllocator());
		}
		return true;
	}
	return false;
}

bool PostProcess::Modify_View_In_SubTree(Document& doc, Value& source, const ViewPermission& new_permission, const ViewModifier& modifier)
{
	if (source.IsObject()) {
		if (Json::Determine_Variable_Type(source) != VariableType::NotAVariable)
			modifier(doc, source, new_permission);

		for (auto& [name, value] : source.GetObject())
		{
			auto aux = name.GetString();
			if (Json::Determine_Variable_Type(value) != VariableType::NotAVariable) {
				modifier(doc, value, new_permission);
			}
			else if (!value.IsArray()) {
				Modify_View_In_SubTree(doc, value, new_permission, modifier);
			}
		}
		return true;
	}
	return false;
}

void PostProcess::Set_View(Document& doc, Value& value, const ViewPermission& new_permission) {
	if (value.HasMember("View")) {
		ViewPermission permission(value["View"].GetString());
		permission.Add(new_permission.Get());
		value["View"].SetString(permission.Get(), doc.GetAllocator());
	}
	else {
		Value key("View", doc.GetAllocator());
		Value val(new_permission.Get(), doc.GetAllocator());
		value.AddMember(key.Move(), val.Move(), doc.GetAllocator());
	}
};

void PostProcess::Clean_View(Document& doc, Value& value, const ViewPermission& new_permission) {
	if (value.HasMember("View")) {
		ViewPermission permission(value["View"].GetString());
		permission.Remove(new_permission.Get());

		if (permission.Get() == "") {
			value.RemoveMember("View");
		}
		else {
			value["View"].SetString(permission.Get(), doc.GetAllocator());
		}
	}
};

bool PostProcess::Change_Language_For_Export_XML()
{
	// Read Language setting and change state of translation units. to use when export XML/XSD.
	const std::string& lang = DATAMANAGER.Get_String("Center.json", { "CONFIGURATION", "GENERAL" ,"GENERAL_SETTINGS","Language","Type" });
	if (lang == "Language::SPA") T.setLanguage(LanguageType::Spanish);
	else if (lang == "Language::ENG") T.setLanguage(LanguageType::English);
	else if (lang == "Language::FRA") T.setLanguage(LanguageType::French);
	else {
		LOG("[ERROR] Lsnguage node does have a valid option.\n");
		return false;
	}

	return true;
}

void PostProcess::Update_Read_Only_Variables_With_Is_Values()
{
	// Helper Lambda.
	auto update_RO_Value = [this](
		std::string_view from, std::initializer_list<std::string_view> typeLocation,
		std::string_view to, std::initializer_list<std::string_view> valueLocation)
		{
			auto str = DATAMANAGER.Get_String(from, typeLocation);
			auto [type, subtype] = Json::Get_Type_And_Subtype(str);
			Set_String(to, valueLocation, (subtype == "") ? type : subtype);
		};


	update_RO_Value(
		"Center.json", { "CONFIGURATION", "GENERAL", "GENERAL_SETTINGS", "Hardware_Type", "Type" },
		"Center.json", { "CONFIGURATION", "INFORMATION", "Hardware_Type.", "Value" });

	update_RO_Value(
		"Center.json", { "CONFIGURATION", "GENERAL", "GENERAL_SETTINGS", "Hardware_Type", "Type" },
		"Center.json", { "CONFIGURATION", "GENERAL", "GENERAL_SETTINGS", "Hardware_Type.", "Value" });

	update_RO_Value(
		"Center.json", { "CONFIGURATION", "GENERAL", "GENERAL_SETTINGS", "Hardware_Mode", "Type" },
		"Center.json", { "CONFIGURATION", "GENERAL", "GENERAL_SETTINGS", "Hardware_Mode.", "Value" });

	update_RO_Value(
		"Center.json", { "CONFIGURATION", "GENERAL", "GENERAL_SETTINGS", "Grid_Type", "Type" },
		"Center.json", { "CONFIGURATION", "GENERAL", "GENERAL_SETTINGS", "Grid_Type.", "Value" });

	update_RO_Value(
		"Center.json", { "CONFIGURATION", "POSITIONS", "P0", "Centre_Type", "Type" },
		"LogicCircuit.json", { "CONFIGURATION", "INFORMATION", "Centre_Type.", "Value" });

	update_RO_Value(
		"Center.json", { "CONFIGURATION", "POSITIONS", "P0", "Centre_Type", "Type" },
		"Center.json", { "CONFIGURATION", "INFORMATION", "Centre_Type.", "Value" });

	update_RO_Value(
		"Center.json", { "CONFIGURATION", "POSITIONS", "P0", "Centre_Type", "Type" },
		"Center.json", { "CONFIGURATION", "POSITIONS", "P0", "Centre_Type.", "Value" });

	update_RO_Value(
		"Center.json", { "CONFIGURATION", "POSITIONS", "P0", "Logical_Device", "Type" },
		"Center.json", { "CONFIGURATION", "POSITIONS", "P0", "Logical_Device.", "Value" });


	auto max_eq_logic_str = DATAMANAGER.Get_String("Center.json", { "CONFIGURATION", "INFORMATION", "Logical_Equipment" , "Max" });
	auto max_eq_logic = std::stoul(max_eq_logic_str);
	for (size_t i = 1; i < max_eq_logic; i++) {
		std::string eq = "P" + std::to_string(i);

		update_RO_Value(
			"Center.json", { "CONFIGURATION", "POSITIONS", eq, "Type", "Type" },
			"Center.json", { "CONFIGURATION", "POSITIONS", eq, "Type.", "Value" });

		update_RO_Value(
			"Center.json", { "CONFIGURATION", "POSITIONS", eq, "Logical_Device", "Type" },
			"Center.json", { "CONFIGURATION", "POSITIONS", eq, "Logical_Device.", "Value" });
	}
}

bool PostProcess::Show_Or_Hide_Logical_Units_By_Max_Number()
{
	auto max_eq_logic_str = DATAMANAGER.Get_String("Center.json", { "CONFIGURATION", "INFORMATION", "Logical_Equipment" , "Max" });
	auto num_eq_logic_str = DATAMANAGER.Get_String("Center.json", { "CONFIGURATION", "INFORMATION", "Logical_Equipment" , "Value" });
	auto max_eq_logic = std::stoul(max_eq_logic_str);
	auto num_eq_logic = std::stoul(num_eq_logic_str);

	using namespace std::placeholders;
	Document& doc = const_cast<Document&>(DATAMANAGER["Center.json"]);
	// These logical devices will be shown.
	for (size_t i = 0; i < num_eq_logic + 1; i++) {
		std::string eq = "P" + std::to_string(i);
		Value& subJSON = const_cast<Value&>(DATAMANAGER.Get_JSON("Center.json", { "CONFIGURATION", "POSITIONS", eq }));

		// UnHide all elements in sub-tree
		Modify_View_In_SubTree(doc, subJSON, ViewPermission("H"), std::bind(&PostProcess::Clean_View, this, _1, _2, _3));
	}

	// These logical devices will be hide.
	for (size_t i = num_eq_logic + 1; i < max_eq_logic + 1; i++) {
		std::string eq = "P" + std::to_string(i);
		Value& subJSON = const_cast<Value&>(DATAMANAGER.Get_JSON("Center.json", { "CONFIGURATION", "POSITIONS", eq }));

		// Hide all elements in sub-tree
		Modify_View_In_SubTree(doc, subJSON, ViewPermission("H"), std::bind(&PostProcess::Set_View, this, _1, _2, _3));
	}
	return true;
}

bool PostProcess::Check_User_Curve_Names_And_Change_Data_Type()
{
	// We'll begin by replacing the names of the user-defined curves into <Curve_Backup> datatype.
	auto str_num_user_curve = DATAMANAGER.Get_String("Center.json", { "CONFIGURATION", "INFORMATION", "Max_User_Curves." , "Value" });
	auto num_user_curve = std::stoul(str_num_user_curve);
	for (size_t i = 1; i < num_user_curve + 1; i++) 
	{
		std::string curve_id = "USER_CURVE_" + std::to_string(i);
		auto curve_name = DATAMANAGER.Get_String("LogicCircuit.json", { "CONFIGURATION", "SETTINGS", "USER_CURVE", "INTERPOLATION", curve_id, "Name", "Value" });
		Set_String("Datatypes.json", { "Curve_Backup", curve_id, "Name" }, curve_name);
	}


	// In the final step, we will rebuild the curve data type with the updated names.
	Document& doc = const_cast<Document&>(DATAMANAGER["Datatypes.json"]);
	const Value& Datatypes = DATAMANAGER.Get_JSON("Datatypes.json");

	Value& old = const_cast<Value&>(Datatypes["Curve"]);
	old.RemoveAllMembers();

	std::set<std::string> unique_names;
	for (auto& [name, value] : Datatypes["Curve_Backup"].GetObject()) {

		if (value.ObjectEmpty()) {
			Value key(name.GetString(), doc.GetAllocator());
			old.AddMember(key, Value(kObjectType), doc.GetAllocator());
		}
		else {
			unique_names.insert(value["Name"].GetString());
			Value key(value["Name"].GetString(), doc.GetAllocator());
			old.AddMember(key, Value(kObjectType), doc.GetAllocator());
		}
	}

	if (num_user_curve != unique_names.size()) {
		LOG("[ERROR] The user curves contain duplicate names.\n");
		return false;
	}

	// TODO: tendria que chequear TODOS los lugares donde se usa alguna curva de usuario   
	//       los nombres acabo de poner podrian sobreescribir una que esta siendo usado.
	return true;
}

void PostProcess::Show_Or_Hide_Protections_Unit_And_Automatism()
{
	// Helper Lambda.
	auto PresentMissing = [this](
		std::string_view filename,
		std::initializer_list<std::string_view> isPresentLocation,
		std::initializer_list<std::string_view> nodeToSet)
		{
			using namespace std::placeholders;
			Document& doc = const_cast<Document&>(DATAMANAGER["LogicCircuit.json"]);
			Value& destJSON = const_cast<Value&>(DATAMANAGER.Get_JSON(filename, nodeToSet));
			std::string type = DATAMANAGER.Get_String(filename, isPresentLocation);

			if (type == "Presence::PRESENT") {
				// UnHide all elements in sub-tree
				Modify_View_In_SubTree(doc, destJSON, ViewPermission("H"), std::bind(&PostProcess::Clean_View, this, _1, _2, _3));
			}
			else if (type == "Presence::MISSING") {
				// Hide all elements in sub-tree
				Modify_View_In_SubTree(doc, destJSON, ViewPermission("H"), std::bind(&PostProcess::Set_View, this, _1, _2, _3));
			}
			else
			{
				LOG("[ERROR] subType Present or Missing was wrong.\n");
				return;
			}
		};


	//OVERCURRENT
	PresentMissing(
		"LogicCircuit.json",
		{ "CONFIGURATION", "MODULES", "PROTECTIONS", "UNIT_51", "Type" },
		{ "CONFIGURATION", "SETTINGS", "PROTECTION_GROUP_1", "CURRENT_PROTECTION", "OVERCURRENT", "UNIT_51_G1" });
	PresentMissing(
		"LogicCircuit.json",
		{ "CONFIGURATION", "MODULES", "PROTECTIONS", "UNIT_51_2", "Type" },
		{ "CONFIGURATION", "SETTINGS", "PROTECTION_GROUP_1", "CURRENT_PROTECTION", "OVERCURRENT", "UNIT_51_2_G1" });
	PresentMissing(
		"LogicCircuit.json",
		{ "CONFIGURATION", "MODULES", "PROTECTIONS", "UNIT_50", "Type" },
		{ "CONFIGURATION", "SETTINGS", "PROTECTION_GROUP_1", "CURRENT_PROTECTION", "OVERCURRENT", "UNIT_50_G1" });
	PresentMissing(
		"LogicCircuit.json",
		{ "CONFIGURATION", "MODULES", "PROTECTIONS", "UNIT_51N", "Type" },
		{ "CONFIGURATION", "SETTINGS", "PROTECTION_GROUP_1", "CURRENT_PROTECTION", "OVERCURRENT", "UNIT_51N_G1" });
	PresentMissing(
		"LogicCircuit.json",
		{ "CONFIGURATION", "MODULES", "PROTECTIONS", "UNIT_51_2_N", "Type" },
		{ "CONFIGURATION", "SETTINGS", "PROTECTION_GROUP_1", "CURRENT_PROTECTION", "OVERCURRENT", "UNIT_51_2_N_G1" });
	PresentMissing(
		"LogicCircuit.json",
		{ "CONFIGURATION", "MODULES", "PROTECTIONS", "UNIT_50N", "Type" },
		{ "CONFIGURATION", "SETTINGS", "PROTECTION_GROUP_1", "CURRENT_PROTECTION", "OVERCURRENT", "UNIT_50N_G1" });
	PresentMissing(
		"LogicCircuit.json",
		{ "CONFIGURATION", "MODULES", "PROTECTIONS", "UNIT_51NS", "Type" },
		{ "CONFIGURATION", "SETTINGS", "PROTECTION_GROUP_1", "CURRENT_PROTECTION", "OVERCURRENT", "UNIT_51NS_G1" });
	PresentMissing(
		"LogicCircuit.json",
		{ "CONFIGURATION", "MODULES", "PROTECTIONS", "UNIT_51_2_NS", "Type" },
		{ "CONFIGURATION", "SETTINGS", "PROTECTION_GROUP_1", "CURRENT_PROTECTION", "OVERCURRENT", "UNIT_51_2_NS_G1" });
	PresentMissing(
		"LogicCircuit.json",
		{ "CONFIGURATION", "MODULES", "PROTECTIONS", "UNIT_50NS", "Type" },
		{ "CONFIGURATION", "SETTINGS", "PROTECTION_GROUP_1", "CURRENT_PROTECTION", "OVERCURRENT", "UNIT_50NS_G1" });
	PresentMissing(
		"LogicCircuit.json",
		{ "CONFIGURATION", "MODULES", "PROTECTIONS", "UNIT_51G", "Type" },
		{ "CONFIGURATION", "SETTINGS", "PROTECTION_GROUP_1", "CURRENT_PROTECTION", "OVERCURRENT", "UNIT_51G_G1" });


	//DIRECTIONAL
	PresentMissing(
		"LogicCircuit.json",
		{ "CONFIGURATION", "MODULES", "PROTECTIONS", "UNIT_67", "Type" },
		{ "CONFIGURATION", "SETTINGS", "PROTECTION_GROUP_1", "CURRENT_PROTECTION", "DIRECTIONAL", "UNIT_67_G1" });
	PresentMissing(
		"LogicCircuit.json",
		{ "CONFIGURATION", "MODULES", "PROTECTIONS", "UNIT_67N", "Type" },
		{ "CONFIGURATION", "SETTINGS", "PROTECTION_GROUP_1", "CURRENT_PROTECTION", "DIRECTIONAL", "UNIT_67N_G1" });
	PresentMissing(
		"LogicCircuit.json",
		{ "CONFIGURATION", "MODULES", "PROTECTIONS", "UNIT_67NS", "Type" },
		{ "CONFIGURATION", "SETTINGS", "PROTECTION_GROUP_1", "CURRENT_PROTECTION", "DIRECTIONAL", "UNIT_67NS_G1" });
	PresentMissing(
		"LogicCircuit.json",
		{ "CONFIGURATION", "MODULES", "PROTECTIONS", "UNIT_67NA", "Type" },
		{ "CONFIGURATION", "SETTINGS", "PROTECTION_GROUP_1", "CURRENT_PROTECTION", "DIRECTIONAL", "UNIT_67NA_G1" });

	//NEGATIVE_SEQUENCE
	PresentMissing(
		"LogicCircuit.json",
		{ "CONFIGURATION", "MODULES", "PROTECTIONS", "UNIT_46BC", "Type" },
		{ "CONFIGURATION", "SETTINGS", "PROTECTION_GROUP_1", "CURRENT_PROTECTION", "NEGATIVE_SEQUENCE", "BROKEN_CONDUCTOR", "UNIT_46BC_G1" });

	//WATTIMETRIC
	PresentMissing(
		"LogicCircuit.json",
		{ "CONFIGURATION", "MODULES", "PROTECTIONS", "UNIT_PWH", "Type" },
		{ "CONFIGURATION", "SETTINGS", "PROTECTION_GROUP_1", "CURRENT_PROTECTION", "WATTIMETRIC", "UNIT_PWH_G1" });

	//BLOCKING
	PresentMissing(
		"LogicCircuit.json",
		{ "CONFIGURATION", "MODULES", "PROTECTIONS", "SECOND_HARMONIC", "Type" },
		{ "CONFIGURATION", "SETTINGS", "PROTECTION_GROUP_1", "CURRENT_PROTECTION", "BLOCKING", "SECOND_HARMONIC_G1" });
	PresentMissing(
		"LogicCircuit.json",
		{ "CONFIGURATION", "MODULES", "PROTECTIONS", "LOGIC_BLOCK_UNIT", "Type" },
		{ "CONFIGURATION", "SETTINGS", "PROTECTION_GROUP_1", "CURRENT_PROTECTION", "BLOCKING", "LOGIC_BLOCKING_G1" });
	PresentMissing(
		"LogicCircuit.json",
		{ "CONFIGURATION", "MODULES", "PROTECTIONS", "IMAX_BLOCK_UNIT", "Type" },
		{ "CONFIGURATION", "SETTINGS", "PROTECTION_GROUP_1", "CURRENT_PROTECTION", "BLOCKING", "IMAX_BLOCKING_UNIT" });

	//// TEMP_PROTECTION
	PresentMissing(
		"LogicCircuit.json",
		{ "CONFIGURATION", "MODULES", "PROTECTIONS", "UNIT_49", "Type" },
		{ "CONFIGURATION", "SETTINGS", "PROTECTION_GROUP_1", "TEMPERATURE_PROTECTION", "THERMAL_OVERLOAD", "UNIT_49_G1" });
	PresentMissing(
		"LogicCircuit.json",
		{ "CONFIGURATION", "MODULES", "PROTECTIONS", "UNIT_49P", "Type" },
		{ "CONFIGURATION", "SETTINGS", "PROTECTION_GROUP_1", "TEMPERATURE_PROTECTION", "THERMAL_OVERLOAD", "UNIT_49P_G1" });
	PresentMissing(
		"LogicCircuit.json",
		{ "CONFIGURATION", "MODULES", "PROTECTIONS", "UNIT_49T", "Type" },
		{ "CONFIGURATION", "SETTINGS", "PROTECTION_GROUP_1", "TEMPERATURE_PROTECTION", "THERMAL_OVERLOAD", "UNIT_49T_G1" });

	// UNDER_VOLTAGE
	PresentMissing(
		"LogicCircuit.json",
		{ "CONFIGURATION", "MODULES", "PROTECTIONS", "UNIT_27_TEMP_G1", "Type" },
		{ "CONFIGURATION", "SETTINGS", "PROTECTION_GROUP_1", "VOLTAGE_PROTECTION", "UNDERVOLTAGE", "UNIT_27_TEMP_G1" });
	PresentMissing(
		"LogicCircuit.json",
		{ "CONFIGURATION", "MODULES", "PROTECTIONS", "UNIT_27_INST_G1", "Type" },
		{ "CONFIGURATION", "SETTINGS", "PROTECTION_GROUP_1", "VOLTAGE_PROTECTION", "UNDERVOLTAGE", "UNIT_27_INST_G1" });

	// OVER_VOLTAGE
	PresentMissing(
		"LogicCircuit.json",
		{ "CONFIGURATION", "MODULES", "PROTECTIONS", "UNIT_59T", "Type" },
		{ "CONFIGURATION", "SETTINGS", "PROTECTION_GROUP_1", "VOLTAGE_PROTECTION", "OVERVOLTAGE", "UNIT_59_TEMP_G1" });
	PresentMissing(
		"LogicCircuit.json",
		{ "CONFIGURATION", "MODULES", "PROTECTIONS", "UNIT_59I", "Type" },
		{ "CONFIGURATION", "SETTINGS", "PROTECTION_GROUP_1", "VOLTAGE_PROTECTION", "OVERVOLTAGE", "UNIT_59_INST_G1" });
	PresentMissing(
		"LogicCircuit.json",
		{ "CONFIGURATION", "MODULES", "PROTECTIONS", "UNIT_59NT", "Type" },
		{ "CONFIGURATION", "SETTINGS", "PROTECTION_GROUP_1", "VOLTAGE_PROTECTION", "OVERVOLTAGE", "UNIT_59N_TEMP_G1" });
	PresentMissing(
		"LogicCircuit.json",
		{ "CONFIGURATION", "MODULES", "PROTECTIONS", "UNIT_59NT", "Type" },
		{ "CONFIGURATION", "SETTINGS", "PROTECTION_GROUP_1", "VOLTAGE_PROTECTION", "OVERVOLTAGE", "UNIT_59N_INST_G1" });
}


// Verifies that the values of the curve specified by the user are
// consistent. I0 <= I1 <= I2 <= I3 <= I4 <= I5
bool PostProcess::Check_If_User_Curve_As_Good_Values_In_Is_Settings()
{
	// Helper Lambda.
	auto CheckValues = [](
		std::string_view filename,
		std::initializer_list<std::string_view> Location)
		{
			const Value& subJSON = DATAMANAGER.Get_JSON(filename, Location);

			float i0 = std::strtof(subJSON["I0"]["Value"].GetString(), nullptr);
			float i1 = std::strtof(subJSON["I1"]["Value"].GetString(), nullptr);
			float i2 = std::strtof(subJSON["I2"]["Value"].GetString(), nullptr);
			float i3 = std::strtof(subJSON["I3"]["Value"].GetString(), nullptr);
			float i4 = std::strtof(subJSON["I4"]["Value"].GetString(), nullptr);
			float i5 = std::strtof(subJSON["I5"]["Value"].GetString(), nullptr);

			return (i0 < i1 && i1 < i2 && i2 < i3 && i3 < i4 && i4 < i5);
		};


	auto num_user_curve = std::stoul(DATAMANAGER.Get_String("Center.json", { "CONFIGURATION", "INFORMATION", "Max_User_Curves.", "Value" }));
	for (size_t i = 0; i < num_user_curve; i++)
	{
		std::string curve_id = "USER_CURVE_" + std::to_string(i + 1);
		if (CheckValues("LogicCircuit.json", { "CONFIGURATION", "SETTINGS", "USER_CURVE", "INTERPOLATION", curve_id }) == false) {
			return false;
		}

	}
	return true;
}

// I need to check that RestoreTemp < Alarm < Trip for all U49P units.
bool PostProcess::Check_Conditions_For_U49P()
{
	// Helper Lambda.
	auto CheckLimits = [](
		std::string_view filename,
		std::initializer_list<std::string_view> Location)
		{
			const Value& subJSON = DATAMANAGER.Get_JSON(filename, Location);

			auto restore_threshold = std::strtof(subJSON["Restore_Threshold"]["Value"].GetString(), nullptr);
			auto alarm_threshold = std::strtof(subJSON["Alarm_Threshold"]["Value"].GetString(), nullptr);
			auto trip_threshold = std::strtof(subJSON["Trip_Threshold"]["Value"].GetString(), nullptr);

			return (restore_threshold < alarm_threshold && alarm_threshold < trip_threshold);
		};


	return (
		CheckLimits("LogicCircuit.json", { "CONFIGURATION", "SETTINGS", "PROTECTION_GROUP_1", "TEMPERATURE_PROTECTION", "THERMAL_OVERLOAD", "UNIT_49_G1" }) &&
		CheckLimits("LogicCircuit.json", { "CONFIGURATION", "SETTINGS", "PROTECTION_GROUP_1", "TEMPERATURE_PROTECTION", "THERMAL_OVERLOAD", "UNIT_49P_G1" })
		//	CheckLimits("LogicCircuit.json", { "CONFIGURATION", "INFORMATION", "PROTECTION_GROUP_2", "TEMPERATURE_PROTECTION", "THERMAL_OVERLOAD", "UNIT_49_G2" }) &&
		//	CheckLimits("LogicCircuit.json", { "CONFIGURATION", "INFORMATION", "PROTECTION_GROUP_2", "TEMPERATURE_PROTECTION", "THERMAL_OVERLOAD", "UNIT_49P_G2" })
		);
}

// I need to check that RestoreTemp < Alarm < Trip for all U49T units.
bool PostProcess::Check_Conditions_For_U49T()
{
	// Helper Lambda.
	auto CheckLimits = [](
		std::string_view filename,
		std::initializer_list<std::string_view> Location)
		{
			const Value& subJSON = DATAMANAGER.Get_JSON(filename, Location);

			auto restore_threshold = std::strtof(subJSON["Restore_Temperature"]["Value"].GetString(), nullptr);
			auto alarm_threshold = std::strtof(subJSON["Alarm1_Temperature"]["Value"].GetString(), nullptr);
			auto trip_threshold = std::strtof(subJSON["Alarm2_Or_Trip_Temperature"]["Value"].GetString(), nullptr);

			return (restore_threshold < alarm_threshold && alarm_threshold < trip_threshold);
		};


	bool res = true;
	for (size_t i = 0; i < 3; i++)
	{
		std::string id = "ELECTRONIC_RTD_" + std::to_string(i + 1);
		res = res && CheckLimits("LogicCircuit.json", { "CONFIGURATION", "SETTINGS", "PROTECTION_GROUP_1", "TEMPERATURE_PROTECTION", "THERMAL_OVERLOAD", "UNIT_49T_G1", id });
		//res = res && CheckLimits("LogicCircuit.json", { "CONFIGURATION", "SETTINGS", "PROTECTION_GROUP_2", "TEMPERATURE_PROTECTION", "THERMAL_OVERLOAD", "UNIT_49T_G2", id });
	}
	return res;
}