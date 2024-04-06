#pragma once

#include <iostream>
#include <map>
#include <utility>
#include <string>
#include <vector>
#include <variant>
#include <algorithm>
#include <sstream>
#include <iterator>
namespace json {   

class Node; 
// Сохраните объявления Dict и Array без изменения
    
using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;
using Number = std::variant<int, double>;  
    
    
// Эта ошибка должна выбрасываться при ошибках парсинга JSON
class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};
;
    
class Node:public std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string> {
public:   
    using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;
    
    Node() = default;    
    Node(const Value& value): value_(value) {};
    
    
    const Value& GetValue() const;    
    
    bool IsInt() const;
    bool IsDouble() const;
    bool IsPureDouble() const;
    bool IsBool() const;
    bool IsString() const;
    bool IsNull() const;
    bool IsArray() const;
    bool IsMap() const;
    
    int AsInt() const;
    bool AsBool() const;
    double AsDouble() const;
    const std::string& AsString() const;
    const Array& AsArray() const;
    const Dict& AsMap() const;    
    
private:
    Value value_;
};

 bool operator ==(const Node n1,const Node n2);
 bool operator !=(const Node n1,const Node n2);  
 

inline void PrintValue(const double value, std::ostream& out) {
    out << value;
}

inline void PrintValue(const int value, std::ostream& out) {
    out << value;
}


// Перегрузка функции PrintValue для вывода значений null
inline void PrintValue(std::nullptr_t, std::ostream& out) {
    out << "null";
}

inline void PrintValue(const std::string& word, std::ostream& out) {
   using namespace std::literals;
   
   out<<"\"";
   for (auto& letter:word){
        if (letter == '\\')          out<<"\\\\"s; 
        else if (letter == '\n')     out<<"\\n"s;
        else if (letter == '\t')     out<<"\\t"s;   
        else if (letter == '\r')     out<<"\\r"s;         
        else if (letter == '"')      out<<"\\\""s;        
        else out<<letter;
   }
   out<<"\"";   
}

inline void PrintValue(const bool is, std::ostream& out) {
    if (is==true) out << "true";
    else out << "false";
}

void PrintNode(const Node& node, std::ostream& out);

inline void PrintValue(const Array& nodes, std::ostream& out) {
    bool is_first=true;
    out<<"[\n";
    for (const auto& node:nodes ){ 
      if (!is_first) out<<",\n"; 
      PrintNode( node, out); 
      is_first=false; 
    }
    out<<"\n]";
}

inline void PrintValue(const Dict& dicts, std::ostream& out) {
    bool is_first=true;
    out<<"{\n";
    for (const auto& [key,node]:dicts ){ 
      if (!is_first) out<<",\n";
      out<<'\"'<<key<<'\"'<<": "; 
      PrintNode( node, out);
      is_first=false; 
    }
    out<<"\n}";
}

inline void PrintNode(const Node& node, std::ostream& out) {
    std::visit(
        [&out](const auto& value){ PrintValue(value, out); },
        node.GetValue());
}     
    
class Document {
public:
    explicit Document(Node root);

    const Node& GetRoot() const;

private:
    Node root_;
};

bool operator ==(const Document n1,const Document n2);       
    
Document Load(std::istream& input);

void Print(const Document& doc, std::ostream& output);    
    
    
}  // namespace json