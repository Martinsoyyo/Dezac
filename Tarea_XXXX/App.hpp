#pragma once
#include "Libraries.hpp"
#include "Log.hpp"

#include "FileManager.hpp"
#include "DataManager.hpp"
#include "PostProcess.hpp"

class APP {
public:
	static void Init();
	static void Run();

	inline static FileManager FILEMANAGER;
	inline static DataManager DATAMANAGER;
	inline static PostProcess POSTPROCESS;
};
