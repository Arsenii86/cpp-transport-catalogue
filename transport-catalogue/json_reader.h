#pragma once
#include <sstream>
#include <map>
#include <vector>
#include "json.h"
#include "svg.h"
#include "map_renderer.h"
#include "transport_catalogue.h"
namespace json_reader{  
    svg::Color ToRgbOrRgba(json:: Node node);
    
    void FillingCatalog  (const json::Dict& requests, transport_directory::tr_cat::TransportCatalogue& catalogue, std::vector<std::string_view>& stops_all, std::vector<std::pair<std::string,bool>>& bus_all);
    
    json::Node GetStatInfo(const json::Dict& requests, transport_directory::tr_cat::TransportCatalogue& catalogue, std::string& svg_file);
    
   mp_rend::MapRenderer SetSettingsForRenderMap(const json::Dict& requests );
    
   json::Node ReadAndProcessJsonFile(std::istream& input,transport_directory::tr_cat::TransportCatalogue& catalogue); 
    
   void OutJsonFile(const json::Node& stat_info, std::ostream& output); 
    
    
    
}
    

