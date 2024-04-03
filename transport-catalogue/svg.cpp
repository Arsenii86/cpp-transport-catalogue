#include "svg.h"

namespace svg {

using namespace std::literals;
std::ostream& operator<<(std::ostream& stream, const StrokeLineJoin& slj)
{ switch(slj){
        case(StrokeLineJoin::ARCS):
            stream << "arcs";
            break;
        case(StrokeLineJoin::BEVEL):
            stream << "bevel";
            break;
        case(StrokeLineJoin::MITER):
            stream << "miter";
            break;
        case(StrokeLineJoin::MITER_CLIP):
            stream << "miter-clip";
            break;
        case(StrokeLineJoin::ROUND):        
            stream << "round";
            break;
        default:break;
    }   
    return stream;
}      

std::ostream& operator<<(std::ostream& stream, const StrokeLineCap& slc)    
{ switch(slc){
        case(StrokeLineCap::BUTT):
            stream << "butt";
            break;
        case(StrokeLineCap::ROUND):
            stream << "round";
            break;
        case(StrokeLineCap::SQUARE):
            stream << "square";
            break;
        default:break;
    }   
    return stream;
}      
    
    RenderContext RenderContext::Indented() const {
        return {out, indent_step, indent + indent_step};
    }
    
    void RenderContext::RenderIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }
    
    
    
void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << std::endl;
}

// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const{
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\" "sv;
    RenderAttrs(out);
    out << "/>"sv;
};
    
//-----------------Polyline---------------
    Polyline& Polyline::AddPoint(Point point){
        points_.push_back(point);
        return *this;
    };  
    
    void Polyline::RenderObject(const RenderContext& context) const {       
        auto& out = context.out;
        out << "<polyline points=\""sv;
        bool first = true;
        for (const auto point:points_){
            if (!first) out <<' ';
            out <<point.x<<','<<point.y;
            first=false;
        }        
        out << "\" "sv;
        RenderAttrs(out);
        out <<"/>"sv;
    };
    
    
  //-----------------Text--------------- 
    Text& Text::SetPosition(Point pos){
        pos_=pos;
        return *this;
    };

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& Text::SetOffset(Point offset){
        offset_=offset;
        return *this;
    };

    // Задаёт размеры шрифта (атрибут font-size)
    Text& Text::SetFontSize(uint32_t size){
        size_=size;
        return *this;
    };
    

    // Задаёт название шрифта (атрибут font-family)
    Text& Text::SetFontFamily(std::string font_family){
        font_family_=font_family;
        return *this;
    };

    // Задаёт толщину шрифта (атрибут font-weight)
    Text& Text::SetFontWeight(std::string font_weight){
        font_weight_=font_weight;
        return *this;
    };

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& Text::SetData(std::string data){
        data_=data;
        return *this;
    };
    
    void Text::RenderObject(const RenderContext& context) const {    
    auto& out = context.out;
    out <<"<text ";
    RenderAttrs(out);    
     out <<" x=\""sv<<pos_.x<<"\" y=\""sv<<pos_.y<<"\" dx=\""sv<<offset_.x<<"\" dy=\""sv<<offset_.y<<"\" "sv<< "font-size=\""sv<<size_<<"\"";
   if(!font_family_.empty())
   out <<" font-family=\""sv<<font_family_<<"\"";
   if(!font_weight_.empty())
   out <<" font-weight=\""sv<<font_weight_<<"\"";
   out <<">"sv<<data_<<"</text>"sv;   
    };
    
   //-----------------Document---------------  
    void Document::AddPtr(std::unique_ptr<Object>&& obj){
        objects_.emplace_back(std::move(obj));
    }
    
    void Document::Render(std::ostream& out) const{
        out<<"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv<<std::endl;
        out<<"<svg xmlns=\""sv<<"http://www.w3.org/2000/svg"sv<<"\" version=\"1.1\">"sv<<std::endl;
        for (const auto &object:objects_){
            out<<"  ";
            object->Render(out);
        }
        out<<"</svg>"sv;
    }; 
}  // namespace svg

namespace shapes {
    //----------------------Triangle--------------------   
    
    void Triangle::Draw(svg::ObjectContainer& container) const  {
        container.Add(svg::Polyline().AddPoint(p1_).AddPoint(p2_).AddPoint(p3_).AddPoint(p1_));
    }
    
  //----------------------Star------------------------ 
    svg::Polyline Star::CreateStar(svg::Point center, double outer_rad, double inner_rad, int num_rays) {
        svg::Polyline polyline;
        for (int i = 0; i <= num_rays; ++i) {
            double angle = 2 * M_PI * (i % num_rays) / num_rays;
            polyline.AddPoint({center.x + outer_rad * sin(angle), center.y - outer_rad * cos(angle)});
            if (i == num_rays) {
                break;
            }
            angle += M_PI / num_rays;
            polyline.AddPoint({center.x + inner_rad * sin(angle), center.y - inner_rad * cos(angle)});
            }
        return polyline;
    }
    
    Star::Star(svg::Point center, double outer_rad, double inner_rad, int num_rays){
        star_=CreateStar(center,outer_rad,inner_rad,num_rays);
        star_.SetFillColor("red");
        star_.SetStrokeColor("black");
    }

    void Star::Draw(svg::ObjectContainer& container) const {
        container.Add(star_);
    }  
   //----------------------Snowman------------------------ 
   Snowman::Snowman(svg::Point center,double radius){        
        head_.SetCenter(center); 
        head_.SetRadius(radius);        
        body_.SetCenter({center.x,center.y+radius*2}); 
        body_.SetRadius(radius*1.5);
        foot_.SetCenter({center.x,center.y+radius*5}); 
        foot_.SetRadius(radius*2);
        head_.SetFillColor("rgb(240,240,240)");
        head_.SetStrokeColor("black");
        body_.SetFillColor("rgb(240,240,240)");
        body_.SetStrokeColor("black");
        foot_.SetFillColor("rgb(240,240,240)");
        foot_.SetStrokeColor("black");
    }
    void Snowman::Draw(svg::ObjectContainer& container) const { 
        container.Add(foot_);
        container.Add(body_);
        container.Add(head_); 
    }      
       
       
       
}    

