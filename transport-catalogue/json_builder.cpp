#include "json_builder.h"

namespace json {
    Builder::KeyContext Builder::Key(std::string key){            
            if (nodes_stack_.size()>0){
                if (json::Node* iter=nodes_stack_.back();!(iter->IsDict())){
                    throw std::logic_error("Ключ задается за пределами словаря");
                } 
                else if (key_!=std::nullopt){
                    throw std::logic_error("Метод задания ключа вызывается сразу после вызова такого же метода ");
                } 
                else{
                    key_=key;
                    return ShadowBuilder{*this};
                }
            }
            else throw std::logic_error("Ключ задается при завершенном объекте");
            
        }
   
    Builder::ShadowBuilder Builder::Value(json::Node::Value val){
            
             if(root_==std::nullopt && nodes_stack_.empty()){
                root_=val;
                return ShadowBuilder{*this};
             }
            else if(root_!=std::nullopt && !nodes_stack_.empty()){
                json::Node* iter=nodes_stack_.back();
                if (iter->IsArray()){
                    json::Node node = val;
                    auto& array=(iter->AsArray_() );
                    array.push_back(std::move(node));                    
                    return ShadowBuilder{*this};
                }
                else if (iter->IsDict()&&(key_!=std::nullopt)){
                    json::Node node = val;
                    auto& dict=(iter->AsDict_() );
                    dict[*key_]=std::move(node);
                    key_.reset();
                    return ShadowBuilder{*this};
                }
                                
                else {                    
                    throw std::logic_error("Значение вставляется в словаре без иницилизированного ключа");
                }
            } 
            else {               
                throw std::logic_error("Значение вставляется при готовомобъекте");
            }
        }
       
    
    Builder::DictItemContext Builder::StartDict(){
            return ShadowBuilder{Builder::StartConteiner(json::Dict{})};
                
        }
        
    Builder::ShadowBuilder Builder::EndDict(){
            if (nodes_stack_.size()>0){
                json::Node* iter=nodes_stack_.back();
                if (iter->IsDict()) {
                    nodes_stack_.erase(nodes_stack_.end()-1);
                     key_.reset();
                    return ShadowBuilder{*this};
                }
                else throw std::logic_error("Пытаемся завершить не словарь");
            }
            else throw std::logic_error("Пытаемся завершить словарь, который не начат");
        }
        
    Builder::ArrayItemContext Builder::StartArray(){   
           return ShadowBuilder{Builder::StartConteiner(json::Array{})};
        }
        
    Builder::ShadowBuilder Builder::EndArray(){
            if (nodes_stack_.size()>0){
                json::Node* iter=nodes_stack_.back();
                if (iter->IsArray()) {
                    nodes_stack_.erase(nodes_stack_.end()-1);
                     key_.reset();
                    return ShadowBuilder{*this};
                }
                else throw std::logic_error("Пытаемся завершить не массив");
            }
            else throw std::logic_error("Пытаемся завершить массив, который не начат");
        }
        
       json::Node Builder::Build(){
           if (nodes_stack_.empty()&&root_!=std::nullopt) return *root_;
            else  throw std::logic_error("Вызов метода Build при неготовом описываемом объекте, то есть сразу после конструктора или при незаконченных массивах и словарях");
        } 
    
}