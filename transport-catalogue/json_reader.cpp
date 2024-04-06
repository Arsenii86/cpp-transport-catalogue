#include "json_reader.h"
namespace json_reader{
    json::Document read_json_file(std::istream& input){
        return    json::Load(input); 
    }  
   
    void out_json_file(const json::Document& doc, std::ostream& output){
        json::Print(doc, output);
    }
    
    svg::Color ToRgbOrRgba(json:: Node node){
		svg::Color  color;
	        if(node.IsString()){
                color = node.AsString();                     
	            return color;	}
                else if(node.IsArray()){				
                    const auto value_arr=node.AsArray();
                    int size = static_cast<int>(value_arr.size());                    
                    if (size == 4){                         
                        int a = static_cast<uint8_t>(value_arr[0].AsInt());
                        int b = static_cast<uint8_t>(value_arr[1].AsInt());
                        int c = static_cast<uint8_t>(value_arr[2].AsInt());
                        double d = value_arr[3].AsDouble();                        
                        svg::Rgba rgba(a, b, c, d);
                        color = rgba; 
                        return color;                       
                    }
                    else{
                        int a,b,c;					
                        a = static_cast<uint8_t>(value_arr[0].AsInt());
                        b = static_cast<uint8_t>(value_arr[1].AsInt());
                        c = static_cast<uint8_t>(value_arr[2].AsInt());			
                        svg::Rgb rgb(a,b,c);                        
                        color = rgb;
                        return color; 
                    }
                }				
                else    {
                    color = std::monostate{};
                    return color;}
}
 
    void FillingCatalog  (const json::Dict& requests, transport_directory::tr_cat::TransportCatalogue& catalogue , std::vector<std::string_view>& stops_all , std::vector<std::pair<std::string, bool>>& bus_all){
    using namespace std::literals;
    if (requests.at("base_requests"s).IsArray()){
               const json::Array& base_requests =requests.at("base_requests"s).AsArray();            
               for (const auto& node_br:base_requests){
                   const json::Dict& requests_br=node_br.AsMap();
                   if(requests_br.at("type"s).AsString() == "Stop"s ){
                       const std::string& stop_name=requests_br.at("name"s).AsString(); 
                       stops_all.push_back(stop_name);////для дальнейшего определения координат остановок, встречающихся на маршруте///
                       const double latitude = requests_br.at("latitude"s).AsDouble() ;  
                       const double longitude = requests_br.at("longitude"s).AsDouble() ;
                       geo::Coordinates coord{latitude,longitude};
                       catalogue.InsertStop(stop_name,coord); 
                   }
               } 
            
                for (const auto& node_br:base_requests){
                   const json::Dict& requests_br=node_br.AsMap();
                   if(requests_br.at("type"s).AsString() == "Stop"s ){                       
                        const std::string& stop_name_from = requests_br.at("name"s).AsString();
                        if (requests_br.at("road_distances"s).IsMap()){
                        const json::Dict& stop_geo_dist = requests_br.at("road_distances"s).AsMap();
                               for (const auto& [stop_name_to, node_dist]:stop_geo_dist){
                                   const auto dist_value = std::to_string(node_dist.AsInt());
                                   catalogue.InsertStopDist(stop_name_from, stop_name_to, dist_value);
                               }
                       }
                   }
               }
            
                for (const auto& node_br:base_requests){
                   const json::Dict& requests_br = node_br.AsMap();
                   if(requests_br.at("type"s).AsString() == "Bus"s ){                       
                       std::vector<std::string_view> stops_on_route;
                       const std::string& bus_name = requests_br.at("name"s).AsString();                       
                       const double is_roundtrip = requests_br.at("is_roundtrip"s).AsBool();
                       bus_all.push_back({bus_name, is_roundtrip});////для дальнейшего определения маршрута для отрисовки///
                       const json::Array& stops = requests_br.at("stops"s).AsArray();
                       for (const auto& stop:stops){
                            stops_on_route.push_back(stop.AsString());
                           }
                       if(!is_roundtrip){
                            std::vector<std::string_view> stops_(stops_on_route.begin(),stops_on_route.end());
                            stops_on_route.insert(stops_on_route.end(), std::next(stops_.rbegin()), stops_.rend());
                       }
                       
                        catalogue.InsertRoute(bus_name, stops_on_route);
                   }
               }
                
        }
    }
    
    
     void GetStatInfo(const json::Dict& requests, transport_directory::tr_cat::TransportCatalogue& catalogue, std::stringstream& svg_file, std::ostream& output){
        using namespace std::literals;
        if (requests.at("stat_requests"s).IsArray()){
            output<<'[';
            const json::Array& stat_requests = requests.at("stat_requests"s).AsArray();
            bool is_first = true;
            for (const auto& node_sr:stat_requests){
                   const json::Dict& requests_sr = node_sr.AsMap(); 
                   if(requests_sr.at("type"s).AsString() == "Stop"s ){
                       const std::string& stop_name = requests_sr.at("name"s).AsString();
                       const int request_id = requests_sr.at("id"s).AsInt();                       
                       if (catalogue.FindStop(stop_name) != nullptr){ 
                           const std::set<std::string_view>* bus_thr_stop = catalogue.GetBusThroughStop(stop_name);  
                           output << svg::StopInform(is_first,bus_thr_stop,request_id);
                           }                        
                       else {
                           output << svg::RequestNotFound(is_first, request_id);
                       }                       
                   }             
                   if(requests_sr.at("type"s).AsString() == "Bus"s ){
                         const std::string& bus_name = requests_sr.at("name"s).AsString();
                         const int request_id = requests_sr.at("id"s).AsInt();
                          if (catalogue.FindRoute(bus_name) != nullptr){                            
                            const auto route_inf = catalogue.GetRoutInform(bus_name);
                            int stop_number = route_inf.stop_number;
                            int unique_stop_number = route_inf.unique_stop_number;                
                            double route_road_lenght = route_inf.route_road_lenght;
                            double curvature = route_inf.curvature;
                            output<<svg::BusInform(is_first, stop_number, unique_stop_number, route_road_lenght, curvature, request_id);
                          }
                          else { 
                            output<<svg::RequestNotFound(is_first, request_id);
                          }                       
                     }                    
                    if(requests_sr.at("type"s).AsString() == "Map"s ){
                          const int request_id = requests_sr.at("id"s).AsInt();
                            output << svg::MapInform(is_first, svg_file, request_id);            
                     }                
                }           
        }
        output << "\n]";
    }  
}
    
    
