//#include "FileManager.hpp"
//
//FileManager::FileManager()
//{
//	LOG("Create FileManager::FileManager()\n");
//};
//
//FileManager::FileManager(const Document& doc)
//{
//
//	LOG("Create FileManager::FileManager()\n");
//};
//
//
//std::optional<std::vector<char>> FileManager::Load(std::string_view filename)
//{
//	std::ifstream file(filename.data(), std::ios::binary | std::ios::ate); // Open in binary and at the end
//
//	if (!file.is_open()) {
//		LOG("[ERROR] Could not open the file: %s\n", filename.data());
//		return std::nullopt;
//	}
//
//	size_t size = (size_t) file.tellg();
//	file.seekg(0, std::ios::beg); 
//	std::vector<char> buffer(size + 1);
//
//	if (!file.read(buffer.data(), size)) {
//		LOG("[ERROR] Could not read the file: %s\n", filename.data());
//		return std::nullopt;
//	}
//
//	buffer.push_back('\0');
//	return buffer;
//}
//
//bool FileManager::Store(std::string_view filename, std::string_view str)
//{
//	//TODO : reemplazar cuando este implementado litleFS.
//	std::ofstream ofs(filename.data());
//	if (!ofs.is_open()) {
//		LOG("[ERROR] Error to open file to write: %s\n", filename.data());
//		return false;
//	}
//
//	ofs << str.data();
//	ofs.close();
//	return true;
//};
//
//bool FileManager::SendToMicroSD(std::string_view filename, std::string_view str)
//{
//	//TODO : reemplazar cuando este implementado la clase MicroSD.
//	
//	std::ofstream ofs(filename.data());
//	if (!ofs.is_open()) {
//		LOG("[ERROR] Error to open file to write: %s\n", filename.data());
//		return false;
//	}
//
//	ofs << str.data();
//	ofs.close();
//	return true;
//};