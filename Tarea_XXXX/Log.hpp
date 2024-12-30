#pragma once
#include "Libraries.hpp"

inline const char* currentDateTime() {
	static char buffer[30]; // Tamaño suficiente para la fecha, hora y milisegundos

	// Obtener el tiempo actual
	auto now = std::chrono::system_clock::now();
	auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
	auto epoch = now_ms.time_since_epoch();
	long milliseconds = epoch.count() % 1000;

	// Obtener el tiempo actual en time_t para formatear la fecha y la hora
	std::time_t now_time = std::chrono::system_clock::to_time_t(now);
	std::tm now_tm = {};
	localtime_s(&now_tm, &now_time);

	// Formatear la fecha y hora
	std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &now_tm);

	// Añadir los milisegundos
	std::snprintf(buffer + std::strlen(buffer), sizeof(buffer) - std::strlen(buffer), ".%03ld", milliseconds);

	return buffer;
}

void CRASH(const char* str);

// Función de registro para modo debug
inline void log(const char* FUNCTION_NAME, const char* format, ...) {
	// Obtener la fecha y hora actual (asumiendo que currentDateTime() está definida)
	const char* dateTime = currentDateTime();

	// Imprimir la fecha, hora y nombre de la función
	std::printf("[%s] [%s] ", dateTime, FUNCTION_NAME);

	// Imprimir el mensaje formateado
	va_list args;
	va_start(args, format);
	std::vprintf(format, args);
	va_end(args);

	CRASH(format);
}

// Sobrecarga para manejar listas de inicialización
template <typename T>
inline void log(const char* FUNCTION_NAME, const char* format, std::initializer_list<T> list) {
	// Construir el formato completo con un marcador de posición para la lista
	std::string full_format = std::string(format) + " {%s}" + "\n";

	// Convertir la lista a una cadena usando un buffer estático
	char buffer[1024]; // Ajusta el tamaño según tus necesidades
	buffer[0] = '\0';
	for (const auto& item : list) {
		strcat_s(buffer, sizeof(buffer), std::string(item).c_str());
		strcat_s(buffer, sizeof(buffer), " ");
	}

	// Llamar a la función log original con los argumentos formateados
	log(FUNCTION_NAME, full_format.c_str(), buffer);

	CRASH(format);
}

#ifdef NDEBUG // RELEASE MODE
#define MY_ASSERT(expr)	do { } while (0) 
#define LOG(fmt, ...)	do { } while (0)

#else		  // DEBUG MODE
#define LOG(fmt, ...) log(__FUNCTION__, fmt, ##__VA_ARGS__)

#define MY_ASSERT(expr) \
		do { \
			if (!(expr)) { \
				std::cerr << "Assertion failed: " << #expr << " in " << __FILE__ << " at line " << __LINE__ << std::endl; \
				throw std::runtime_error("Assertion failed: " #expr); \
			} \
		} while (0)
#endif
