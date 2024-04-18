#pragma once
#include <string>
#include <variant>
#include <vector>
#include <iterator>
#include <optional>
#include "json.h"
namespace json { 
   class KeyContext; 
   class DictItemContext;
   class ValueContextDict;
   class ArrayItemContext;
   class ValueContextArr; 
    
    class Builder{
        std::optional<std::string> key_;
        std::optional<json::Node> root_;     
        std::vector<json::Node*> nodes_stack_;       
    public:   
        Builder(){};          
        KeyContext Key(std::string key);        
        Builder& Value(json::Node::Value val);        
        DictItemContext StartDict();        
        Builder& EndDict();        
        ArrayItemContext StartArray();        
        Builder& EndArray();        
        json::Node Build();         
    };
    
        
    
    
    class DictItemContext:public Builder{
           Builder& builder_;
       public:
           DictItemContext(Builder& builder);  
        
           KeyContext Key(std::string key); 
           Builder& EndDict();  
                  
           Builder& Value(json::Node::Value val)=delete;        
           DictItemContext StartDict()=delete; 
           ArrayItemContext StartArray()=delete;        
           Builder& EndArray()=delete;        
           json::Node Build()=delete;
        
   };
    
   class KeyContext:public Builder{
           Builder& builder_;
       public:
           KeyContext(Builder& builder);        
           ValueContextDict Value(json::Node::Value val,[[maybe_unused]] bool after_key=true);       
           ArrayItemContext StartArray() ;       
           DictItemContext StartDict() ;     
       
            KeyContext Key(std::string key)=delete;      
           // Builder& Value(json::Node::Value val)=delete;  
            Builder& EndDict()=delete;
            Builder& EndArray()=delete;        
            json::Node Build()=delete;
       
    }; 
          
    class ValueContextDict :public Builder {
          Builder& builder_;
       public:
            ValueContextDict(Builder& builder);  
            Builder& EndDict();
            KeyContext Key(std::string key); 
        
                
            Builder& Value(json::Node::Value val)=delete;       
            DictItemContext StartDict()=delete;
            ArrayItemContext StartArray()=delete;        
            Builder& EndArray()=delete;       
            json::Node Build()=delete; 
        
        
        
    };

    class ArrayItemContext:public Builder{
           Builder& builder_;
       public:
            ArrayItemContext(Builder& builder);    
            ValueContextArr Value(json::Node::Value val,[[maybe_unused]] bool in_array=true);/////////
            DictItemContext StartDict();
            ArrayItemContext StartArray();
            Builder& EndArray(); 
        
            KeyContext Key(std::string key)=delete;        
            //Builder& Value(json::Node::Value val)=delete;
            Builder& EndDict()=delete; 
            json::Node Build()=delete; 
        
    };
    
   class ValueContextArr:public Builder{
           Builder& builder_;
       public:
           ValueContextArr(Builder& builder);
            ValueContextArr Value(json::Node::Value val,[[maybe_unused]] bool in_array=true);
            DictItemContext StartDict();
            ArrayItemContext StartArray();
            Builder& EndArray();
        
            KeyContext Key(std::string key)=delete;        
            //Builder& Value(json::Node::Value val)=delete; 
            Builder& EndDict()=delete;  
            json::Node Build()=delete; 
       
       
       
   };
}
