#pragma once
#include "Libraries.hpp"

#include "XML.hpp"
#include "Json.hpp"
#include "Log.hpp"
#include "FileManager.hpp"


class PostProcess {
public:
	PostProcess()
	{
		LOG("Create PostProcess::PostProcess()\n");
	}

	void DoPostProcess(std::string_view filename)
	{
		if (filename == "Center.json") {
			Change_Language_For_Export_XML();
			Update_Read_Only_Variables_With_Is_Values();
			Show_Or_Hide_Logical_Units_By_Max_Number();
			return;
		}
		if (filename == "LogicCircuit.json") {
			Check_User_Curve_Names_And_Change_Data_Type();
			Show_Or_Hide_Protections_Unit_And_Automatism();
			Check_If_User_Curve_As_Good_Values_In_Is_Settings();
			Check_Conditions_For_U49P();
			Check_Conditions_For_U49T();
			return;
		}
	}


private:
	using ViewModifier = std::function<void(Document&, Value&, const ViewPermission&)>;

	bool SetString(std::string_view filename, std::initializer_list<std::string_view> location, std::string_view new_value);

	bool Modify_View_In_SubTree(Document& doc, Value& source, const ViewPermission& new_permission, const ViewModifier& modifier);

	void Set_View(Document& doc, Value& value, const ViewPermission& new_permission);

	void Clean_View(Document& doc, Value& value, const ViewPermission& new_permission);


	// Center.json
	bool Change_Language_For_Export_XML();
	void Update_Read_Only_Variables_With_Is_Values();
	bool Show_Or_Hide_Logical_Units_By_Max_Number();
	// LogicCircuit.json
	bool Check_User_Curve_Names_And_Change_Data_Type();
	bool Check_If_User_Curve_As_Good_Values_In_Is_Settings();
	void Show_Or_Hide_Protections_Unit_And_Automatism();
	bool Check_Conditions_For_U49P();
	bool Check_Conditions_For_U49T();
};

/////////////////////////////////////////////////
inline PostProcess POSTPROCESS; /// OBJETO GLOBAL
/////////////////////////////////////////////////

