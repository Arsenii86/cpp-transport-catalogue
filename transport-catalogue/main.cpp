#include <iostream>
#include <sstream>
#include <string>
//#include "json_reader.h"
#include "transport_catalogue.h"
#include "request_handler.h"


int main() {   
    transport_directory::tr_cat::TransportCatalogue catalogue;    
    std::string req_inf = req_hand::QueryProcessor(std::cin,catalogue);
    json_reader::OutJsonFile(req_inf, std::cout);
}