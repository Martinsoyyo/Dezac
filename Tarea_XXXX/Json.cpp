#include "Json.hpp"


// Checks if the values that i want to import has a correct type/subtype, formats, limits etc...
bool Json::checkTypeBounds(const Value& original, std::string_view name, std::string_view value) const
{
	if (!original.HasMember(name.data())) return false;

	const auto& _data = original[name.data()];
	switch (DetermineVariableType(_data))
	{
	case VariableType::Enum: // {"Type": <type>::<subtype>} is a valid one?
	{
		auto [type, subtype] = GetTypeAndSubtype(_data["Type"].GetString());
		return Json::GetDatatype()[type].HasMember(value.data());
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
bool Json::isValidJSON(const Value& source, const Value& original) const
{
	if (source.IsObject() && original.IsObject()) {
		for (const auto& [name, value] : source.GetObject())
		{
			if (!value.IsObject()) {
				if (!checkTypeBounds(original, name.GetString(), value.GetString()))
				{
					return false;
				}
			}
			else if (!original.HasMember(name) || !isValidJSON(value, original[name]))
			{
				return false;
			}
		}
		return true;
	}
	return true; // TODO: check condition!
}

// Checks XML file compatibility for shape and datatype, and create json_ for later. 
bool Json::ProcessXML(std::string_view fileFromXML)
{
	json_.SetObject();

	// Open File and parse XML.
	xml_document<> _xml_to_proccess;
	auto str = FileManager::Load(fileFromXML);
	if (!str.has_value()) {
		LOG("[ERROR] Error loading XML file : %s\n", fileFromXML.data());
		return false;
	}

	_xml_to_proccess.parse<0>(str.value().data());
	XmlToJson(_xml_to_proccess.first_node(), json_, json_.GetAllocator());

	if (json_.HasParseError()) {
		LOG("[ERROR] Json has parse errors.\n");
		return false;
	}

	return true;
}

// Combine JSON with the new values from Import.
void Json::Combine(Value& dest, const Value& from, Document::AllocatorType& alloc) const
{
	if (dest.IsObject())
	{
		for (auto& [name, value] : dest.GetObject())
		{
			auto str = name.GetString();
			auto var = DetermineVariableType(value);
			if (var != VariableType::NotAVariable && from.HasMember(str))
			{
				if (var == VariableType::Enum)
				{
					auto [type, subtype] = GetTypeAndSubtype(value["Type"].GetString());
					auto str_subtype = (subtype == "") ? "" : std::string("::") + from[str].GetString();
					value["Type"].SetString(type + str_subtype, alloc);
				}
				else {
					value["Value"].SetString(from[str].GetString(), alloc);
				}
			}
			if (from.IsObject() && from.HasMember(str))
			{
				Combine(value, from[str], alloc);
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
	if (DetermineVariableType(source) != VariableType::NotAVariable) {
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

void Json::Import(Document& jsonOriginal, std::string_view viewer_priority, std::string_view XMLfile)
{
	if (ProcessXML(XMLfile) == false) {
		LOG("[ERROR] ProcessXML as errors loading XML file : %s\n", XMLfile.data());
		return;
	}

	Document sub_tree;
	Filter(true, viewer_priority, jsonOriginal, sub_tree, sub_tree.GetAllocator());

	//Json::Show(sub_tree);
	if (isValidJSON(json_, sub_tree) == false) {
		LOG("[ERROR] The XML dont have a valid structure to import, XML file : %s\n", XMLfile.data());
		return;
	}

	Combine(jsonOriginal, json_, jsonOriginal.GetAllocator());
}

void Json::Export(const Value& jsonOriginal, std::string_view viewer_priority, std::string_view AdressXMLfile, std::string_view AdressXSDfile) const
{
	//Show(jsonOriginal);
	Document sub_tree;
	Filter(false, viewer_priority, jsonOriginal, sub_tree, sub_tree.GetAllocator());
	//Show(sub_tree);

	XML xml(std::move(sub_tree), AdressXMLfile, AdressXSDfile);
}

// "Type::SubType" --> {"Type", "SubType"}
std::pair<std::string, std::string> Json::GetTypeAndSubtype(std::string_view str)
{
	static constexpr std::string_view delimiter = "::";
	size_t pos = str.find(delimiter);

	if (pos == std::string_view::npos) {
		return { std::string(str), std::string("") };
	}
	return { std::string(str.substr(0, pos)), std::string(str.substr(pos + delimiter.length())) };
}

// Recursive Function to reconstruct from XML --> JSON 
void Json::XmlToJson(xml_node<>* XMLnode, Value& JSONnode, Document::AllocatorType& allocator)
{
	Value json_child;

	if (XMLnode->first_node() && XMLnode->first_node()->first_node()) {
		json_child.SetObject();
		for (xml_node<>* child = XMLnode->first_node(); child; child = child->next_sibling())
		{
			XmlToJson(child, json_child, allocator);
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
const Value& Json::GetDatatype(const Value& datatype) {
	static const Value& _datatype = datatype;
	return _datatype;
}

// "Type::SubType" is in datatype structure ?
bool Json::IsValidTypeAndSubtype(std::string_view str)
{
	const Value& datatype = Json::GetDatatype();

	auto [type, subtype] = Json::GetTypeAndSubtype(str);

	return (subtype == "") ? datatype.HasMember(type.data()) :
		datatype.HasMember(type.data()) && datatype[type.data()].HasMember(subtype.data());
}

// Unknow Variable --> { Enum or ReadOnly or Number or ... }
VariableType Json::DetermineVariableType(const Value& node)
{
	if (node.IsObject())
	{
		bool hasTyp = node.HasMember("Type") && node["Type"].IsString() && IsValidTypeAndSubtype(node["Type"].GetString());
		bool hasVal = node.HasMember("Value") && node["Value"].IsString();
		bool hasMin = node.HasMember("Min") && node["Min"].IsString();
		bool hasMax = node.HasMember("Max") && node["Max"].IsString();

		if (hasTyp && hasVal && hasMin && hasMax) { return VariableType::Number; }
		if (hasTyp && hasVal)
		{
			auto [type, subtype] = Json::GetTypeAndSubtype(node["Type"].GetString());
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