#include "json.h"

using namespace std;

namespace json {

namespace {

std::string LoadString(std::istream& input) {
    using namespace std::literals;    
    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    std::string s;
    while (true) {
        if (it == end) {
            // Поток закончился до того, как встретили закрывающую кавычку?
            throw ParsingError("String parsing error");
        }
        const char ch = *it;
        if (ch == '"') {
            // Встретили закрывающую кавычку
            ++it;
            break;
        } else if (ch == '\\') {
            // Встретили начало escape-последовательности
            ++it;
            if (it == end) {
                // Поток завершился сразу после символа обратной косой черты
                throw ParsingError("String parsing error");
            }
            const char escaped_char = *(it);
            // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
            switch (escaped_char) {
                case 'n':
                    s.push_back('\n');
                    break;
                case 't':
                    s.push_back('\t');
                    break;
                case 'r':
                    s.push_back('\r');
                    break;
                case '"':
                    s.push_back('"');
                    break;
                case '\\':
                    s.push_back('\\');
                    break;
                default:
                    // Встретили неизвестную escape-последовательность
                    throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
            }
        } else if (ch == '\n' || ch == '\r') {
            // Строковый литерал внутри- JSON не может прерываться символами \r или \n
            throw ParsingError("Unexpected end of line"s);
        } else {
            // Просто считываем очередной символ и помещаем его в результирующую строку
            s.push_back(ch);
        }
        ++it;
    }     
    return s;
}    
    
Number LoadNumber(std::istream& input) {
    using namespace std::literals;
    std::string parsed_num;
    // Считывает в parsed_num очередной символ из input
    auto read_char = [&parsed_num, &input] {
        parsed_num += static_cast<char>(input.get());
        if (!input) {
            throw ParsingError("Failed to read number from stream"s);
        }
    };

    // Считывает одну или более цифр в parsed_num из input
    auto read_digits = [&input, read_char] {
        if (!std::isdigit(input.peek())) {
            throw ParsingError("A digit is expected"s);
        }
        while (std::isdigit(input.peek())) {
            read_char();
        }
    };

    if (input.peek() == '-') {
        read_char();
    }
    // Парсим целую часть числа
    if (input.peek() == '0') {
        read_char();
        // После 0 в JSON не могут идти другие цифры
    } else {
        read_digits();
    }

    bool is_int = true;
    // Парсим дробную часть числа
    if (input.peek() == '.') {
        read_char();
        read_digits();
        is_int = false;
    }

    // Парсим экспоненциальную часть числа
    if (int ch = input.peek(); ch == 'e' || ch == 'E') {
        read_char();
        if (ch = input.peek(); ch == '+' || ch == '-') {
            read_char();
        }
        read_digits();
        is_int = false;
    }

    try {
        if (is_int) {
            // Сначала пробуем преобразовать строку в int
            try {
                return std::stoi(parsed_num);
            } catch (...) {
                // В случае неудачи, например, при переполнении,
                // код ниже попробует преобразовать строку в double
            }
        }
        return std::stod(parsed_num);
    } catch (...) {
        throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
    }
}
    
Node LoadNode(istream& input);

Node LoadArray(istream& input) {
    Array result;
    for (char c; input >> c && c != ']';) {
        if (c != ',') {
            input.putback(c);
        }
        result.push_back(LoadNode(input));
    }
    return Node(move(result));
}

Node LoadDict(istream& input) {
    Dict result;
    for (char c; input >> c && c != '}';) {
        if (c == ',') {
            input >> c;
        }
        string key = LoadString(input);
        input >> c;
        result.insert({move(key), LoadNode(input)});
    }

    return Node(move(result));
}

Node LoadNode(istream& input) {
    char c;
    input >> c;
    if(c==' '||c=='\r'||c=='\n'||c=='\t' ){
        while(c==' '||c=='\r'||c=='\n'||c=='\t' ) input >> c;
    }
    
    
    if (c == '[') {
            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            if (it == end) {
                throw ParsingError("Array parsing error");
            }
            else return LoadArray(input);
            
    }else if (c == ']')  {
        throw ParsingError("Array parsing error");
        
    }else if (c == '{') {
        auto it = std::istreambuf_iterator<char>(input);
        auto end = std::istreambuf_iterator<char>();
        if (it == end) {
            throw ParsingError("Dict parsing error");
            }
        else return LoadDict(input);
        
    }else if (c == '}')  {
        throw ParsingError("Dict parsing error");
        
    } else if (c == '"') {
        std::string line=LoadString(input);
        return  Node(std::move(line));
    }
    else if (c == 'n') {
        input.putback(c);
        std::string line;
        for (char c; input >> c ;) {
            if (c == ','||c == ' '||c == '}'||c == ']'){
                input.putback(c);
                break;
                };
        line += c;
        }        
        //std::cerr<<line<< std::endl;
        if (line=="null") return  Node(nullptr);
        else throw ParsingError("Null parsing error");
        
    } else if (c == 't'||c == 'f') {               
        input.putback(c);
        std::string line;
        for (char c; input >> c ;) {
            if (c == ','||c == ' '||c == '}'||c == ']'){
                input.putback(c);
                break;
                };
        line += c;
        }     
        //std::cerr<<line<< std::endl;
        if (line=="true") return Node(true);
        else if(line=="false") return  Node(false);     
        else throw ParsingError("Bool parsing error");
    } 
    else {
        input.putback(c);
        const auto number=LoadNumber(input);
        if(holds_alternative<int>(number)) return Node(get<int>(number));
        else return Node(get<double>(number));
        
    }
}

}  // namespace    
    
const Value& Node::GetValue() const { return value_; }   

    bool Node::IsInt() const{
      if (holds_alternative<int>(value_)) return true;
        else return false;
    };
    
    bool Node::IsDouble() const{
        if (holds_alternative<int>(value_)||holds_alternative<double>(value_)) return true;
        
        else return false;
    };
    
    bool Node::IsPureDouble() const{
        if (!holds_alternative<int>(value_)&&holds_alternative<double>(value_)) return true;
        else return false;
    };
    
    bool Node::IsBool() const{
        if (holds_alternative<bool>(value_)) return true;
        else return false;
    };
    
    bool  Node::IsString() const{
        if (holds_alternative<std::string>(value_)) return true;
        else return false;
    };
    
    bool Node::IsNull() const{
        if (holds_alternative<std::nullptr_t>(value_)) return true;
        else return false;        
    };
    
    bool Node::IsArray() const{
        if (holds_alternative<Array>(value_)) return true;
        else return false;   
    };
    
    bool Node::IsMap() const{
        if (holds_alternative<Dict>(value_)) return true;
        else return false;
    };
    
    int Node::AsInt() const{
        if(IsInt()) return get<int>(value_);
        else throw std::logic_error{" "};
    };
    
    bool Node::AsBool() const{
        if(IsBool()) return get<bool>(value_);
        else throw std::logic_error{" "};
    };
    
    double Node::AsDouble() const{
       if(IsPureDouble()) return get<double>(value_);
       if(IsInt()) return static_cast<double>(get<int>(value_));
       throw std::logic_error{" "};        
    };
    
    const std::string& Node::AsString() const{
       if(IsString()) return get<std::string>(value_);
        else throw std::logic_error{" "}; 
    };
    
    const Array& Node::AsArray() const{
        if(IsArray()) return get<Array>(value_);
        else throw std::logic_error{" "};
    };
    
    const Dict& Node::AsMap() const{
        if(IsMap()) return get<Dict>(value_);
        else throw std::logic_error{" "};
    };
    
bool operator ==(const Node n1,const Node n2){
  if (n1.IsInt()&&n2.IsInt())               return n1.AsInt()==n2.AsInt();
  if (n1.IsPureDouble()&&n2.IsPureDouble()) return n1.AsDouble()==n2.AsDouble();  
  if (n1.IsBool()&&n2.IsBool())             return n1.AsBool()==n2.AsBool();
  if (n1.IsString()&&n2.IsString())         return n1.AsString()==n2.AsString();
  if (n1.IsArray()&&n2.IsArray())           return n1.AsArray()==n2.AsArray();   
  if (n1.IsMap()&&n2.IsMap())               return n1.AsMap()==n2.AsMap();
  if (n1.IsNull()&&n2.IsNull())             return true;  
  return false;
};
    
bool operator !=(const Node n1,const Node n2){
    return !(n1==n2);
};   
    
bool operator ==(const Document n1,const Document n2){
   return n1.GetRoot()==n2.GetRoot();
};      
    
    
Document::Document(Node root)
    : root_(move(root)) {
}

const Node& Document::GetRoot() const {
    return root_;
}

Document Load(istream& input) {
    return Document{LoadNode(input)};
}

void Print(const Document& doc, std::ostream& output) {
    PrintNode(doc.GetRoot(),output);   
}

}  // namespace json