#pragma once
#include "json.h"
namespace json_reader{
     json::Document read_json_file(std::istream& input); 
   
     void out_json_file(const json::Document& doc, std::ostream& output);
}
    

