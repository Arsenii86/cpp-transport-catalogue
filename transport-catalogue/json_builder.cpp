#include "json_builder.h"

namespace json {
    KeyContext Builder::Key(std::string key){
            //std::cerr<<key<<std::endl;
            if (nodes_stack_.size()>0){
                if (json::Node* iter=nodes_stack_.back();!(iter->IsDict())){
                    throw std::logic_error("Ключ задается за пределами словаря");
                } 
                else if (key_!=std::nullopt){
                    throw std::logic_error("Метод задания ключа вызывается сразу после вызова такого же метода ");
                } 
                else{
                    key_=key;
                    return *this;
                }
            }
            else throw std::logic_error("Ключ задается при завершенном объекте");
            
        }
        
    Builder& Builder::Value(json::Node::Value val){
            
             if(root_==std::nullopt&&nodes_stack_.empty()){
                root_=val;
                return *this;
             }
            else if(root_!=std::nullopt&&!nodes_stack_.empty()){
                json::Node* iter=nodes_stack_.back();
                if (iter->IsArray()){
                    json::Node node = val;
                    auto& array=(iter->AsArray_() );
                    array.push_back(std::move(node));                    
                    return *this;
                }
                else if (iter->IsDict()&&(key_!=std::nullopt)){
                    json::Node node = val;
                    auto& dict=(iter->AsDict_() );
                    dict[*key_]=std::move(node);
                    key_.reset();
                    return *this;
                }
                else throw std::logic_error("Значение вставляется в словаре без иницилизированного ключа");
            } 
            else throw std::logic_error("Значение вставляется при готовомобъекте");
        }
        
    DictItemContext Builder::StartDict(){
            if(root_==std::nullopt&&nodes_stack_.empty()){
                root_=json::Dict{};
                nodes_stack_.push_back(&(*root_));
                return *this;
                }
            else if(root_!=std::nullopt&&!nodes_stack_.empty()){
                json::Node* iter=nodes_stack_.back();
                if (iter->IsArray()){
                    json::Node node=json::Dict{};
                    auto& array=(iter->AsArray_() );
                    array.push_back(std::move(node));
                    nodes_stack_.emplace_back(&array.back());
                    return *this;
                }
                else if (iter->IsDict()&&(key_!=std::nullopt)){
                    json::Node node=json::Dict{};
                    auto& dict=(iter->AsDict_() );
                    dict[*key_]=std::move(node);
                    nodes_stack_.emplace_back(&dict[*key_]);                   
                    return *this;
                }
                else throw std::logic_error("Словарь вставляется в словарь без иницилизированного ключа");
            }
            else throw std::logic_error("Словарь вставляется при готовом объекте");
                
        }
        
        Builder& Builder::EndDict(){
            if (nodes_stack_.size()>0){
                json::Node* iter=nodes_stack_.back();
                if (iter->IsDict()) {
                    nodes_stack_.erase(nodes_stack_.end()-1);
                     key_.reset();
                    return *this;
                }
                else throw std::logic_error("Пытаемся завершить не словарь");
            }
            else throw std::logic_error("Пытаемся завершить словарь, который не начат");
        }
        
        ArrayItemContext Builder::StartArray(){   
            if(root_==std::nullopt&&nodes_stack_.empty()){
                root_=json::Array{};
                nodes_stack_.emplace_back(&(*root_));
                return *this;
                }
            else if(root_!=std::nullopt&&!nodes_stack_.empty()){
                json::Node* iter=nodes_stack_.back();
                if (iter->IsArray()){
                    json::Node node=json::Array{};
                    auto& array=(iter->AsArray_() );
                    array.emplace_back(std::move(node));
                    nodes_stack_.emplace_back(&array.back());
                    return *this;
                }
                else if (iter->IsDict()&&(key_!=std::nullopt)){
                    json::Node node=json::Array{};
                    auto& dict=(iter->AsDict_() );
                    dict[*key_]=std::move(node);
                    nodes_stack_.emplace_back(&dict[*key_]);                    
                    return *this;
                }
                else throw std::logic_error("Массив вставляется в словаре без иницилизированного ключа");
            }
            else throw std::logic_error("Массив вставляется вставляется при готовом объекте");
        }
        
        Builder& Builder::EndArray(){
            if (nodes_stack_.size()>0){
                json::Node* iter=nodes_stack_.back();
                if (iter->IsArray()) {
                    nodes_stack_.erase(nodes_stack_.end()-1);
                     key_.reset();
                    return *this;
                }
                else throw std::logic_error("Пытаемся завершить не массив");
            }
            else throw std::logic_error("Пытаемся завершить массив, который не начат");
        }
        
       json::Node Builder::Build(){
           if (nodes_stack_.empty()&&root_!=std::nullopt) return *root_;
            else  throw std::logic_error("Вызов метода Build при неготовом описываемом объекте, то есть сразу после конструктора или при незаконченных массивах и словарях");
        }
    /////////////////DictItemContext///////////////////
    DictItemContext::DictItemContext(Builder& builder):builder_(builder){};
    
   KeyContext DictItemContext::Key(std::string key){
           //std::cerr<<"ok"<<std::endl;          
           return builder_.Key(key);
           
       };
       Builder& DictItemContext::EndDict(){
          return builder_.EndDict();
            
       };     
    
   ///////////////////KeyContext//////////////////////
    KeyContext::KeyContext(Builder& builder):builder_(builder){};
        
       ValueContextDict KeyContext::Value (json::Node::Value val,[[maybe_unused]]bool after_key){
           return builder_.Value(val);
       }
       
        ArrayItemContext KeyContext::StartArray(){
            return builder_.StartArray();
        };
       
       DictItemContext KeyContext::StartDict(){
            return builder_.StartDict();
       }
    
    ///////////////////ValueContext//////////////////////
    ValueContextDict::ValueContextDict(Builder& builder):builder_(builder){};
    Builder& ValueContextDict::EndDict(){
        return builder_.EndDict();
    }
     KeyContext ValueContextDict::Key(std::string key){
        return builder_.Key(key);        
     }
    
   ///////////////////ArrayItemContext////////////////////// 
    ArrayItemContext::ArrayItemContext(Builder& builder):builder_(builder){};
    
    ValueContextArr ArrayItemContext::Value(json::Node::Value val,[[maybe_unused]] bool in_array){
        return builder_.Value(val);
    };
    
    DictItemContext ArrayItemContext::StartDict(){
        return builder_.StartDict();
    };
    
    ArrayItemContext ArrayItemContext::StartArray(){
        return builder_.StartArray();
    };
    
    Builder& ArrayItemContext::EndArray(){
        return builder_.EndArray();
    };
    
    //////////////////////////ValueContextArr
    ValueContextArr::ValueContextArr(Builder& builder):builder_(builder){};
    
    ValueContextArr ValueContextArr::Value(json::Node::Value val,[[maybe_unused]] bool in_array){
        return builder_.Value(val);
    };
    
    DictItemContext ValueContextArr::StartDict(){
        return builder_.StartDict();
    };
    
    ArrayItemContext ValueContextArr::StartArray(){
        return builder_.StartArray();
    };
    
    Builder& ValueContextArr::EndArray(){
        return builder_.EndArray();
    };
    
    
    
    
    
}