//#include "DataManager.hpp"
#include "FileManager.hpp"
//#include "PostProcess.hpp"
#include "Log.hpp"

void APP()
{
	//const auto& _doc = DATAMANAGER.Get_JSON("Config.json", { "FileManager" });
	//Json::Show(_doc);

	//// Escribir un archivo
	//FILEMANAGER.SendToMicroSD("test.txt", "Hello LittleFS!");

	//// Leer el archivo
	//auto data = FILEMANAGER.Load("test.txt");

	//if (data.has_value())
	//{
	//	std::cout << "Contenido leído: " << data.value() << "\n";
	//}


}

int main()
{
	APP();
	//DATAMANAGER.Import("LogicCircuit.json", "*");
	//POSTPROCESS.Do_Post_Process("LogicCircuit.json");
	//DATAMANAGER.Store("LogicCircuit.json");
	//DATAMANAGER.Export("LogicCircuit.json", "*");
	//DATAMANAGER.Export("LogicCircuit.json", "C");
	//DATAMANAGER.Export("LogicCircuit.json", "I");
	//DATAMANAGER.Export("LogicCircuit.json", "U");


	////DATAMANAGER.Store("Datatypes.json");

	//DATAMANAGER.Import("Center.json", "*");
	//POSTPROCESS.Do_Post_Process("Center.json");
	//DATAMANAGER.Store("Center.json");
	////DATAMANAGER.Export("Center.json", "*");


	//DATAMANAGER.Export("Center.json", "*");
	//DATAMANAGER.Export("Center.json", "O");
	//DATAMANAGER.Export("Center.json", "C");
	//DATAMANAGER.Export("Center.json", "A");
	//DATAMANAGER.Export("Center.json", "I");
	//DATAMANAGER.Export("Center.json", "D");
	//DATAMANAGER.Export("Center.json", "X");
	//DATAMANAGER.Export("Center.json", "L");
	//DATAMANAGER.Export("Center.json", "P");
	//DATAMANAGER.Export("Center.json", "T");
	//DATAMANAGER.Export("Center.json", "R");
	//DATAMANAGER.Export("Center.json", "U");

	system("pause");
}
