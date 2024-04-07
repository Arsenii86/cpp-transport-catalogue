#pragma once
#include "domain.h"
//#include <sstream>
#include <string>
#include <set>
#include <variant>
#include "map_renderer.h"
#include "json_reader.h"
#include "transport_catalogue.h"
#include "geo.h"
//#include "svg.h"
namespace req_hand{
std::string QueryProcessor(std::istream& input,transport_directory::tr_cat::TransportCatalogue& catalogue);
}