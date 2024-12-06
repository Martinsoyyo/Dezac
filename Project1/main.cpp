#include "Libraries.hpp"

int main() {
	//rapidxml::xml_document<> doc;

	const char* json = "{\"project\":\"rapidjson\",\"stars\":10}";
	Document d;
	d.Parse(json);

	system("pause");
}