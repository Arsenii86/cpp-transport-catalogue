#pragma once
#include "geo.h"
#include "svg.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <vector>
#include <map>
#include "domain.h"
namespace mp_rend{
    
inline const double EPSILON = 1e-6;
inline bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}

class SphereProjector {
public:
    SphereProjector(){};
    
    // points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
    template <typename PointInputIt>
    SphereProjector(PointInputIt points_begin, PointInputIt points_end,
                    double max_width, double max_height, double padding)
        : padding_(padding) //
    {
        // Если точки поверхности сферы не заданы, вычислять нечего
        if (points_begin == points_end) {
            return;
        }

        // Находим точки с минимальной и максимальной долготой
        const auto [left_it, right_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
        min_lon_ = left_it->lng;
        const double max_lon = right_it->lng;

        // Находим точки с минимальной и максимальной широтой
        const auto [bottom_it, top_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
        const double min_lat = bottom_it->lat;
        max_lat_ = top_it->lat;

        // Вычисляем коэффициент масштабирования вдоль координаты x
        std::optional<double> width_zoom;
        if (!IsZero(max_lon - min_lon_)) {
            width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
        }

        // Вычисляем коэффициент масштабирования вдоль координаты y
        std::optional<double> height_zoom;
        if (!IsZero(max_lat_ - min_lat)) {
            height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
        }

        if (width_zoom && height_zoom) {
            // Коэффициенты масштабирования по ширине и высоте ненулевые,
            // берём минимальный из них
            zoom_coeff_ = std::min(*width_zoom, *height_zoom);
        } else if (width_zoom) {
            // Коэффициент масштабирования по ширине ненулевой, используем его
            zoom_coeff_ = *width_zoom;
        } else if (height_zoom) {
            // Коэффициент масштабирования по высоте ненулевой, используем его
            zoom_coeff_ = *height_zoom;
        }
    }

    // Проецирует широту и долготу в координаты внутри SVG-изображения
    svg::Point operator()(geo::Coordinates coords) const {
        return {
            (coords.lng - min_lon_) * zoom_coeff_ + padding_,
            (max_lat_ - coords.lat) * zoom_coeff_ + padding_
        };
    }

private:
    double padding_;
    double min_lon_ = 0;
    double max_lat_ = 0;
    double zoom_coeff_ = 0;
};
    
    
    
    
class MapRenderer{
        double width_=1200.0;
        double height_=1200.0;
        double padding_=50.0;
        double line_width_=14.0;
        double stop_radius_=5.0;
        int bus_label_font_size_=20;
        int stop_label_font_size_=20;
        svg::Point bus_label_offset_ = {7.0, 15.0};        
        svg::Point stop_label_offset_ = {7.0, -3.0};
        svg::Color  underlayer_color_ = svg::Rgba{255, 255, 255, 0.85};
        double underlayer_width_ = 3.0;
        std::vector <svg::Color> color_palette_= {
                                                "green",
                                                 svg::Rgb{255, 160, 0},
                                                "red"
                                                }; 
        SphereProjector SpPr;        
       
    public:   
       MapRenderer()=default; 
              
       void SetWidth(double width);
    
        void SetHeight(double height);
    
        void SetPadding(double padding);
    
        void SetLineWidth(double line_width);
    
       void SetStopRadius(double stop_radius);
    
        void SetBusLabelFontSize(int bus_label_font_size);
        
        void SetStopLabelFontSize(int stop_label_font_size);
       
        void SetBusLabelOffset(svg::Point& bus_label_offset);
        
        void SetStopLabelOffset(svg::Point& stop_label_offset);
    
        void SetUnderlayerColor(svg::Color&  underlayer_color);
    
       void SetUnderlayerWidth(double underlayer_width);
    
       void SetColorPalette(std::vector <svg::Color>& color_palette);
        
       void SetSphereProjector(std::vector<geo::Coordinates> geo_coords); 
        
       void DrowMap(std::map<std::string,std::pair<const domain::Route*,bool>> routs_to_drow, std::map<std::string,geo::Coordinates> uniq_stop_in_all_routes, std::ostream& out) const;
   };
    
    
    
    
    
    
}//end namespace