#include "request_handler.h"
namespace req_hand{  
    
json::Document QueryProcessor(std::istream& input,transport_directory::tr_cat::TransportCatalogue& catalogue){
    using namespace std::literals;  
    //Создание json документа
    json::Document doc=json_reader::read_json_file(input);
    std::stringstream output_json; 
    std::stringstream output_svg;
    json::Node node_root = doc.GetRoot();
    if(node_root.IsMap()){
        const json::Dict& requests = node_root.AsMap(); 
        std::vector<std::string_view> stops_all;
        std::vector<std::pair<std::string,bool>> bus_all;        
        json_reader::FillingCatalog  (requests ,catalogue ,stops_all ,bus_all); 
        mp_rend::SetAndRenderMap(requests , catalogue , stops_all , bus_all , output_svg ); 
        json::Node node_doc(output_svg.str());
        output_svg.str(std::string());
        PrintNode(node_doc, output_svg);        
        json_reader::GetStatInfo(requests ,catalogue, output_svg ,output_json); 
        return json::Load(output_json);      
    }
   else{
       output_json << "null" << std::endl;
       return json::Load(output_json);
    }
};
}///закрытие пространства имен