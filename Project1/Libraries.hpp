#pragma once
#include <iostream>
#include <string>
#include <string_view>
#include <fstream>
#include <sstream>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
using namespace rapidjson;

#include "rapidxml_utils.hpp"
#include "rapidxml.hpp"
using namespace rapidxml;

#include <boost/sml.hpp>
namespace sml = boost::sml;

#include "lfs.h"
#include "lfs_util.h"