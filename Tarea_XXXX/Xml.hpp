#pragma once
#include "Libraries.hpp"
#include "Log.hpp"

class  XML {
public:
	XML(const Value& json);

	const std::string Get_XSD();
	const std::string Get_XML();

private:
	void Add_Xml_Node(xml_node<>* node, std::string_view key, std::string_view value);
	void Add_Xsd_Node(xml_node<>* node, std::string_view key, std::string_view type, std::string_view displayName);
	auto Add_Xsd_Element(xml_node<>* node, std::string_view key);
	void Add_Xsd_Number(std::string_view type, std::string_view min, std::string_view max, std::string_view unit);
	void Add_Xsd_Enums(std::string_view key, std::vector<std::string_view>&& values, std::string_view unit);
	void Add_Xsd_Basic_Type(std::string_view type, std::string_view extension, std::string_view attribute = "");
	void Add_Xsd_Type_Enum(std::string_view type, std::string_view unit);
	void Json_To_Xml(const Value& json, xml_node<>* node_xml, xml_node<>* node_xsd, const std::string& parent = "");

	//----------------------------------------------------------------------------
	// STATICS MEMBERS 
	//----------------------------------------------------------------------------

	// Show XML tree in Pretty format. (to easy debug)
	static void Show(xml_node<>* node, int depth = 0);
	// {"x.xxx" , "y.y"} --> "x.x" | {"x.x" , "y.yyy"} --> "x.x00" | {"x.xxx" , "y"} --> "x" 
	static std::string Adjust_Decimals(std::string_view to, std::string_view from);
	// {any words of any length} --> {Hexa number of 32bits}
	static std::string Generate_Short_ID(const std::string& input);
	// "5.34" --> 2 | "12.9" --> 1 | "89" --> 0
	static size_t  CountDecimals(std::string_view str);

private:
	std::vector<std::string> xsdTypesIsPresent_ = {};
	xml_document<> xml_;
	xml_document<> xsd_;
	xml_document<> xsdTypes_;
};
