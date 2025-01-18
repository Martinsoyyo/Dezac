//#pragma once
//#include "Libraries.hpp"
//
//#include "XML.hpp"
//#include "Json.hpp"
//#include "Log.hpp"
//
//class FileManager {
//public:
//	FileManager();
//
//	FileManager(const Document&);
//
//	static std::optional<std::vector<char>> Load(std::string_view filename);
//	static bool Store(std::string_view filename, std::string_view content);
//	static bool SendToMicroSD(std::string_view filename, std::string_view content);
//};
//
///////////////////////////////////////////////////
//inline FileManager FILEMANAGER; /// OBJETO GLOBAL
///////////////////////////////////////////////////
//

#ifndef APPLICATION_FILEMANAGER_FILEMANAGER_HPP_
#define APPLICATION_FILEMANAGER_FILEMANAGER_HPP_

#include "Libraries.hpp"

// Definiciones para los par�metros de la Flash simulada
#define LFS_BLOCK_SIZE  256   // Tama�o del bloque simulado
#define LFS_BLOCK_COUNT 64    // N�mero de bloques disponibles

class FileManager {
public:
	FileManager() {
		std::memset(flash_buffer, 0xFF, FLASH_SIZE); // Inicializa con 0xFF (Flash borrada)

		cfg.context = nullptr;
		cfg.read = flash_read;
		cfg.prog = flash_prog;
		cfg.erase = flash_erase;
		cfg.sync = block_device_sync;
		cfg.block_size = LFS_BLOCK_SIZE;
		cfg.block_count = LFS_BLOCK_COUNT;
		cfg.lookahead_size = 8;
		cfg.block_cycles = 500;

		// Configuraci�n de lectura/escritura
		cfg.read_size = 2;
		cfg.prog_size = 2;
		cfg.cache_size = 8;

		int error = lfs_mount(&lfs, &cfg);
		if (error != LFS_ERR_OK)
		{
			lfs_format(&lfs, &cfg);
			printf("FileSystem formating...\n");
			error = lfs_mount(&lfs, &cfg);
			if (error != LFS_ERR_OK) {
				printf("[ERROR] FileSystem formating failed.\n");
			}
		}
		else
		{
			printf("FileSystem monted.\n");
		}

	}

	~FileManager() {
		lfs_unmount(&lfs);
	}

	auto Read(std::string_view filename)
	{
		std::ifstream file(filename.data(), std::ios::binary | std::ios::ate); // Open in binary and at the end

		size_t size = (size_t)file.tellg();
		file.seekg(0, std::ios::beg);
		std::vector<char> buffer(size + 1);

		if (!file.read(buffer.data(), size)) {
			LOG("[ERROR] Could not read the file: %s\n", filename.data());
		}

		buffer.push_back('\0');
		return buffer;
	}


	// Escribe datos en un archivo
	void SendToMicroSD(std::string_view filename, std::string_view data)
	{
		lfs_file_t file;
		if (lfs_file_open(&lfs, &file, filename.data(), LFS_O_WRONLY | LFS_O_CREAT) == LFS_ERR_OK) {
			lfs_file_write(&lfs, &file, data.data(), (lfs_size_t)data.size());
			lfs_file_close(&lfs, &file);
			std::cout << "[INFO] Archivo escrito: " << filename << "\n";
		}
		else {
			std::cerr << "[ERROR] No se pudo abrir el archivo para escribir: " << filename << "\n";
		}
	}

	// Lee datos de un archivo
	std::optional<std::string> Load(std::string_view filename) {
		//Simulo que ya esta guardado en RAM. en STM32 sera flash.
		SendToMicroSD(filename, Read(filename).data());


		lfs_file_t file;
		char buffer[256*256] = { 0 };

		if (lfs_file_open(&lfs, &file, filename.data(), LFS_O_RDONLY) == LFS_ERR_OK) {
			int read_size = lfs_file_read(&lfs, &file, buffer, sizeof(buffer) - 1);
			buffer[read_size] = '\0';
			lfs_file_close(&lfs, &file);
			std::cout << "[INFO] Archivo le�do: " << filename << "\n";
			return std::string(buffer);
		}
		else {
			std::cerr << "[ERROR] No se pudo abrir el archivo para leer: " << filename << "\n";
			return std::nullopt;
		}
	}

private:
	static constexpr size_t FLASH_SIZE = LFS_BLOCK_SIZE * LFS_BLOCK_COUNT;
	static inline char flash_buffer[FLASH_SIZE] = {};

	lfs_t lfs;              // Estructura principal de LittleFS
	struct lfs_config cfg;  // Configuraci�n del dispositivo para LittleFS

	// Funci�n de sincronizaci�n (no hace nada en este caso)
	static int block_device_sync(const struct lfs_config* c) {
		return LFS_ERR_OK;
	}

	// Simula la lectura de la Flash
	static int flash_read(const struct lfs_config* context, lfs_block_t block, lfs_off_t offset, void* buffer, lfs_size_t size) {
		std::memcpy(buffer, flash_buffer + block * LFS_BLOCK_SIZE + offset, size);
		return LFS_ERR_OK;
	}

	// Simula la escritura en la Flash
	static int flash_prog(const struct lfs_config* context, lfs_block_t block, lfs_off_t offset, const void* buffer, lfs_size_t size) {
		std::memcpy(flash_buffer + block * LFS_BLOCK_SIZE + offset, buffer, size);
		return LFS_ERR_OK;
	}

	// Simula el borrado de un bloque de Flash
	static int flash_erase(const struct lfs_config* context, lfs_block_t block) {
		std::memset(flash_buffer + block * LFS_BLOCK_SIZE, 0xFF, LFS_BLOCK_SIZE);
		return LFS_ERR_OK;
	}
};

/////////////////////////////////////////////////
inline FileManager FILEMANAGER; /// OBJETO GLOBAL
/////////////////////////////////////////////////

#endif /* APPLICATION_FILEMANAGER_FILEMANAGER_HPP_ */
