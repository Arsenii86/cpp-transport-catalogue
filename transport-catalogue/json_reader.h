#pragma once
#include <sstream>
#include "json.h"
#include "svg.h"
#include "transport_catalogue.h"
namespace json_reader{
     json::Document read_json_file(std::istream& input); 
   
     void out_json_file(const json::Document& doc, std::ostream& output);
    
    svg::Color ToRgbOrRgba(json:: Node node);
    
    void FillingCatalog  (const json::Dict& requests, transport_directory::tr_cat::TransportCatalogue& catalogue, std::vector<std::string_view>& stops_all, std::vector<std::pair<std::string,bool>>& bus_all);
    
    void GetStatInfo(const json::Dict& requests, transport_directory::tr_cat::TransportCatalogue& catalogue, std::stringstream& svg_file, std::ostream& output);
    
}
    

