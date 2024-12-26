#include "Libraries.hpp"

void GenerateHeader(const std::string& jsonContent) {
    std::ofstream headerFile("generated.hpp");

    headerFile << "#ifndef GENERATED_HPP\n";
    headerFile << "#define GENERATED_HPP\n\n";
    headerFile << "const char* config_json =  R\"JSON(" << jsonContent << ")JSON\";\n\n";
    headerFile << "#endif // GENERATED_HPP\n";
    headerFile.close();
}

std::string Remove_Unwanted_Characters(const std::string& str) {
    std::string result = str;

    result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
    result.erase(std::remove(result.begin(), result.end(), '\r'), result.end());
    result.erase(std::remove(result.begin(), result.end(), '\t'), result.end());
    result.erase(std::remove(result.begin(), result.end(), ' '), result.end());

    return result;
}

std::string readJsonFile(const std::string& filePath) {
    std::ifstream jsonFile(filePath);

    if (!jsonFile.is_open()) {
        std::cerr << "Error al abrir el archivo JSON." << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << jsonFile.rdbuf();

    return Remove_Unwanted_Characters(buffer.str());
}

//int main() {
//    std::string jsonContent = readJsonFile("config.json");
//
//    if (!jsonContent.empty()) {
//        GenerateHeader(jsonContent);
//    }
//
//    return 0;
//}