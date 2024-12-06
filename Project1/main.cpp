#include "Libraries.hpp"

int main() {

	const char* json = "{\"project\":\"rapidjson\",\"stars\":10}";
	Document d;
	d.Parse(json);

	system("pause");
}