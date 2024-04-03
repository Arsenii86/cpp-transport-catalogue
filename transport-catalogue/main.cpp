#include <iostream>
#include <sstream>
#include <string>
#include "json_reader.h"
#include "transport_catalogue.h"
#include "request_handler.h"


int main() {   
    transport_directory::tr_cat::TransportCatalogue catalogue;    
    json::Document req_inf= req_hand::query_processor(std::cin,catalogue);
    json_reader::out_json_file(req_inf, std::cout);
}