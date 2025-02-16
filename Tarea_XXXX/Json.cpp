#include "Json.hpp"

#include "XML.hpp"
#include "FileManager.hpp"
#include "App.hpp"

Json::Json(Document& jsonOriginal, std::string_view viewer) :
	baseJson_(jsonOriginal),
	viewPermissionLevel_(viewer)
{
	processedJson_.SetObject();
}

// Checks if the values that i want to import has a correct type/subtype, formats, limits etc...
bool Json::Validate_Type_And_Bounds(const Value& original, std::string_view name, std::string_view value)
{
	if (!original.HasMember(name.data())) return false;

	const auto& _data = original[name.data()];
	switch (Get_Variable_Type(_data))
	{
	case VariableType::Enum: // {"Type": <type>::<subtype>} is a valid one?
	{
		auto [type, subtype] = Extract_Type_And_Subtype(_data["Type"].GetString());
		return Json::Datatype()[type].HasMember(value.data());
	}
	case VariableType::Number: // {"Value": <value>} is in range?
	{
		float num = std::stof(value.data());
		float min = std::stof(_data["Min"].GetString());
		float max = std::stof(_data["Max"].GetString());
		return (num >= min && num <= max);
	}
	case VariableType::ReadOnly:	 return true;
	case VariableType::String:		 return true;
	case VariableType::Date:		 return true; // TODO: bool CheckIsValidDate(value);
	case VariableType::Time:		 return true; // TODO: bool CheckIsValidTime(value);
	case VariableType::NotAVariable: return false;
	default: return false;
	};
}

// Comprare JSON to check if is a valid structure, and source data make sense.
bool Json::Validate_Json_Structure(const Value& source, const Value& new_json)
{
	if (source.IsObject() && new_json.IsObject()) {
		for (const auto& [name, value] : source.GetObject())
		{
			if (!value.IsObject()) {
				if (!Validate_Type_And_Bounds(new_json, name.GetString(), value.GetString()))
				{
					return false;
				}
			}
			else if (!new_json.HasMember(name) || !Validate_Json_Structure(value, new_json[name]))
			{
				return false;
			}
		}
		return true;
	}
	return true;
}

// Checks XML file compatibility for shape and datatype, and create processedJson_ for later.
bool Json::Parse_Xml_To_Json(std::string_view XMLfile)
{
	auto str = APP::FILEMANAGER.LoadFromMicroSD(XMLfile);
	if (!str.has_value()) {
		LOG("[ERROR] Error loading XML file : %s\n", XMLfile.data());
		return false;
	}

	// Open File and parse XML.
	xml_document<> _xml_to_proccess;
	try { _xml_to_proccess.parse<0>(str.value().data()); }
	catch (...) {
		LOG("[ERROR] Error parsing XML file : %s\n", XMLfile.data());
		return false;
	}

	Convert_Xml_Node_To_Json(_xml_to_proccess.first_node(), processedJson_, processedJson_.GetAllocator());
	if (processedJson_.HasParseError()) {
		LOG("[ERROR] Json has parse errors.\n");
		return false;
	}

	return true;
}

// Combine JSON with the new values from Import.
void Json::Merge_Json(Value& dest, const Value& from, Document::AllocatorType& alloc) const
{
	if (dest.IsObject())
	{
		for (auto& [name, value] : dest.GetObject())
		{
			auto str = name.GetString();
			auto var = Get_Variable_Type(value);
			if (var != VariableType::NotAVariable && from.HasMember(str))
			{
				if (var == VariableType::Enum)
				{
					auto [type, subtype] = Extract_Type_And_Subtype(value["Type"].GetString());
					auto str_subtype = (subtype == "") ? "" : std::string("::") + from[str].GetString();
					value["Type"].SetString(type + str_subtype, alloc);
				}
				else {
					value["Value"].SetString(from[str].GetString(), alloc);
				}
			}
			if (from.IsObject() && from.HasMember(str))
			{
				Merge_Json(value, from[str], alloc);
			}
		}
	}
}

// Get sub-TREE from the original JSON according to the viewer_priority.
bool Json::Filter(
	bool ignoreHide,
	std::string_view viewer_priority,
	const Value& source,
	Value& destination,
	Document::AllocatorType& alloc,
	ViewPermission parent = ViewPermission(""))
{
	if (!source.IsObject()) return false;

	// Checks if the node has a "view", if not, we heredate from the parent.
	std::string str = source.HasMember("View") ? source["View"].GetString() : parent.Get();
	ViewPermission _p(str);

	Value current_result(kObjectType);
	if (Get_Variable_Type(source) != VariableType::NotAVariable) {
		bool ignoreHideTag = (ignoreHide == true) ? true : !_p.HasMember("H");
		if (viewer_priority == "*" || (_p.HasMember(viewer_priority) && ignoreHideTag))
		{
			current_result.CopyFrom(source, alloc);
			if (current_result.HasMember("View")) current_result.RemoveMember("View");
		}
	}

	for (auto& [name, value] : source.GetObject())
	{
		Value child_result(kObjectType);
		if (Filter(ignoreHide, viewer_priority, value, child_result, alloc, _p)) {
			Value keyValue(name.GetString(), alloc);
			current_result.AddMember(keyValue, child_result, alloc);
		}
	}

	if (!current_result.ObjectEmpty()) {
		destination.CopyFrom(current_result, alloc);
		return true;
	}
	return false;
}

void Json::Import(std::string_view XMLfile)
{
	if (Parse_Xml_To_Json(XMLfile) == false) {
		LOG("Process_XML as errors loading XML file : %s\n", XMLfile.data());
		return;
	}

	Document sub_tree;
	Filter(true, viewPermissionLevel_, baseJson_, sub_tree, sub_tree.GetAllocator());
	//Json::Show(sub_tree);

	if (Validate_Json_Structure(processedJson_, sub_tree) == false) {
		LOG("The XML dont have a valid structure to import, XML file : %s\n", XMLfile.data());
		return;
	}

	Merge_Json(baseJson_, processedJson_, baseJson_.GetAllocator());
}

void Json::Export(std::string_view AdressXMLfile, std::string_view AdressXSDfile) const
{
	Document sub_tree;
	Filter(false, viewPermissionLevel_, baseJson_, sub_tree, sub_tree.GetAllocator());

	XML xml(sub_tree);

	APP::FILEMANAGER.SaveToMicroSD(AdressXMLfile, xml.Get_XML());
	APP::FILEMANAGER.SaveToMicroSD(AdressXSDfile, xml.Get_XSD());
}

// "Type::SubType" --> {"Type", "SubType"}
std::pair<std::string, std::string> Json::Extract_Type_And_Subtype(std::string_view str)
{
	static constexpr std::string_view delimiter = "::";
	size_t pos = str.find(delimiter);

	if (pos == std::string_view::npos) {
		return { std::string(str), std::string("") };
	}
	return { std::string(str.substr(0, pos)), std::string(str.substr(pos + delimiter.length())) };
}

// Recursive Function to reconstruct from XML --> JSON
void Json::Convert_Xml_Node_To_Json(xml_node<>* XMLnode, Value& JSONnode, Document::AllocatorType& allocator)
{
	Value json_child;

	if (XMLnode->first_node() && XMLnode->first_node()->first_node()) {
		json_child.SetObject();
		for (xml_node<>* child = XMLnode->first_node(); child; child = child->next_sibling())
		{
			Convert_Xml_Node_To_Json(child, json_child, allocator);
		}
	}
	else {
		json_child.SetString(XMLnode->value(), allocator);
	}

	Value name;
	name.SetString(XMLnode->name(), allocator);
	JSONnode.AddMember(name, json_child, allocator);
}

// Show JSON tree in Pretty format. (to easy debug)
void Json::Show(const Value& json)
{
	StringBuffer buffer;
	PrettyWriter<StringBuffer> writer(buffer);
	writer.SetIndent(' ', 4);
	json.Accept(writer);
	std::cout << buffer.GetString() << std::endl;
}

// Lazy Initialization of Datatype
const Value& Json::Datatype(const Value& datatype) {
	static const Value& _datatype = datatype;
	return _datatype;
}

// "Type::SubType" is in datatype structure ?
bool Json::Is_Valid_Type_And_Subtype(std::string_view str)
{
	auto [type, subtype] = Json::Extract_Type_And_Subtype(str);

	return (subtype == "") ? 
		Json::Datatype().HasMember(type.data()) :
		Json::Datatype().HasMember(type.data()) && Json::Datatype()[type.data()].HasMember(subtype.data());
}

// Unknow Variable --> { Enum or ReadOnly or Number or ... }
VariableType Json::Get_Variable_Type(const Value& node)
{
	if (node.IsObject())
	{
		bool hasTyp = node.HasMember("Type") && node["Type"].IsString() && Is_Valid_Type_And_Subtype(node["Type"].GetString());
		bool hasVal = node.HasMember("Value") && node["Value"].IsString();
		bool hasMin = node.HasMember("Min") && node["Min"].IsString();
		bool hasMax = node.HasMember("Max") && node["Max"].IsString();

		if (hasTyp && hasVal && hasMin && hasMax) { return VariableType::Number; }
		if (hasTyp && hasVal)
		{
			auto [type, subtype] = Json::Extract_Type_And_Subtype(node["Type"].GetString());
			if (subtype == "") {
				if (type == "String") return VariableType::String;
				if (type == "Date")   return VariableType::Date;
				if (type == "Time")	  return VariableType::Time;
			}
		}
		if (hasVal) { return VariableType::ReadOnly; }
		if (hasTyp) { return VariableType::Enum; }
	}
	return VariableType::NotAVariable;
}