#include "XML.hpp"


void XML::Show(xml_node<>* node, int depth)
{
	std::cout << node->name();
	if (node->first_node() == nullptr) {
		std::cout << ": " << node->value();
	}

	std::cout << std::endl;
	for (xml_node<>* child = node->first_node(); child; child = child->next_sibling()) {
		Show(child, depth + 1);
	}
}

void XML::addXmlNode(xml_node<>* node, std::string_view key, std::string_view value = "")
{
	char* nodeName = xml_.allocate_string(key.data());
	char* nodeValue = xml_.allocate_string(value.data());
	xml_node<>* newNode = xml_.allocate_node(node_type::node_element, nodeName, nodeValue);
	node->append_node(newNode);
}

void XML::addXsdNode(xml_node<>* node, std::string_view key, std::string_view type, std::string_view displayName)
{
	xml_node<>* newNode = xsd_.allocate_node(node_type::node_element, "xs:element");
	newNode->append_attribute(xsd_.allocate_attribute("name", xsd_.allocate_string(key.data())));
	newNode->append_attribute(xsd_.allocate_attribute("orm:displayName", xsd_.allocate_string(T(displayName).data())));
	newNode->append_attribute(xsd_.allocate_attribute("type", xsd_.allocate_string(type.data())));
	node->append_node(newNode);
}

auto XML::addXsdElement(xml_node<>* node, std::string_view key)
{
	xml_node<>* element = xsd_.allocate_node(node_type::node_element, "xs:element");
	xml_node<>* complexType = xsd_.allocate_node(node_type::node_element, "xs:complexType");
	xml_node<>* sequence = xsd_.allocate_node(node_type::node_element, "xs:sequence");
	element->append_attribute(xsd_.allocate_attribute("name", xsd_.allocate_string(key.data())));
	element->append_attribute(xsd_.allocate_attribute("orm:displayName", xsd_.allocate_string(T(key).data())));

	complexType->append_node(sequence);
	element->append_node(complexType);
	node->append_node(element);

	return sequence;
}

void XML::addXsdEnums(std::string_view key, std::vector<std::string_view>&& values, std::string_view unit)
{
	if (unit != "") {
		char* nodeValue = xsdTypes_.allocate_string(key.data());
		char* UnitValue = xsdTypes_.allocate_string(unit.data());
		xml_node<>* _complex = xsdTypes_.allocate_node(node_type::node_element, "xs:complexType");
		xml_node<>* _type = xsdTypes_.allocate_node(node_type::node_element, "xs:simpleContent");
		xml_node<>* _base = xsdTypes_.allocate_node(node_type::node_element, "xs:restriction");
		_complex->append_attribute(xsdTypes_.allocate_attribute("name", nodeValue));
		_base->append_attribute(xsdTypes_.allocate_attribute("base", "tString"));

		for (const std::string_view& value : values) {
			xml_node<>* _enum = xsdTypes_.allocate_node(node_type::node_element, "xs:enumeration");
			_enum->append_attribute(xsdTypes_.allocate_attribute("value", value.data()));
			_enum->append_attribute(xsdTypes_.allocate_attribute("orm:displayName", T(value).data()));
			_base->append_node(_enum);
		}

		xml_node<>* _attrib = xsdTypes_.allocate_node(node_type::node_element, "xs:attribute");
		_attrib->append_attribute(xsdTypes_.allocate_attribute("name", "unit"));
		_attrib->append_attribute(xsdTypes_.allocate_attribute("fixed", UnitValue));
		_base->append_node(_attrib);
		_type->append_node(_base);
		_complex->append_node(_type);
		xsdTypes_.append_node(_complex);
	}
	else {
		char* nodeValue = xsdTypes_.allocate_string(key.data());
		xml_node<>* _type = xsdTypes_.allocate_node(node_type::node_element, "xs:simpleType");
		xml_node<>* _base = xsdTypes_.allocate_node(node_type::node_element, "xs:restriction");
		_type->append_attribute(xsdTypes_.allocate_attribute("name", nodeValue));
		_base->append_attribute(xsdTypes_.allocate_attribute("base", "xs:string"));

		for (const std::string_view& value : values) {
			xml_node<>* _enum = xsdTypes_.allocate_node(node_type::node_element, "xs:enumeration");
			_enum->append_attribute(xsdTypes_.allocate_attribute("value", value.data()));
			_enum->append_attribute(xsdTypes_.allocate_attribute("orm:displayName", T(value).data()));
			_base->append_node(_enum);
		}
		_type->append_node(_base);
		xsdTypes_.append_node(_type);
	}
}

void XML::addXsdTypeEnum(std::string_view type, std::string_view unit)
{
	if (std::find(xsdTypesIsPresent.begin(), xsdTypesIsPresent.end(), type) == xsdTypesIsPresent.end())
		xsdTypesIsPresent.push_back(std::string(type));
	else
		return;

	std::vector<std::string_view> enum_types;

	const Value& datatype = Json::GetDatatype();
	if (datatype.HasMember(type.data()) && datatype[type.data()].IsObject()) {
		const Value& typeObject = datatype[type.data()];

		for (auto& [name, _] : typeObject.GetObject())
			enum_types.push_back(name.GetString());
	}

	addXsdEnums(type, std::move(enum_types), unit);
}

void XML::addXsdNumber(std::string_view type, std::string_view min, std::string_view max, std::string_view unit)
{
	xml_node<>* _type = xsdTypes_.allocate_node(node_type::node_element, "xs:complexType");
	xml_node<>* _simple = xsdTypes_.allocate_node(node_type::node_element, "xs:simpleContent");
	xml_node<>* _base = xsdTypes_.allocate_node(node_type::node_element, "xs:restriction");
	xml_node<>* _min = xsdTypes_.allocate_node(node_type::node_element, "xs:minInclusive");
	xml_node<>* _max = xsdTypes_.allocate_node(node_type::node_element, "xs:maxInclusive");
	xml_node<>* _attr = xsdTypes_.allocate_node(node_type::node_element, "xs:attribute");

	_type->append_attribute(xsdTypes_.allocate_attribute("name", xsdTypes_.allocate_string(type.data())));
	_base->append_attribute(xsdTypes_.allocate_attribute("base", xsdTypes_.allocate_string("tDecimal")));
	_min->append_attribute(xsdTypes_.allocate_attribute("value", xsdTypes_.allocate_string(min.data())));
	_max->append_attribute(xsdTypes_.allocate_attribute("value", xsdTypes_.allocate_string(max.data())));

	_base->append_node(_min);
	_base->append_node(_max);
	_simple->append_node(_base);
	_type->append_node(_simple);

	if (unit != "") {
		_attr->append_attribute(xsdTypes_.allocate_attribute("name", "unit"));
		_attr->append_attribute(xsdTypes_.allocate_attribute("fixed", xsdTypes_.allocate_string(unit.data())));
		_base->append_node(_attr);
	}

	xsdTypes_.append_node(_type);
}

void XML::addXsdBasicType(std::string_view type, std::string_view extension, std::string_view attribute)
{
	xml_node<>* _type = xsdTypes_.allocate_node(node_type::node_element, "xs:complexType");
	xml_node<>* _simple = xsdTypes_.allocate_node(node_type::node_element, "xs:simpleContent");
	xml_node<>* _extension = xsdTypes_.allocate_node(node_type::node_element, "xs:extension");

	_type->append_attribute(xsdTypes_.allocate_attribute("name", xsdTypes_.allocate_string(type.data())));
	_extension->append_attribute(xsdTypes_.allocate_attribute("base", xsdTypes_.allocate_string(extension.data())));

	_type->append_node(_simple);
	_simple->append_node(_extension);

	if (attribute != "") {
		xml_node<>* _attribute = xsdTypes_.allocate_node(node_type::node_element, "xs:attribute");
		_attribute->append_attribute(xsdTypes_.allocate_attribute("name", "unit"));
		_extension->append_node(_attribute);
	}

	xsdTypes_.append_node(_type);
}

XML::XML(const Value&& json, std::string_view XMLfile, std::string_view XSDfile)
{
	if (json.IsNull()) {
		LOG("[ERROR] Json to process is empty!\n");
		return;
	}

	addXsdBasicType("tString", "xs:string", "unit");
	addXsdBasicType("tDecimal", "xs:decimal", "unit");
	addXsdBasicType("tReadOnly", "xs:string");

	jsonToXml(json, &xml_, &xsd_);
	save(XMLfile, XSDfile);
}

void XML::save(std::string_view XMLfile, std::string_view XSDfile)
{
	std::string xml_as_string = "<?xml version=\"1.0\" encoding=\"utf-8\"?> \n";
	print(std::back_inserter(xml_as_string), xml_);
	FileManager::SendToMicroSD(XMLfile, xml_as_string);

	std::string xml_head = R"(<?xml version = "1.0" encoding = "ISO-8859-1" ?>)""\n";
	std::string xml_content = R"(<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:orm="http://com.ormazabal.es/settings" attributeFormDefault="unqualified" elementFormDefault="qualified">)""\n";
	std::string xml_closure = R"(</xs:schema>)""\n";

	xml_as_string = xml_head + xml_content;
	print(std::back_inserter(xml_as_string), xsd_);
	print(std::back_inserter(xml_as_string), xsdTypes_);
	xml_as_string += xml_closure;

	FileManager::SendToMicroSD(XSDfile, xml_as_string);
}

void XML::jsonToXml(const Value& node, xml_node<>* xml_, xml_node<>* xsd_, const std::string& parent)
{
	for (const auto& [name, value] : node.GetObject()) {
		auto variable_type = Json::DetermineVariableType(value);
		auto strname = name.GetString();

		if (variable_type == VariableType::NotAVariable) {
			addXmlNode(xml_, strname);
			auto last_child = addXsdElement(xsd_, strname);
			jsonToXml(value, xml_->last_node(), last_child, parent + strname);
		}
		else if (variable_type == VariableType::ReadOnly) {
			addXmlNode(xml_, strname, value["Value"].GetString());
			addXsdNode(xsd_, strname, "tReadOnly", strname);
		}
		else {
			auto [type, subtype] = Json::GetTypeAndSubtype(value["Type"].GetString());

			const Value& datatype = Json::GetDatatype();
			std::string unit = "";
			if (subtype == "")
			{
				if (datatype[type.data()].HasMember("Unit")) {
					unit = datatype[type.data()]["Unit"].GetString();
				}
			}
			else {
				if (datatype[type.data()][subtype.data()].HasMember("Unit")) {
					unit = datatype[type.data()][subtype.data()]["Unit"].GetString();
				}
			}
			if (variable_type == VariableType::Enum) {
				addXmlNode(xml_, strname, subtype);
				addXsdNode(xsd_, strname, type, strname);
				addXsdTypeEnum(type, unit);
			}
			else if (variable_type == VariableType::Number) {
				addXmlNode(xml_, strname, value["Value"].GetString());
				addXsdNode(xsd_, strname, GenerateShortID(parent + strname), strname);

				addXsdNumber(
					GenerateShortID(parent + strname),
					AdjustDecimals(value["Min"].GetString(), value["Value"].GetString()),
					AdjustDecimals(value["Max"].GetString(), value["Value"].GetString()),
					unit
				);
			}
			else if (variable_type == VariableType::String) {
				addXmlNode(xml_, strname, value["Value"].GetString());
				addXsdNode(xsd_, strname, "tString", strname);
			}
			else if (variable_type == VariableType::Date) {
				addXmlNode(xml_, strname, value["Value"].GetString());
				addXsdNode(xsd_, strname, "xs:date", strname);
			}
			else if (variable_type == VariableType::Time) {
				addXmlNode(xml_, strname, value["Value"].GetString());
				addXsdNode(xsd_, strname, "xs:time", strname);
			}
		}
	}
}

// "5.34" --> 2 | "12.9" --> 1 | "89" --> 0
size_t CountDecimals(std::string_view str)
{
	auto pos = str.find('.');
	return (pos != std::string_view::npos) ? (str.length() - pos - 1) : 0;
}

// {"x.xxx" , "y.y"} --> "x.x" | {"x.x" , "y.yyy"} --> "x.x00" | {"x.xxx" , "y"} --> "x" 
std::string XML::AdjustDecimals(std::string_view str, std::string_view value)
{
	size_t valueDecimals = CountDecimals(value);
	size_t numDecimals = CountDecimals(str);
	int delta = valueDecimals - numDecimals;

	if (delta == 0) return std::string(str);

	std::string ret(str);
	if (delta > 0) {					  // Add extra decimal places to NUM
		if (numDecimals == 0) {
			ret += ".";
		}
		ret.append(delta, '0');
	}
	else if (delta < 0) {				  // Remove decimal places from NUM
		if (numDecimals == -delta) {
			ret.resize(ret.length() - 1); // Remove the point if necessary
		}
		ret.erase(ret.size() + delta);    // deltaDecimals is negative, so this reduces size
	}
	return ret;
}

// {any words of any length} --> {Hexa number of 32bits}
std::string XML::GenerateShortID(const std::string& input)
{
	std::hash<std::string> hasher;
	size_t hashValue = hasher(input);

	char buffer[18];
	buffer[0] = 'x';
	snprintf(buffer + 1, sizeof(buffer) - 1, "%lx", hashValue);
	return std::string(buffer);
}