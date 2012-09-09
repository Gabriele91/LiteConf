#ifndef LITE_CONFIG_H
#define LITE_CONFIG_H
/**********************************************************************************
* @autor: Gabriele Di Bari
* @version 1.6
*
* MIT LICENSE
*
* Copyright (c) 2012 Gabriele Di Bari
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
**********************************************************************************/


#include <string>
#include <vector>
#include <list>
#include <map>
#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <stdio.h>

#ifdef _MSC_VER
    #define DINLINE _inline
    #define DFORCEINLINE __forceinline
#else
    #define DINLINE inline
    #define DFORCEINLINE inline
#endif

namespace LiteConf{

        namespace String{
            template<class T> std::string ToString(T dato){
                   std::stringstream br;
                   br << dato;
                   return br.str();
                }
            float StringToFloat(std::string str);
        };

        extern char lettersKey[];
        extern char lettersJump[];
        typedef unsigned char uchar;
        //class declaration
        struct Value;
        struct Block;
        struct ObjPushArray;
        class  Script;
        typedef std::list<Value*>::iterator ValuesIterator;
        typedef std::list<Block*>::iterator BlocksIterator;
        typedef std::map<std::string, std::list<Value*> >::iterator MapValuesIterator;
        typedef std::map<std::string, std::list<Block*> >::iterator MapBlocksIterator;

        /**
        *  Class management errors
        */
        struct ErrorParse{

            enum ErrorType{
                ERROR_PARSE=0,
                ERROR_PARSE_NAME_BLOCK=1,
                ERROR_FIND_START_BLOCK=2,
                ERROR_FIND_END_BLOCK=3,
                ERROR_IN_BLOCK=4,
                ERROR_PARSE_NAME_VALUE=5,
                ERROR_FIND_SET_VALUE=6,
                ERROR_PARSE_STRING=7,
                ERROR_FIND_END_VALUE=8,
                ERROR_FILE_NOT_FOUND=9,
                ERROR_PARSE_ARRAY=10,
                ERROR_FIND_END_ARRAY=11
            };

            struct Error{ unsigned int line; uchar error; };
            std::vector<Error> errors;
            static const char *ErrorString[];
            void PushError(unsigned int line, uchar errorType){
                Error error;
                error.line=line+1;
                error.error=errorType;
                errors.push_back(error);
            }
        };


        /**
        *  Class value representation
        */
        struct Value{

            friend struct Block;
            friend struct ObjPushArray;

        protected:

            std::string name;///<name value

            uchar type;///<type value

            union _ValueUnion{
                std::vector<Value*> *_array;
                char *_string;
                float _float;
            }value; ///< value
        
            /**
            * Value constructor.
            */
            Value();
            /**
            * Value destructor.
            */
            ~Value();
            /**
            * Value constructor.
            * @param name vale
            * @param float (value)
            */
            Value(const std::string& name,float value);

            /**
            * Value constructor.
            * @param name value
            * @param string (value)
            */
            Value(const std::string& name,const std::string& value);
            /**
            * Parse value
            */
            static uchar ParseValue(Block *block,ErrorParse& error,const char* start,const char* in,const char** end);
            /**
            * Parse value array
            */
            static uchar ParseArray(Value **out,ErrorParse& error,const char* start,const char* in,const char** end);

        public:
            /**
            * Enum value types
            */
            enum{
                VALUE_NUMBER=1, ///< value is a number
                VALUE_STRING=2, ///< value is a string
                VALUE_ARRAY=3   ///< value is a array
            };
            /**
            * Class array overload Value::operator[]
            */ 
            struct ValueArray{             
                    ValueArray operator[] (unsigned int i) { return ValueArray(ptr->GetValueArray(i)); }  
                    ValueArray operator[] (unsigned int i) const { return ValueArray(ptr->GetValueArray(i)); }
                    /**
                    * If value is an array, return true
                    * @return bool
                    */
                    bool IsArray(){ return ptr->type==VALUE_ARRAY; }
                    /**
                    * Return size array
                    * @return size (unsigned int)
                    */
                    unsigned int Size(){ if(ptr->type==VALUE_ARRAY) return (ptr->value._array->size()); else return 0;  }
                    /**
                    * Push a string value and return there where it is pushed
                    * @param value (string)
                    * @return index
                    */
                       unsigned int Push(const std::string& v){ 
                       if(ptr->type==VALUE_ARRAY){ 
                           ptr->value._array->push_back(new Value("",v));
                           return Size()-1;
                       }
                       return -1; 
                    }
                    /**
                    * Push a number value and return there where it is pushed
                    * @param value (float)
                    * @return index
                    */
                       unsigned int Push(float v){ 
                       if(ptr->type==VALUE_ARRAY){ 
                           ptr->value._array->push_back(new Value("",v));
                           return Size()-1;
                       }
                       return -1; 
                    }
                    /**
                    * Push an array and return there where it is pushed
                    * @return index
                    */
                       unsigned int PushArray(){ 
                       if(ptr->type==VALUE_ARRAY){                    
                           //array
                           Value *out=new Value();
                           out->type=Value::VALUE_ARRAY;
                           out->value._array=new std::vector<Value*>();
                           ptr->value._array->push_back(out); 
                           return Size()-1;
                       }
                       return -1;
                    }
                    /**
                    * Remove a value from array
                    * @param index
                    */
                    void Remove(unsigned int i){
                           if(i<Size()){
                            delete (*ptr->value._array)[i];
                            ptr->value._array->erase(ptr->value._array->begin()+i);
                           }
                    }

                    Value * operator->() { return ptr; }  
                    Value * operator->() const { return ptr; } 

                    operator float () const { return ptr->ToFloat(); }
                    operator std::string () const { return ptr->ToString(); }
                    ValueArray(const ValueArray& v):ptr(v.ptr){}        

                private: 
                    friend struct Value;
                    friend std::ostream& operator<<(std::ostream &output,const Value::ValueArray& value);

                    Value *ptr; 
                    ValueArray():ptr(0){}
                    ValueArray( Value *ptr ):ptr(ptr){}        
            };
            /**
            * Get value type.
            * @return value type
            */
            uchar GetType(){ return this->type; }
            /**
            * If value is an array, return true
            * @return bool
            */
            bool IsArray(){ return type==VALUE_ARRAY; }
            /**
            * If value is a string, return true
            * @return bool
            */
            bool IsString(){ return type==VALUE_STRING; }
            /**
            * If value is a number, return true
            * @return bool
            */
            bool IsNumber(){ return type==VALUE_NUMBER; }
            /**
            * Get value name.
            * @return value name (std::string)
            */
            const std::string& GetName(){ return this->name; }
            /**
            * Get value string.
            * @return value string (constant character pointer)
            */
            const char* GetCString(){
                if(type==VALUE_STRING){
                    return value._string;
                }
                else{
                    return 0;
                }

            }
            /**
            * Get value string.
            * @return value string (constant character pointer)
            */
            std::string GetString(){
                if(type==VALUE_STRING){
                    return std::string(value._string);
                }
                else{
                    return "";
                }
            }
            /**
            * Get value number.
            * @return value number (float)
            */
            float GetFloat(){
                if(type==VALUE_NUMBER){
                    return value._float;
                }
                else{
                    return 0;
                }

            }
            /**
            * Get array
            * @return array pointer
            */
            std::vector<Value*>* GetArray(){
                if(type==VALUE_ARRAY){
                    return value._array;
                }
                else{
                    return 0;
                }
            }
            /**
            * Get a array value 
            * @param ID value
            * @return value pointer
            */
            Value* GetValueArray(unsigned int id) const{
                if(type==VALUE_ARRAY){
                    if(value._array->size()>id)
                        return (*(value._array))[id];
                    else
                        return 0;
                }
                else{
                    return 0;
                }
            }
            /**
            * Return a ValueArray
            * @return ValueArray
            */
            ValueArray Array() const{
                return ValueArray((Value*)this);
            }
            /**
            * Get value cast in a string, if value an array return script array.
            * @return string (std::string)
            */
            std::string ToString() const;
            /**
            * Get value cast in a number, if value is an array return array.size().
            * @return number (float)
            */
            float ToFloat() const {
                if(type==VALUE_STRING) return String::StringToFloat(std::string(value._string));
                if(type==VALUE_NUMBER) return value._float;
                if(type==VALUE_ARRAY)  return (float)(value._array->size());
                return 0;
            }
            /**
            * Cast float
            */
            operator float () const { return this->ToFloat(); }
            operator std::string () const { return this->ToString(); }            
            friend std::ostream& operator<<(std::ostream &output,Value* value);
            ValueArray operator[] (unsigned int i) { return ValueArray(this->GetValueArray(i)); }  
            ValueArray operator[] (unsigned int i) const { return ValueArray(this->GetValueArray(i)); } 
            //
        };        
        /**@addtogroup std::cout<<Value overload */
        /*@{*/
        extern std::ostream& operator<<(std::ostream &output,Value* value);
        extern std::ostream& operator<<(std::ostream &output,const Value::ValueArray& value);
        /*@}*/

        /** @addtogroup IteratorsGroup */
        /*@{*/
        template<class ITMAP,class IT,class V> struct Iterator{

         friend struct Block;

        protected:

            ITMAP values;
            IT it;
            int IsInvalid;

            Iterator(const ITMAP& vals)
            :values(vals),IsInvalid(false)
            {
                it=(*values).second.begin();
            }
            Iterator():values(),IsInvalid(true)
            {
            }

        public:

            /**
            * Move iterator to next node
            * @return true if is not the end node
            */
            bool Next(){
                if(IsInvalid) return false;
                it++;
                return it!=(*values).second.end();
            }
            /**
            * Set the iterators position back to first node
            */
            void Restart(){
                if(IsInvalid) return;
                it=((*values).second.begin());
            }
            /**
            * Get true if it is a pointer to first node
            */
            bool IsAtStart(){
                if(IsInvalid) return false;
                return it==(*values).second.begin();
            }
            /**
            * Get true if it is a pointer to last node
            * @return bool
            */
            bool IsAtEnd(){
                if(IsInvalid) return true;
                return it==(*values).second.end();
            }
            /**
            * Return true if after this node there's another node
            * @return bool
            */
            bool HasNext(){
                if(IsInvalid) return false;
                return it!=(*values).second.end();
            }
            /**
            * Return true if there aren't node
            * @return bool
            */
            bool IsEmpty(){
                if(IsInvalid) return true;
                return (*values).second.size()==0;
            }
            /**
            * Get object
            * @return value or block
            */
            V* Get() const {
                if(IsInvalid) return NULL;
                return (*it);
            }

        };

        /**
        *  Class values iterator
        */
        struct IteratorValues: public Iterator<MapValuesIterator,ValuesIterator,Value>{
          friend struct Block;
          IteratorValues():Iterator<MapValuesIterator,ValuesIterator,Value>(){}
          IteratorValues(const MapValuesIterator& vals):Iterator<MapValuesIterator,ValuesIterator,Value>(vals){}
        };

        /**
        *  Class blocks iterator
        */
        struct IteratorBlocks: public Iterator<MapBlocksIterator,BlocksIterator,Block>{
          friend struct Block;
          IteratorBlocks():Iterator<MapBlocksIterator,BlocksIterator,Block>(){}
          IteratorBlocks(const MapBlocksIterator& vals):Iterator<MapBlocksIterator,BlocksIterator,Block>(vals){}
        };
        /*@}*/


        /**
        * Class push array
        */
        struct ObjPushArray{

                  friend struct Block;
                  /**
                  * Push a string value and return there where it is pushed
                  * @param value (string)
                  * @return ObjPushArray
                  */
                  ObjPushArray Push(const std::string& value){
                    this->value->value._array->push_back(new Value("",value));
                    return *this;
                  }
                  /**
                  * Push a number value and return there where it is pushed
                  * @param value (float)
                  * @return ObjPushArray
                  */
                  ObjPushArray Push(float value){
                    this->value->value._array->push_back(new Value("",value));
                    return *this;
                  }                 
                  /**
                  * Push an sub array and return where you add values into array
                  * @param name array (string)
                  * @return ObjPushArray
                  */
                  ObjPushArray PushArray(){
                    //array
                    Value *out=new Value();
                    out->type=Value::VALUE_ARRAY;
                    out->value._array=new std::vector<Value*>();
                    //array
                    //add array
                    this->value->value._array->push_back(out);
                    //return pusher class
                    return ObjPushArray(obj,out,this->value);
                  }
                  /**
                  * Return to super array
                  */
                  ObjPushArray Closure(){ return ObjPushArray(obj,backValue); }
                  /**
                  * Return to block
                  */
                  Block* Back(){ return obj; }

                  //overload operator
                  ObjPushArray* operator->() { return (ObjPushArray* )this; }
                  ObjPushArray* operator->() const { return (ObjPushArray* )this; }
                  
                  protected:
                    
                  Value *value;
                  Value *backValue;
                  Block* obj;

                  ObjPushArray()
                  :obj(0)
                  ,value(0)
                  ,backValue(0){}

                  ObjPushArray(Block* obj,Value *value,Value *backValue=0)
                  :obj(obj)
                  ,value(value)
                  ,backValue(backValue){}

        };

        /**
        *  Class block representation
        */
        struct Block{

            friend struct Value;
            friend class Script;

        protected:
            //value list
            std::map<std::string, std::list<Value*> > values;///<values in this block
            std::map<std::string, std::list<Block*> > blocks;///<blocks in this block
            std::string name;///<name block
            Block *parent;///<parent block

            DFORCEINLINE void PushValue(const std::string& name,float value){
                values[name].push_back(new Value(name,value));
            }
            DFORCEINLINE void PushValue(const std::string& name,const std::string& value){
                values[name].push_back(new Value(name,value));
            }
            DFORCEINLINE void PushValue(const std::string& name,Value* value){
                values[name].push_back(value);
            }
            DFORCEINLINE Block* LitePushBlock(const std::string& name,Block *parent=NULL){
                Block* block=new Block(name,parent);
                blocks[name].push_back(block);
                return block;
            }

            /**
            * Push a Block from script (parse)
            */
            uchar PushBlocks(ErrorParse& error,const std::string& name,const char* start,const char* in,const char** end);

            Block(Block *parent=NULL);
            Block(const std::string& name,Block *parent=NULL);
            ~Block();
            uchar ParseValues(ErrorParse& error,const char* start,const char* in,const char** end);
            /**
            * Return a script from this block
            */
            std::string MakeScript(int livel=0);
            /**
            * Return a script from this block without spaces
            */
            std::string MakeLiteScript();
    
        public:
            /**
            * Get name block;
            * @return name (std::string)
            */
            const std::string GetName(){
                return this->name;
            }
            /**
            * Push a block and return it
            * @param name block (string)
            * @return block (pushed)
            */
            Block* Push(const std::string& name);
            /**
            * Push an array and return BlockPushArray, where you add values into array
            * @param name array (string)
            * @return BlockPushArray
            */
            ObjPushArray PushArray(const std::string& name);
            /**
            * Push a string value and return there where it is pushed
            * @param name value (string)
            * @param value (string)
            * @return block
            */
            Block* Push(const std::string& name,const std::string& value);
            /**
            * Push a number value and return there where it is pushed
            * @param name value (string)
            * @param value (float)
            * @return block
            */
            Block* Push(const std::string& name,float value);
            /**
            * Get parent block
            */
            Block* Back();
            /**
            * Get first value with this name.
            * @param name value (string)
            * @return value
            */
            Value* GetFirstValue(const std::string& name);
            /**
            * Get all values
            * @return IteratorValues
            */
            IteratorValues GetValues();
            /**
            * Get all values with this name
            * @param name values (string)
            * @return IteratorValues
            */
            IteratorValues GetValues(const std::string& name);

            /**
            * Get first block with this name.
            * @param name block (string)
            * @return block
            */
            Block *GetFirstBlock(const std::string& name);
            /**
            * Get all blocks
            * @return IteratorBlocks
            */
            IteratorBlocks GetBlocks();
            /**
            * Get all blocks with this name
            * @param name blocks (string)
            * @return IteratorBlocks
            */
            IteratorBlocks GetBlocks(const std::string& name);
        };
        /**
        * Class script manager
        */
        class Script{

            private:
                Block root;
                ErrorParse error;
                /**
                * Get file length
                 */
                static size_t GetFileLen(FILE *file){
                    //Get file length
                    fseek(file, 0L, SEEK_END);
                    size_t fileLen=(size_t)ftell(file);
                    fseek(file, 0L, SEEK_SET);
                    //
                    return fileLen;
                }
                /**
                * Load a file and save in a CString
                * @param file name (std::string)
                * @return char pointer (You need to deallocate)
                */
                char* GetFile(const std::string& filename){
                    FILE *file = fopen(filename.c_str(), "rb");
                    if(file){
                        size_t size=GetFileLen(file);
                        char* script=(char*)malloc(size+1);
                        fread(script,size,1,file);
                        fclose(file);
                        script[size]='\0';
                        return  script;
                    }
                    return NULL;
                }

            public:

                /**
                * Script constructor.
                */
                Script();
                /**
                * Parse a file
                * @param file name (std::string)
                * @return true if there aren't errors
                 */
                bool Parse(const std::string& filename);
                /**
                * Parse a string
                * @param script (std::string)
                * @return true if there aren't errors
                 */
                bool ParseString(const std::string& scode);
                /**
                * Serialize all blocks
                * @return script (std::string)
                 */
                std::string MakeScript();
                /**
                * Serialize all blocks
                * @return script without spaces (std::string)
                 */
                std::string MakeLiteScript();
                /**
                * Return errors
                * @return string
                 */
                std::string OuputErrors();
                /**
                * Get root block
                * @return block pointer
                 */
                Block* GetRoot();
        };


};


#endif
