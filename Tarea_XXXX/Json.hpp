#pragma once
#include "Libraries.hpp"

#include "Log.hpp"
#include "XML.hpp"

class ViewPermission {
public:
	ViewPermission(std::string_view MEMBER = "") : members_(MEMBER) {}

	bool HasMember(std::string_view str) const {
		for (char c : str) {
			if (members_.find(c) != std::string::npos) {
				return true;
			}
		}
		return false;
	}

	void Add(std::string_view str) {
		for (char c : str) {
			if (members_.find(c) == std::string::npos) {
				members_ += c;
			}
		}
	}

	void Remove(std::string_view str) {
		for (char c : str) {
			auto pos = members_.find(c);
			if (pos != std::string::npos) {
				members_.erase(pos, 1);
			}
		}
	}

	const std::string Get() const { return members_; }


private:
	std::string members_;
};

enum class VariableType { Enum, Number, ReadOnly, String, Date, Time, NotAVariable };

class PostProcess;

class Json {
public:
	void Import(
		Document& jsonOriginal,
		std::string_view viewer_priority,
		std::string_view pathXMLfile);

	void Export(
		const Value& jsonOriginal,
		std::string_view viewer_priority,
		std::string_view AdressXMLfile,
		std::string_view AdressXSDfile) const;

	// Show JSON tree in Pretty format. (to easy debug)
	static void Show(const Value& json);

	// Initialize static data in Json Class. (lazy initialization)
	static const Value& GetDatatype(const Value& datatype = {});


private:
	friend class XML;
	friend class PostProcess;
	friend class DataManager;

	// Checks XML file compatibility for shape and datatype, and create json_ for later. 
	bool ProcessXML(std::string_view fileFromXML);

	// Checks if the values has a correct type/subtype, formats, limits etc...
	bool checkTypeBounds(const Value& original, std::string_view, std::string_view) const;

	// Comprare JSON to check if is the same structure.
	bool isValidJSON(const Value& source, const Value& original) const;

	// Combine JSON with the new values from Import.
	void Combine(Value& to, const Value& from, Document::AllocatorType& alloc) const;

	//----------------------------------------------------------------------------
	// STATICS MEMBERS 
	//----------------------------------------------------------------------------

	// "Type::SubType" is in datatype structure ?
	static bool IsValidTypeAndSubtype(std::string_view type);
		
	// Unknow Variable -->  { Enum or ReadOnly or Number or NotaVariable }
	static VariableType DetermineVariableType(const Value& node);

	// "Type::SubType" --> {"Type", "SubType"}
	static std::pair<std::string, std::string> GetTypeAndSubtype(std::string_view str);

	// Recursive Function to reconstruct from XML --> JSON (be aware of shape of this JSON)
	static void XmlToJson(xml_node<>* source, Value& destination, Document::AllocatorType& alloc);

	// Get sub-TREE from the original JSON according to the viewer_priority.
	static bool Filter(
		bool ignoreHide,
		std::string_view viewer_priority,
		const Value& source,
		Value& destination,
		Document::AllocatorType& alloc,
		ViewPermission parent);

private:
	Document json_;
};

