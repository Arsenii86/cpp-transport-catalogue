#include "json_reader.h"
#include "json_builder.h"
namespace json_reader{
    
   
    
    
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
                   const json::Dict& requests_br=node_br.AsDict();
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
                   const json::Dict& requests_br=node_br.AsDict();
                   if(requests_br.at("type"s).AsString() == "Stop"s ){                       
                        const std::string& stop_name_from = requests_br.at("name"s).AsString();
                        if (requests_br.at("road_distances"s).IsDict()){
                        const json::Dict& stop_geo_dist = requests_br.at("road_distances"s).AsDict();
                               for (const auto& [stop_name_to, node_dist]:stop_geo_dist){
                                   const auto dist_value = std::to_string(node_dist.AsInt());
                                   catalogue.InsertStopDist(stop_name_from, stop_name_to, dist_value);
                               }
                       }
                   }
               }
            
                for (const auto& node_br:base_requests){
                   const json::Dict& requests_br = node_br.AsDict();
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
    
    
     json::Node GetStatInfo(const json::Dict& requests, transport_directory::tr_cat::TransportCatalogue& catalogue, std::string& svg_file){
        using namespace std::literals;
        json::Builder answer=json::Builder{};//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
        if (requests.at("stat_requests"s).IsArray()){            
            json::ArrayItemContext array_=answer.StartArray();//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            const json::Array& stat_requests = requests.at("stat_requests"s).AsArray();
           
            for (const auto& node_sr:stat_requests){
                   const json::Dict& requests_sr = node_sr.AsDict(); 
                   if(requests_sr.at("type"s).AsString() == "Stop"s ){
                       const std::string& stop_name = requests_sr.at("name"s).AsString();
                       const int request_id = requests_sr.at("id"s).AsInt();                       
                       if (catalogue.FindStop(stop_name) != nullptr){ 
                           const std::set<std::string_view>* bus_thr_stop = catalogue.GetBusThroughStop(stop_name); 
                           json::DictItemContext dict_=array_.StartDict();//!!!!!!!!!!!!!!!!!!!!!!!
                           json::ArrayItemContext array_=dict_.Key("buses"s).StartArray();//!!!!!!!!!!!!!!!!!!!!!
                           if (bus_thr_stop!=nullptr){ 
                                   json::ValueContextArr value_arr=array_.Value(std::string(*(*bus_thr_stop).begin()));//!!!!!!!!!!
                                   for (auto iter=++(*bus_thr_stop).begin();iter!= (*bus_thr_stop).end();++iter){
                                        value_arr.Value(std::string(*iter));//!!!!!!!!!!!!!!!!!!!!
                                   }
                                value_arr.EndArray();//!!!!!!!!!!!!!!!!!!!
                           }
                           else array_.EndArray();//!!!!!!!!!!!!!!!!!!!!!!!
                           dict_.Key("request_id"s).Value(request_id).EndDict();//!!!!!!!!!!!!!!!!!!!!!!!
                           
                       }                        
                       else {
                           array_.StartDict()//!!!!!!!!!!!!!!!!!!!!!!!
                               .Key("request_id").Value(request_id)//!!!!!!!!!!!!!!!!!!!!!!!
                               .Key("error_message").Value("not found")//!!!!!!!!!!!!!!!!!!!!!!!
                               .EndDict();//!!!!!!!!!!!!!!!!!!!!!!!
                       }                       
                   }             
                   if(requests_sr.at("type"s).AsString() == "Bus"s ){
                         const std::string& bus_name = requests_sr.at("name"s).AsString();
                         const int request_id = requests_sr.at("id"s).AsInt();                        
                          if (catalogue.FindRoute(bus_name) != nullptr){ 
                            json::DictItemContext dict_=array_.StartDict();//!!!!!!!!!!!!!!!!!!!!!!!    
                            const auto route_inf = catalogue.GetRoutInform(bus_name);
                            int stop_number = route_inf.stop_number;
                            int unique_stop_number = route_inf.unique_stop_number;                
                            double route_road_lenght = route_inf.route_road_lenght;
                            double curvature = route_inf.curvature;
                            dict_.Key("curvature").Value(curvature)//!!!!!!!!!!!!!!!!
                                  .Key("request_id").Value(request_id)//!!!!!!!!!!!!!!!!
                                  .Key("route_length").Value(route_road_lenght)//!!!!!!!!!!!!!!!!
                                  .Key("stop_count").Value(stop_number)//!!!!!!!!!!!!!!!!
                                  .Key("unique_stop_count").Value(unique_stop_number)//!!!!!!!!!!!!!!!!
                                  .EndDict();//!!!!!!!!!!!!!!!!  
                          }
                          else { 
                            array_.StartDict().
                                      Key("request_id").Value(request_id).
                                      Key("error_message").Value("not found").
                                      EndDict();
                          }                       
                     }                    
                    if(requests_sr.at("type"s).AsString() == "Map"s ){
                            const int request_id = requests_sr.at("id"s).AsInt();
                            array_.StartDict().
                                   Key("request_id").Value(request_id).
                                   Key("map").Value(svg_file).
                                   EndDict();   
                     }                
                }  
            array_.EndArray();
        }        
        return answer.Build();
    }  
    
    
    mp_rend::MapRenderer SetSettingsForRenderMap(const json::Dict& requests ){
    using namespace std::literals; 
    mp_rend::MapRenderer MR;
    if (requests.at("render_settings"s).IsDict()){   
       const json::Dict& render_st=requests.at("render_settings"s).AsDict();
       for(const auto& [setting,value]:render_st){
            if (setting=="width"){
                double width=value.AsDouble();
                MR.SetWidth(width);
            }
            else if (setting=="height"){
                double height=value.AsDouble();
                MR.SetHeight(height);
            }
            else if (setting=="padding"){
                double padding=value.AsDouble();
                MR.SetPadding(padding);
            }
            else if (setting=="line_width"){
                double line_width=value.AsDouble();
                MR.SetLineWidth(line_width);
            }
            else if (setting=="stop_radius"){
                double stop_radius=value.AsDouble();
                MR.SetStopRadius(stop_radius);
            }
            else if (setting=="underlayer_width"){
                double underlayer_width=value.AsDouble();
                MR.SetUnderlayerWidth(underlayer_width);
            }
            else if (setting=="bus_label_font_size"){
                int bus_label_font_size=value.AsInt();
                MR.SetBusLabelFontSize(bus_label_font_size);
            }
            else if (setting=="stop_label_font_size"){
                int stop_label_font_size=value.AsInt();
                MR.SetStopLabelFontSize(stop_label_font_size);
            }
            else if (setting=="bus_label_offset"){
                const auto values_arr=value.AsArray();
                double a=values_arr[0].AsDouble();
                double b=values_arr[1].AsDouble();
                svg::Point point(a,b);
                MR.SetBusLabelOffset(point);
            }
            else if (setting=="stop_label_offset"){
                const auto values_arr=value.AsArray();
                double a=values_arr[0].AsDouble();
                double b=values_arr[1].AsDouble();
                svg::Point point(a,b);
                MR.SetStopLabelOffset(point);
            }	
            else if (setting=="underlayer_color"){
                svg::Color  color;
                color=json_reader::ToRgbOrRgba(value);			
                MR.SetUnderlayerColor(color);
            }
            else if (setting=="color_palette"){
                const auto values_arr=value.AsArray();                           
                std::vector <svg::Color> color_palette;
                for(const auto& val_arr:values_arr){
                    svg::Color  color;
                    color=json_reader::ToRgbOrRgba(val_arr);
                    color_palette.push_back(color);                
                }
                MR.SetColorPalette(color_palette);
            }
        } 
    }
        return MR;
    };
    
   
    json::Node ReadAndProcessJsonFile(std::istream& input,transport_directory::tr_cat::TransportCatalogue& catalogue){
         using namespace std::literals;  
        //Создание json документа
        json::Document doc=json::Load(input);
        
        json::Node node_root = doc.GetRoot();
        if(node_root.IsDict()){
            const json::Dict& requests = node_root.AsDict(); 
            std::vector<std::string_view> stops_all;
            std::vector<std::pair<std::string,bool>> bus_all;
            
            FillingCatalog  (requests ,catalogue ,stops_all ,bus_all); 
            
            mp_rend::MapRenderer MR=json_reader::SetSettingsForRenderMap(requests);

            std::string output_svg;////////строка длявывода svg
            
            mp_rend::RenderMap(MR ,catalogue , stops_all , bus_all , output_svg);
            
            json::Node stat_info= json_reader::GetStatInfo(requests ,catalogue, output_svg ); 
            
            return stat_info;      
        }
       else{
             return json::Node {"null"s};
        }
    } ; 
    
   void OutJsonFile(const json::Node& stat_info, std::ostream& output){
        
        json::Document doc(stat_info);
        json::Print(doc, output);
    } 
    
    
}
    
    
