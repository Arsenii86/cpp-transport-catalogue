#include "request_handler.h"
namespace req_hand{  
    
std::string QueryProcessor(std::istream& input,transport_directory::tr_cat::TransportCatalogue& catalogue){
    return json_reader::ReadAndProcessJsonFile(input,catalogue);
};
}///закрытие пространства имен