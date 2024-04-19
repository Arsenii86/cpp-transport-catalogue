#pragma once
#include <string>
#include <variant>
#include <vector>
#include <iterator>
#include <optional>
#include <cassert>
#include "json.h"
namespace json { 
   
    
class Builder{
    private:
        std::optional<std::string> key_;
        std::optional<json::Node> root_;     
        std::vector<json::Node*> nodes_stack_;
    
       class ShadowBuilder;
       class KeyContext; 
       class DictItemContext;      
       class ArrayItemContext;
       
        
    public:   
        Builder(){}; 
        json::Node Build();  
        KeyContext Key(std::string key);        
        ShadowBuilder Value(json::Node::Value val);        
        DictItemContext StartDict(); 
        ArrayItemContext StartArray();
        ShadowBuilder EndDict();
        ShadowBuilder EndArray();   
        
    private:
    
    template <typename T>
        Builder& StartConteiner(T conteiner){
            if(root_==std::nullopt&&nodes_stack_.empty()){
                root_=conteiner;///////////////
                nodes_stack_.emplace_back(&(*root_));
                return *this;
                }
            else if(root_!=std::nullopt&&!nodes_stack_.empty()){
                json::Node* iter=nodes_stack_.back();
                if (iter->IsArray()){
                    json::Node node=conteiner;///////
                    auto& array=(iter->AsArray_() );
                    array.emplace_back(std::move(node));
                    nodes_stack_.emplace_back(&array.back());
                    return *this;
                }
                else if (iter->IsDict()&&(key_!=std::nullopt)){
                    json::Node node=conteiner;//////
                    auto& dict=(iter->AsDict_() );
                    dict[*key_]=std::move(node);
                    nodes_stack_.emplace_back(&dict[*key_]);                    
                    return *this;
                }
                else throw std::logic_error("Контейнер вставляется в словарь без иницилизированного ключа");
            }
            else throw std::logic_error("Контейнер вставляется при готовом объекте");

        }
    
        class ShadowBuilder {
        public:
            ShadowBuilder(Builder& builder):sh_build_(builder){};
            json::Node Build(){
                return sh_build_.Build();
            }
            KeyContext Key(std::string key) {
                return sh_build_.Key(key);
            }
            ShadowBuilder Value(Node::Value value) {
                return sh_build_.Value(value);
            }
            DictItemContext StartDict() {
                return sh_build_.StartDict();
            }
            ArrayItemContext StartArray() {
                return sh_build_.StartArray();
            }
            ShadowBuilder EndDict() {
                return sh_build_.EndDict();
            }
            ShadowBuilder EndArray() {
                return sh_build_.EndArray();
            }
              
        private:
            Builder& sh_build_;
        };
    
        class KeyContext:public ShadowBuilder{           
       public:
            
            KeyContext(ShadowBuilder builder):ShadowBuilder(builder){}; 
            DictItemContext Value(json::Node::Value val){ return ShadowBuilder::Value(val); }        
            KeyContext Key(std::string key)=delete; 
            ShadowBuilder EndDict()=delete;
            ShadowBuilder EndArray()=delete;        
            json::Node Build()=delete;
            
        }; 
    
        class DictItemContext:public ShadowBuilder{           
        public:
           DictItemContext(ShadowBuilder builder):ShadowBuilder(builder){};           
           ShadowBuilder Value(json::Node::Value val)=delete;        
           DictItemContext StartDict()=delete; 
           ArrayItemContext StartArray()=delete;        
           ShadowBuilder EndArray()=delete;        
           json::Node Build()=delete;
        
   };
    
    class ArrayItemContext:public ShadowBuilder{
        public:
            ArrayItemContext(ShadowBuilder builder):ShadowBuilder(builder){};
            ArrayItemContext Value(json::Node::Value val){ return ShadowBuilder::Value(val); }      
            KeyContext Key(std::string key)=delete;
            ShadowBuilder EndDict()=delete; 
            json::Node Build()=delete; 
        
    };
  };
}
