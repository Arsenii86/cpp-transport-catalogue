#include <iostream>
#include <sstream>
#include <string>
//#include "json_reader.h"
#include "transport_catalogue.h"
#include "request_handler.h"


int main() {   
    transport_directory::tr_cat::TransportCatalogue catalogue;    
    json::Node stat_info = req_hand::QueryProcessor(std::cin,catalogue);
    json_reader::OutJsonFile(stat_info, std::cout);
}