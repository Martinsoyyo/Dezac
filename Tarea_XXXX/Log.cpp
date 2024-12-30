#include "Log.hpp"

void CRASH(const char* str) {
	std::string full_format = std::string(str);
	size_t pos = full_format.find("ERROR");
	if (pos != std::string::npos) {

		while (true)
		{
			//....
		}
	}
}