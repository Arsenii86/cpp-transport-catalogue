#include "json_reader.h"
namespace json_reader{
    json::Document read_json_file(std::istream& input){
        return    json::Load(input); 
    }  
   
    void out_json_file(const json::Document& doc, std::ostream& output){
        json::Print(doc,output);
    }
    
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
    
    
    
    
    
    
    
}