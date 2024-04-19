#include "map_renderer.h"
namespace mp_rend{
    
    void RenderMap(mp_rend::MapRenderer& MR, transport_directory::tr_cat::TransportCatalogue& catalogue, const std::vector<std::string_view>& stops_all ,const std::vector<std::pair<std::string,bool>>& bus_all , std::string& output_svg ){
        
        StopsInfo uniq_stop_in_all_routes;
        std::vector<geo::Coordinates> geo_coords;        
        for(const auto& stop:stops_all){
            if (catalogue.GetBusThroughStop(std::string(stop)) != nullptr){
                uniq_stop_in_all_routes[std::string(stop)]=catalogue.FindStop(stop)->position;;
                geo::Coordinates stop_cord = catalogue.FindStop(stop)->position;
                geo_coords.push_back(stop_cord);
            }
        }
        
        //Заполнения словаря автобус - маршрут,для передачи в MapRenderer MR() с последующей отрисовкой
        RoutesInfo routs_to_drow;/*Не хочу по указателю, а то можно ненароком изменить базу.Хотя все равно внутри Route указатель на остановку в базе....поэтому по указателю*/
        for (const auto& [bus,is_round]:bus_all){
            if (catalogue.FindRoute(bus)!=nullptr){
                routs_to_drow[bus]={catalogue.FindRoute(bus),is_round};
            }
        }                   
        
        MR.SetSphereProjector(geo_coords);        
        //теперь иаршруты с остановками routs_to_drow можно передавать в MapRenderer MR для отрисовки
        
        MR.DrawMap(routs_to_drow,uniq_stop_in_all_routes, output_svg);
        
    }
    
     void MapRenderer::SetWidth(double width){
           width_=width;          
       } 
        void MapRenderer::SetHeight(double height){
           height_=height;          
       }         
        void MapRenderer::SetPadding(double padding){
           padding_=padding;          
       } 
        void MapRenderer::SetLineWidth(double line_width){
           line_width_=line_width;          
       } 
       void MapRenderer::SetStopRadius(double stop_radius){
           stop_radius_=stop_radius;          
       }  
        void MapRenderer::SetBusLabelFontSize(int bus_label_font_size){
           bus_label_font_size_=bus_label_font_size;          
       } 
        
        void MapRenderer::SetStopLabelFontSize(int stop_label_font_size){
           stop_label_font_size_=stop_label_font_size;          
       } 
       
        void MapRenderer::SetBusLabelOffset(svg::Point& bus_label_offset){
           bus_label_offset_=std::move(bus_label_offset);          
       }
        
        void MapRenderer::SetStopLabelOffset(svg::Point& stop_label_offset){
           stop_label_offset_=std::move(stop_label_offset);          
       }       
        void MapRenderer::SetUnderlayerColor(svg::Color&  underlayer_color){
           underlayer_color_=std::move(underlayer_color);          
       }        
       void MapRenderer::SetUnderlayerWidth(double underlayer_width){
           underlayer_width_=underlayer_width;          
       } 
       void MapRenderer::SetColorPalette(std::vector <svg::Color>& color_palette){
           color_palette_=std::move(color_palette);          
       } 
        
       void MapRenderer::SetSphereProjector(std::vector<geo::Coordinates> geo_coords){
           SphereProjector Projector(geo_coords.begin(),geo_coords.end(),width_,height_,padding_);
           SpPr=std::move(Projector);   
       } 
       
    void MapRenderer::DrawRouteLine(svg::Document& doc, RoutesInfo& routs_to_drow)const {
        int i=0;//т.е. первый в векторе возможных цветов маршрутов color_palette_
           for(const auto& [bus,bus_route] : routs_to_drow){ 
               if((bus_route.first->stops).size()>0){                   
                   const auto& stops_in_route = bus_route.first->stops;                   
                   svg::Polyline routes;                   
                   for (const auto& stop : stops_in_route){                   
                        svg::Point point = SpPr(stop->position);
                        routes.AddPoint(point); 
                   }
                   routes.SetFillColor(svg::NoneColor);
                   routes.SetStrokeWidth(line_width_);
                   routes.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
                   routes.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);               
                   routes.SetStrokeColor(color_palette_[i%color_palette_.size()]);                   
                   doc.Add(std::move(routes));                   
                   ++i;
               }
           }
    }
    
    void MapRenderer::DrawRouteName(svg::Document& doc, RoutesInfo& routs_to_drow)const {
        int i=0;//т.е. первый в векторе возможных цветов маршрутов color_palette_
           for(const auto& [bus,bus_route]:routs_to_drow){ 
               if((bus_route.first->stops).size()>0){ 
                   const auto& stops_in_route = bus_route.first->stops;
                   std::string bus_name = std::string(bus);
                   std::string first_stop_name = stops_in_route[0]->name;
                   int stops_number_in_route = static_cast<int>(stops_in_route.size());
                   svg::Point point_stop = SpPr(stops_in_route[0]->position);
                   svg::Text routes_name;
                   {
                       routes_name.SetPosition(point_stop);
                       routes_name.SetOffset(bus_label_offset_);
                       routes_name.SetFontSize(bus_label_font_size_);
                       routes_name.SetFontFamily("Verdana");
                       routes_name.SetFontWeight("bold");
                       routes_name.SetData(bus_name);
                       routes_name.SetFillColor(color_palette_[i%color_palette_.size()]);
                   }
                   svg::Text routes_name_back;
                   {
                       routes_name_back.SetPosition(point_stop);
                       routes_name_back.SetOffset(bus_label_offset_);
                       routes_name_back.SetFontSize(bus_label_font_size_);
                       routes_name_back.SetFontFamily("Verdana");
                       routes_name_back.SetFontWeight("bold");
                       routes_name_back.SetData(bus_name);
                       
                       routes_name_back.SetFillColor(underlayer_color_);
                       routes_name_back.SetStrokeColor(underlayer_color_);
                       routes_name_back.SetStrokeWidth(underlayer_width_);                       
                       routes_name_back.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
                       routes_name_back.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
                   }   
                   doc.Add(routes_name_back);
                   doc.Add(routes_name);
                   
                   if(!bus_route.second ){
                       if(stops_in_route[stops_number_in_route/2]->name != first_stop_name){
                       point_stop=SpPr(stops_in_route[stops_number_in_route/2]->position);
                       routes_name.SetPosition(point_stop);
                       routes_name_back.SetPosition(point_stop);
                       doc.Add(routes_name_back);
                       doc.Add(routes_name);  
                       }
                   }                    
                   ++i;   
               }
            }
    }
    
    void MapRenderer::DrawStopPoint(svg::Document& doc, StopsInfo& uniq_stop_in_all_routes)const {
        for(const auto& [stop_name, coord] : uniq_stop_in_all_routes){ 
                        svg::Circle stop_sign;                        
                        svg::Point point= SpPr(coord);
                        stop_sign.SetCenter(point);
                        stop_sign.SetRadius(stop_radius_);
                        stop_sign.SetFillColor("white");
                        doc.Add(std::move(stop_sign));                   
            }
    }
    
    void MapRenderer::DrawStopName(svg::Document& doc, StopsInfo& uniq_stop_in_all_routes)const {
        for(const auto& [stop, coord] : uniq_stop_in_all_routes){ 
                        svg::Point point= SpPr(coord);
                        svg::Text stop_name;                        
                   {
                       stop_name.SetPosition(point);
                       stop_name.SetOffset(stop_label_offset_);
                       stop_name.SetFontSize(stop_label_font_size_);
                       stop_name.SetFontFamily("Verdana");                       
                       stop_name.SetData(stop);
                       stop_name.SetFillColor("black");
                   }
                     svg::Text stop_name_back;
                   {
                       stop_name_back.SetPosition(point);
                       stop_name_back.SetOffset(stop_label_offset_);
                       stop_name_back.SetFontSize(stop_label_font_size_);
                       stop_name_back.SetFontFamily("Verdana");                       
                       stop_name_back.SetData(stop);
                       
                       stop_name_back.SetFillColor(underlayer_color_); 
                       stop_name_back.SetStrokeColor(underlayer_color_);
                       stop_name_back.SetStrokeWidth(underlayer_width_);
                       stop_name_back.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
                       stop_name_back.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
                   }   
                   doc.Add(stop_name_back);
                   doc.Add(stop_name);                                    
            }    
    }
    
       void MapRenderer::DrawMap(RoutesInfo& routs_to_drow, StopsInfo& uniq_stop_in_all_routes, std::string& out) const{
        svg::Document doc;
        std::stringstream out_tmp;
        //Отрисовка линии маршрута
        DrawRouteLine(doc, routs_to_drow);
        //Отрисовка названий маршрута
        DrawRouteName(doc, routs_to_drow);
        //Отрисовка точек остановок
        DrawStopPoint(doc, uniq_stop_in_all_routes);
        //отрисовка названий остановок
        DrawStopName(doc, uniq_stop_in_all_routes);
        doc.Render(out_tmp);  
        out=out_tmp.str();
       } 
    
    
    
    
    
}
/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршртутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */