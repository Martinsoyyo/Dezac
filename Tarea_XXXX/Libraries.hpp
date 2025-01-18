#pragma once

#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <functional>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <optional>
#include <set>
#include <stack>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
using namespace rapidjson;

//#define RAPIDXML_NO_EXCEPTIONS
#include <RapidXML/rapidxml.hpp>
#include <RapidXML/RapidXMLSTD.hpp>
#include <RapidXML/rapidxml_print.hpp>
#include <RapidXML/rapidxml_utils.hpp>
using namespace rapidxml;

#define LFS_TRACE
#include "LittleFS/lfs.h"
#include <LittleFS/lfs_util.h>