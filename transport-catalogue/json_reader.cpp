#include "json_reader.h"
namespace json_reader{
    json::Document read_json_file(std::istream& input){
        return    json::Load(input); 
    }  
   
    void out_json_file(const json::Document& doc, std::ostream& output){
        json::Print(doc,output);
    }
}