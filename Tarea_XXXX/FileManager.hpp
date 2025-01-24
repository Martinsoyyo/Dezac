#pragma once
#include "Libraries.hpp"
#include "Log.hpp"

class FileManager {
public:
	~FileManager() { lfs_unmount(&lfs); }

	void Init(const Value& doc);

	std::optional<std::string> Read(std::string_view filename);
	void Write(std::string_view fileName, std::string_view jsonContent);

	// TODO: this is a MicroSD function. we have to move there!-----------
	std::optional<std::string> LoadFromMicroSD(std::string_view filename);
	void SaveToMicroSD(std::string_view filename, std::string_view data);
	//--------------------------------------------------------------------

private:
	static constexpr size_t LFS_BLOCK_SIZE = 256;
	static constexpr size_t LFS_BLOCK_COUNT = 256;
	static constexpr size_t FLASH_SIZE = LFS_BLOCK_SIZE * LFS_BLOCK_COUNT;
	static inline char flash_buffer[FLASH_SIZE] = {};

	lfs_t lfs;
	struct lfs_config cfg;

	// Función de sincronización (no hace nada en este caso)
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
