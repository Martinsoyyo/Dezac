#pragma once
#include "Libraries.hpp"

#include "JSON.hpp"
#include "Language.hpp"
#include "DataManager.hpp"
#include "FileManager.hpp"

// This class is instantiated only to save to XML file, it will be consumed, after constructor.
class  XML {
public:
	XML(const Value&& json, std::string_view XMLfile, std::string_view XSDfile);


private:
	void addXmlNode(xml_node<>* node, std::string_view key, std::string_view value);
	void addXsdNode(xml_node<>* node, std::string_view key, std::string_view type, std::string_view displayName);
	auto addXsdElement(xml_node<>* node, std::string_view key);
	void addXsdNumber(std::string_view type, std::string_view min, std::string_view max, std::string_view unit);
	void addXsdEnums(std::string_view key, std::vector<std::string_view>&& values, std::string_view unit);
	void addXsdBasicType(std::string_view type, std::string_view extension, std::string_view attribute = "");
	void addXsdTypeEnum(std::string_view type, std::string_view unit);
	void jsonToXml(const Value& json, xml_node<>* node_xml, xml_node<>* node_xsd, const std::string& parent = "");
	void save(std::string_view XMLfile, std::string_view XSDfile);

	//----------------------------------------------------------------------------
	// STATICS MEMBERS 
	//----------------------------------------------------------------------------

	// Show XML tree in Pretty format. (to easy debug)
	static void Show(xml_node<>* node, int depth = 0);
	// {"x.xxx" , "y.y"} --> "x.x" | {"x.x" , "y.yyy"} --> "x.x00" | {"x.xxx" , "y"} --> "x" 
	static std::string AdjustDecimals(std::string_view to, std::string_view from);
	// {any words of any length} --> {Hexa number of 32bits}
	static std::string GenerateShortID(const std::string& input);

private:
	std::vector<std::string> xsdTypesIsPresent = {};
	xml_document<> xml_;
	xml_document<> xsd_;
	xml_document<> xsdTypes_;
};
