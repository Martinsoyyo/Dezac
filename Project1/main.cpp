#include "Libraries.hpp"
#include "generated.hpp"

#define LFS_BLOCK_SIZE  256  // Tamaño del bloque simulado (puede ser diferente)
#define LFS_BLOCK_COUNT 64  // Número de bloques disponibles

// Buffer que simula la memoria Flash
char* flash_buffer = nullptr;

int block_device_sync(const struct lfs_config* c)
{
	return 0;
}

// Simula la función de lectura desde Flash (realmente lee desde el buffer en RAM)
int flash_read(const struct lfs_config* context, lfs_block_t block, lfs_off_t offset, void* buffer, lfs_size_t size) {
	std::memcpy(buffer, flash_buffer + block * LFS_BLOCK_SIZE + offset, size);
	return LFS_ERR_OK;  
}

// Simula la función de escritura en Flash (realmente escribe en el buffer en RAM)
int flash_prog(const struct lfs_config* context, lfs_block_t block, lfs_off_t offset, const void* buffer, lfs_size_t size) {
	std::memcpy(flash_buffer + block * LFS_BLOCK_SIZE + offset, buffer, size);
	return LFS_ERR_OK;
}

// Simula la función de borrado de Flash (realmente pone 0 en la memoria)
int flash_erase(const struct lfs_config* context, lfs_block_t block) {
	std::memset(flash_buffer + block * LFS_BLOCK_SIZE, 0, LFS_BLOCK_SIZE);
	return LFS_ERR_OK;
}

lfs_config cfg;

void init_lfs_config() {
	cfg.context = nullptr;
	cfg.read = flash_read;
	cfg.prog = flash_prog;
	cfg.erase = flash_erase;
	cfg.sync = block_device_sync;
	cfg.block_size = LFS_BLOCK_SIZE;
	cfg.block_count = LFS_BLOCK_COUNT;
	cfg.lookahead_size = 8; // Tamaño de la búsqueda anticipada
	cfg.block_cycles = 500; // Ciclos de escritura por bloque

	// block device configuration
	cfg.read_size = 2;
	cfg.prog_size = 2;
	cfg.cache_size = 8;
}


int main() {


	flash_buffer = new char[LFS_BLOCK_SIZE * LFS_BLOCK_COUNT];  // Bloques de 512 bytes, 128 bloques

	if (!flash_buffer) {
		std::cerr << "Error al asignar memoria para el buffer de Flash." << std::endl;
		return -1;
	}

	// Inicializa la configuración de LittleFS
	init_lfs_config();

	lfs_t lfs;
	int err = lfs_mount(&lfs, &cfg);
	if (err) {
		std::cout << "Formateando..." << std::endl;
		lfs_format(&lfs, &cfg);
		lfs_mount(&lfs, &cfg);
	}

	const char* filename = "/test.txt";
	lfs_file_t file;

	// Crear archivo
	err = lfs_file_open(&lfs, &file, filename, LFS_O_RDWR | LFS_O_CREAT);
	if (err) {
		std::cerr << "Error al abrir el archivo." << std::endl;
		return -1;
	}

	// Escribir en el archivo
	const char* data = "Hola, LittleFS!";
	err = lfs_file_write(&lfs, &file, data, (lfs_size_t)std::strlen(data));
	if (err < 0) {
		std::cerr << "Error al escribir en el archivo." << std::endl;
		return -1;
	}

	//// Leer desde el archivo
	char buffer[128];
	lfs_file_rewind(&lfs, &file);
	err = lfs_file_read(&lfs, &file, buffer, sizeof(buffer));
	if (err < 0) {
		std::cerr << "Error al leer desde el archivo." << std::endl;
		return -1;
	}
	buffer[err] = '\0';
	std::cout << "Leído: " << buffer << std::endl;

	// Cerrar archivo
	lfs_file_close(&lfs, &file);

	// Liberar el buffer de memoria
	delete[] flash_buffer;


	system("pause");
	return 0;
}