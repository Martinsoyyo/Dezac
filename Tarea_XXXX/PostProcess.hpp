#pragma once
#include "Libraries.hpp"
#include "Log.hpp"

class ViewPermission;

class PostProcess {
public:
	void Do_Post_Process(std::string_view filename);


public:
	// Center.json
	bool Change_Language_For_Export_XML();
	bool Update_Read_Only_Variables_With_Is_Values();
	bool Show_Or_Hide_Logical_Units_By_Max_Number();
	// LogicCircuit.json
	bool Check_User_Curve_Names_And_Change_Data_Type();
	bool Check_If_User_Curve_As_Good_Values_In_Is_Settings();
	bool Show_Or_Hide_Protections_Unit_And_Automatism();
	bool Check_Conditions_For_U49P();
	bool Check_Conditions_For_U49T();


private:
	using ViewModifier = std::function<void(Document&, Value&, const ViewPermission&)>;

	bool Set_String(std::string_view filename, std::initializer_list<std::string_view> location, std::string_view new_value);
	bool Modify_View_In_SubTree(Document& doc, Value& source, const ViewPermission& new_permission, const ViewModifier& modifier);
	void Set_View(Document& doc, Value& value, const ViewPermission& new_permission);
	void Clean_View(Document& doc, Value& value, const ViewPermission& new_permission);
};

