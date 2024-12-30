#pragma once 
#include <array>
#include <string_view>

enum class LanguageType { English, Spanish, French };

class Language {
public:
	constexpr Language(const LanguageType& LANG) :Lang(LANG) {};

	constexpr std::string_view operator()(std::string_view key) const {
		if (Lang == LanguageType::English) return key;

		size_t left  = 0;
		size_t right = translations.max_size();

		while (left < right) {
			std::size_t mid = left + (right - left) / 2;
			const auto& mid_elem = translations[mid];

			if (key < mid_elem[0]) { right = mid; }
			else if (key > mid_elem[0]) { left = mid + 1; }
			else { return translations[mid][size_t(Lang)]; }
		}
		return key;
	}

	void setLanguage(LanguageType LANG) { Lang = LANG; }

	//std::string_view operator()(std::string_view key) const 
	//{
	//	for (auto& elem : translations) {
	//		if (elem[0] == key) {
	//			return elem[size_t(Lang)];
	//		}
	//	}
	//	return key;
	//}


private:
	LanguageType Lang;

	static constexpr std::array<std::array<std::string_view, 3>, 41> translations = { {
			{"COM1_Protocol"        , "Protocolo_COM1"              , "Protocole_COM1"          },
			{"COM2_Protocol"        , "Protocolo_COM2"              , "Protocole_COM2"          },
			{"COM3_Protocol"        , "Protocolo_COM3"              , "Protocole_COM3"          },
			{"CONFIGURATION"        , "Configuracion"               , "Configuration"           },
			{"Centre_Type"          , "Tipo_Centro"                 , "Type_De_Centre"          },
			{"Centre_Type_RO"       , "Tipo_Centro_RO"              , "Type_De_Centre_RO"       },
			{"Cubicle_Type"         , "Tipo_De_Cubiculo"            , "Type_De_Cubicle"         },
			{"Current_Channel_1"    , "Canal_De_Corriente_1"        , "Canal_De_Courant_1"      },
			{"DIGITAL_CHANNELS"     , "Canales_Digitales"           , "Canaux_Digitaux"         },
			{"DIGITAL_INPUTS"       , "Entradas_Digitales"          , "Entrees_Digitales"       },
			{"DIGITAL_OUTPUTS"      , "Salidas_Digitales"           , "Sorties_Digitales"       },
			{"Dpf_Type"             , "Tipo_Dpf"                    , "Type_Dpf"                },
			{"Filter_Time"          , "Tiempo_De_Filtro"            , "Temps_De_Filtre"         },
			{"Frecuency"            , "Frecuencia"                  , "Frequence"               },
			{"GENERAL"              , "General"                     , "General"                 },
			{"GENERAL_SETTINGS"     , "Configuracion_General"       , "Parametres_Generaux"     },
			{"Grid_Type"            , "Tipo_De_Grilla"              , "Type_De_Grille"          },
			{"Hardware_Mode"        , "Modo_De_Hardware"            , "Mode_De_Hardware"        },
			{"Hardware_Type"        , "Tipo_De_Hardware"            , "Type_De_Hardware"        },
			{"Language"             , "Idioma"                      , "Langue"                  },
			{"Logical_Device"       , "Dispositivo_Logico"          , "Dispositif_Logique"      },
			{"Logical_Device_RO"    , "Dispositivo_Logico_RO"       , "Dispositif_Logique_RO"   },
			{"Measure_Schema"       , "Esquema_De_Medida"           , "Schema_De_Mesure"        },
			{"Neutral"              , "Neutro"                      , "Neutre"                  },
			{"POSITIONS"            , "Posiciones"                  , "Positions"               },
			{"PRIMARY"              , "Primario"                    , "Primarie"                },
			{"Phase"                , "Fase"                        , "Phase"                   },
			{"PowerOn_Delay_Cards"  , "Retraso_De_Encendido_Cartas" , "Delai_DActivation_Cards" },
			{"PowerOn_Delay_SOM"    , "Retraso_De_Encendido_Som"    , "Delai_DActivation_Som"   },
			{"Pulse_Time"           , "Tiempo_De_Pulso"             , "Temps_De_Pulsion"        },
			{"RELATION_LPCT"        , "Relacion_LPCT"               , "Relation_LPCT"           },
			{"SECONDARY"            , "Secundario"                  , "Secundarie"              },
			{"Settings_Value"       , "Valor_De_Config"             , "Valeur_De_Config"        },
			{"Time_Zone"            , "Zona_Horaria"                , "Fuseau_Horaire"          },
			{"Type"                 , "Tipo"                        , "Type"                    },
			{"Type_Ro"              , "Tipo_Ro"                     , "Type_Ro"                 },
			{"Value"                , "Valor"                       , "Valeur"                  },
			{"Voltage_Channel_1"    , "Canal_De_Voltaje_1"          , "Canal_De_Tension_1"      },
			{"Voltage_Channel_2"    , "Canal_De_Voltaje_2"          , "Canal_De_Tension_2"      },
			{"Voltage_Channel_3"    , "Canal_De_Voltaje_3"          , "Canal_De_Tension_3"      },
			{"Voltage_Channel_4"    , "Canal_De_Voltaje_4"          , "Canal_De_Tension_4"      }
	} };
};

///////////////////////////////////////////////////////////
inline Language T(LanguageType::English); /// OBJETO GLOBAL
///////////////////////////////////////////////////////////