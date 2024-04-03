#include "request_handler.h"
namespace req_hand{
 svg::Color ToRgbOrRgba(json:: Node node){
		svg::Color  color;
	        if(node.IsString()){
                color=node.AsString();                     
	            return color;	}
                else if(node.IsArray()){				
                    const auto value_arr=node.AsArray();
                    int size=static_cast<int>(value_arr.size());                    
                    if (size==4){                         
                        int a=static_cast<uint8_t>(value_arr[0].AsInt());
                        int b=static_cast<uint8_t>(value_arr[1].AsInt());
                        int c=static_cast<uint8_t>(value_arr[2].AsInt());
                        double d=value_arr[3].AsDouble();                        
                        svg::Rgba rgba(a,b,c,d);
                        color=rgba; 
                        return color;                       
                    }
                    else{
                        int a,b,c;					
                        a=static_cast<uint8_t>(value_arr[0].AsInt());
                        b=static_cast<uint8_t>(value_arr[1].AsInt());
                        c=static_cast<uint8_t>(value_arr[2].AsInt());			
                        svg::Rgb rgb(a,b,c);                        
                        color=rgb;
                        return color; 
                    }
                }				
                else    {
                    color=std::monostate{};
                    return color;}
}

    void SetAndRenderMap(const json::Dict& requests , transport_directory::tr_cat::TransportCatalogue& catalogue , const std::vector<std::string_view>& stops_all ,const std::vector<std::pair<std::string,bool>>& bus_all , std::ostream& output_svg ){
    using namespace std::literals;    
    mp_rend::MapRenderer MR;       
    if (requests.at("render_settings"s).IsMap()){   
       const json::Dict& render_st=requests.at("render_settings"s).AsMap();
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
                color=ToRgbOrRgba(value);			
                MR.SetUnderlayerColor(color);
            }
            else if (setting=="color_palette"){
                const auto values_arr=value.AsArray();                           
                std::vector <svg::Color> color_palette;
                for(const auto& val_arr:values_arr){
                    svg::Color  color;
                    color=ToRgbOrRgba(val_arr);
                    color_palette.push_back(color);                
                }
                MR.SetColorPalette(color_palette);
            }
        } 
    }   
        
        std::map<std::string ,geo::Coordinates> uniq_stop_in_all_routes;
        std::vector<geo::Coordinates> geo_coords;        
        for(const auto& stop:stops_all){
            if (catalogue.GetBusThroughStop(std::string(stop)) != nullptr){
                uniq_stop_in_all_routes[std::string(stop)]=catalogue.FindStop(stop)->position;;
                geo::Coordinates stop_cord = catalogue.FindStop(stop)->position;
                geo_coords.push_back(stop_cord);
            }
        }
        
        //Заполнения словаря автобус - маршрут,для передачи в MapRenderer MR() с последующей отрисовкой
        std::map<std::string,std::pair<const domain::Route*,bool>> routs_to_drow;/*Не хочу по указателю, а то можно ненароком изменить базу.Хотя все равно внутри Route указатель на остановку в базе....поэтому по указателю*/
        for (const auto& [bus,is_round]:bus_all){
            if (catalogue.FindRoute(bus)!=nullptr){
                routs_to_drow[bus]={catalogue.FindRoute(bus),is_round};
            }
        }                   
        
        MR.SetSphereProjector(geo_coords);        
        //теперь иаршруты с остановками routs_to_drow можно передавать в MapRenderer MR для отрисовки
        
        MR.DrowMap(routs_to_drow,uniq_stop_in_all_routes, output_svg);
        
    }
    
    
void FillingCatalog  (const json::Dict& requests , transport_directory::tr_cat::TransportCatalogue& catalogue , std::vector<std::string_view>& stops_all , std::vector<std::pair<std::string,bool>>& bus_all){
    using namespace std::literals;
    if (requests.at("base_requests"s).IsArray()){
               const json::Array& base_requests =requests.at("base_requests"s).AsArray();            
               for (const auto& node_br:base_requests){
                   const json::Dict& requests_br=node_br.AsMap();
                   if(requests_br.at("type"s).AsString()=="Stop"s ){
                       const std::string& stop_name=requests_br.at("name"s).AsString(); 
                       stops_all.push_back(stop_name);////для дальнейшего определения координат остановок, встречающихся на маршруте///
                       const double latitude=requests_br.at("latitude"s).AsDouble() ;  
                       const double longitude=requests_br.at("longitude"s).AsDouble() ;
                       geo::Coordinates coord{latitude,longitude};
                       catalogue.InsertStop(stop_name,coord); 
                   }
               } 
            
                for (const auto& node_br:base_requests){
                   const json::Dict& requests_br=node_br.AsMap();
                   if(requests_br.at("type"s).AsString()=="Stop"s ){                       
                        const std::string& stop_name_from=requests_br.at("name"s).AsString();
                        if (requests_br.at("road_distances"s).IsMap()){
                        const json::Dict& stop_geo_dist=requests_br.at("road_distances"s).AsMap();                        
                               for (const auto& [stop_name_to, node_dist]:stop_geo_dist){
                                   const auto dist_value=std::to_string(node_dist.AsInt());
                                   catalogue.InsertStopDist(stop_name_from,stop_name_to, dist_value);
                               }
                       }
                   }
               }
            
                for (const auto& node_br:base_requests){
                   const json::Dict& requests_br=node_br.AsMap();
                   if(requests_br.at("type"s).AsString()=="Bus"s ){                       
                       std::vector<std::string_view> stops_on_route;
                       const std::string& bus_name = requests_br.at("name"s).AsString();                       
                       const double is_roundtrip = requests_br.at("is_roundtrip"s).AsBool();
                       bus_all.push_back({bus_name,is_roundtrip});////для дальнейшего определения маршрута для отрисовки///
                       const json::Array& stops=requests_br.at("stops"s).AsArray();
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
    
    void GetStatInfo(const json::Dict& requests ,transport_directory::tr_cat::TransportCatalogue& catalogue, std::stringstream& svg_file ,std::ostream& output){
        using namespace std::literals;
        if (requests.at("stat_requests"s).IsArray()){
            output<<'[';
            const json::Array& stat_requests =requests.at("stat_requests"s).AsArray();
            bool is_first =true;
            for (const auto& node_sr:stat_requests){
                   const json::Dict& requests_sr=node_sr.AsMap(); 
                   if(requests_sr.at("type"s).AsString()=="Stop"s ){
                       const std::string& stop_name=requests_sr.at("name"s).AsString();
                       const int request_id=requests_sr.at("id"s).AsInt();
                       if (catalogue.FindStop(stop_name)!=nullptr){ 
                       const std::set<std::string_view>* bus_thr_stop = catalogue.GetBusThroughStop(stop_name);  
                           if(!is_first){
                                output<<',';                                
                            }
                            output<<"\n{"<<"\n\"buses\": ["<<'\n';
                            bool is_first_=true;
                            if (bus_thr_stop!=nullptr){
                                   for(const auto& bus:*bus_thr_stop){
                                       if (!is_first_) output<<", ";
                                       output<<'\"'<<std::string{bus}<<'\"';
                                       is_first_=false;
                                   }
                            }  
                           is_first=false;
                           output<<'\n'<<"],"<<"\n\"request_id\": "<<request_id<<"\n}";
                            } 
                       
                       else {
                           if(!is_first){
                                output<<',';                                
                            }
                          is_first=false;
                          output<<"\n{"<< "\n\"request_id\": "<<request_id<<','
                            <<"\n\"error_message\": "<<"\"not found\""<<"\n}";
                       }
                       
                   }            
                                 
                   if(requests_sr.at("type"s).AsString()=="Bus"s ){
                         const std::string& bus_name=requests_sr.at("name"s).AsString();
                         const int request_id=requests_sr.at("id"s).AsInt();
                          if (catalogue.FindRoute(bus_name)!=nullptr){                            
                             const auto route_inf=catalogue.GetRoutInform(bus_name);
                             if(!is_first){
                                output<<',';                                
                            }
                              is_first=false;
                             output<<"\n{"<<"\n\"curvature\": "<<route_inf.curvature<<','
                               <<"\n\"request_id\": "<<request_id<<','
                               <<"\n\"route_length\": "<<route_inf.route_road_lenght<<','
                               <<"\n\"stop_count\": "<<route_inf.stop_number<<','
                               <<"\n\"unique_stop_count\": "<<route_inf.unique_stop_number
                               <<"\n}";
                          }
                          else { 
                            if(!is_first){
                                output<<',';                               
                            }
                           is_first=false;
                           output<<"\n{"<< "\n\"request_id\": "<<request_id<<','
                            <<"\n\"error_message\": "<<"\"not found\""<<"\n}";
                          }                       
                     }
                    //is_first =true;
                    if(requests_sr.at("type"s).AsString()=="Map"s ){
                          const int request_id=requests_sr.at("id"s).AsInt();
                              if(!is_first){
                                output<<',';                                
                              }
                             is_first=false;
                             output<<"\n{"<<"\n\"map\": "<<svg_file.str()<<", "
                                 <<"\n\"request_id\": "<<request_id
                               <<"\n}";                                           
                     }                
                }           
        }
        output<<"\n]";
    }
    
//////////////////////////////////////////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!    
json::Document query_processor(std::istream& input,transport_directory::tr_cat::TransportCatalogue& catalogue){
    using namespace std::literals;  
    //Создание json документа
    json::Document doc=json_reader::read_json_file(input);
    std::stringstream output_json; 
    std::stringstream output_svg;
    json::Node node_root=doc.GetRoot();
    if(node_root.IsMap()){
        const json::Dict& requests = node_root.AsMap(); 
        std::vector<std::string_view> stops_all;//////////////
        std::vector<std::pair<std::string,bool>> bus_all;///////////////
        
        ////////////////Заполнение данными json окумента транспортного справочника/////////////////////////////
        
        FillingCatalog  (requests ,catalogue ,stops_all ,bus_all);      
        
        ////////////////////Чтение параметров вывода и заполнение переменнных map render/////////////////////
        
        SetAndRenderMap(requests , catalogue , stops_all , bus_all , output_svg ); 
        
       ////////////////////////////////////Чтение запросов на вывод/////////////////////////////////////////// 
        json::Node node_doc(output_svg.str());
        output_svg.str(std::string());
        PrintNode(node_doc,output_svg); 
        
        GetStatInfo(requests ,catalogue, output_svg ,output_json);
        
      //////////////////////////////////////////////////////////////       
        
        return json::Load(output_json);      
    }
   else{
       output_json<<"null"<<std::endl;
       return json::Load(output_json);
    }
};
}///закрытие пространства имен