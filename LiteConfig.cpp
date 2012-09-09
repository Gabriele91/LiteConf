#include "LiteConfig.h"
//////////////////////////////
using namespace LiteConf;
//////////////////////////////
namespace LiteConf{
        enum {
            FIND_ENDFILE=0,
            FIND_GOOD=1,
            FIND_BAD=2,
            FIND_BAD_SET_VALUE=3,
            FIND_BAD_END_VALUE=4,
            FIND_BAD_START_BLOCK=5,
            FIND_BAD_END_BLOCK=6,
            FIND_VALUE=7,
            FIND_BLOCK_START=8,
            FIND_BLOCK_END=9,
            FIND_BAD_BLOCK_NAME=10,
            FIND_BAD_VALUE_NAME=11,
            FIND_BAD_NEXT_ARRAY_VALUE=12,
            FIND_BAD_END_ARRAY=13
        };
        enum{
            TOKEN_BLOCK=0,
            TOKEN_OPEN_BLOCK=1,
            TOKEN_END_BLOCK=2,
            TOKEN_SET_VAR=3,
            TOKEN_END_VAR=4,
            TOKEN_OPEN_ARRAY=5,
            TOKEN_NEXT_ARRAY_VALUE=6,
            TOKEN_END_ARRAY=7,
        };
        char lettersKey[]={'.','{','}',':',';','[',',',']'};
        char lettersJump[]={' ','\t','\r','\n'};
};

const char *ErrorParse::ErrorString []={
        "generic error",
        "name block not valid",
        "start block not found",
        "end block not found",
        "error parse block",
        "name value not valid",
        "not found propriety finalizer",
        "string not valid",
        "not found value finalizer ",
        "file not found ",
        "error parse array ",
        "not found array finalizer "
};

DFORCEINLINE bool jumpKeys(const char* in){
    bool val=false;
    for(unsigned int i=0;i<sizeof(lettersJump) && val==false;++i) val=val||((*in)==lettersJump[i]);
    return val;
}
DFORCEINLINE uchar jumpSpace(const char* in,const char** out){
    while(jumpKeys(in) && (*in)!='\0') ++in;
    if((*in)=='\0') return FIND_ENDFILE;
    (*out)=in; return FIND_GOOD;
}
/////////////////////////////////////////////
/////////////////////////////////////////////
/*
Newline	NL (LF)	10	\n
Horizontal tab	HT	9	\t
Vertical tab	VT	11	\v
Backspace	BS	8	\b
Carriage return	CR	13	\r
Formfeed	FF	12	\f
Alert	BEL	7	\a
Backslash	\	92	\\
Question mark	?	63	\?
Single quotation mark	'	39	\'
Double quotation mark	"	34	\"
*/

DFORCEINLINE uchar parseACString(const char* in,std::string& out,const char** cout=NULL){
        const char *tmp;
        out="";
        if(jumpSpace(in,&tmp)==FIND_GOOD){//[  "...."]
            if((*tmp)=='\"'){ //["...."]
                ++tmp;  //[...."]
                while((*tmp)!='\"'&&(*tmp)!='\n'){
                    if((*tmp)=='\\'){//[\.]
                        ++tmp;  //[.]
                        switch(*tmp){
                            case 'n': out+='\n'; break;
                            case 't': out+='\t'; break;
                            case 'b': out+='\b'; break;
                            case 'r': out+='\r'; break;
                            case 'f': out+='\f'; break;
                            case 'a': out+='\a'; break;
                            case '\\': out+='\\'; break;
                            case '?': out+='\?'; break;
                            case '\'': out+='\''; break;
                            case '\"': out+='\"'; break;
                            case '\n': /* jump unix */ break;
                            case '\r': /* jump mac */
                                if((*(tmp+1))=='\n') ++tmp; /* jump window (\r\n)*/
                            break;
                            default: return FIND_BAD; break;
                        }
                    }else{
                        if((*tmp)!='\0') out+=(*tmp);
                        else return FIND_BAD;
                    }
                    ++tmp;//next char
                }
                if((*tmp)=='\n') return FIND_BAD;
                if(cout) (*cout)=tmp;
                return FIND_GOOD;
            }
        }
        return FIND_BAD;
}
DFORCEINLINE uchar makeACString(const char* in,std::string& out){
        const char *tmp=in;
        out='\"';// add ".....
        while((*tmp)!='\0'){
            switch(*tmp){
                case '\n': out+="\\n"; break;
                case '\t': out+="\\t"; break;
                case '\b': out+="\\b"; break;
                case '\r': out+="\\r"; break;
                case '\f': out+="\\f"; break;
                case '\a': out+="\\a"; break;
                case '\\': out+="\\"; break;
                case '\?': out+="\\?"; break;
                case '\'': out+="\\\'"; break;
                case '\"': out+="\\\""; break;
                default: out+=(*tmp); break;
            }
            ++tmp;//next char
        }
        out+='\"';// add ...."
        return FIND_GOOD;
}
/////////////////////////////////////////////
/////////////////////////////////////////////
DFORCEINLINE uchar parseBlockName(const char* in,std::string& out,const char** cout=NULL){
        out="";

		if((*in)!='\"')
			while(!jumpKeys(in) && (*in)!= lettersKey[TOKEN_OPEN_BLOCK] &&(*in)!='\0'){ out+=(*in); ++in; }
		else{
			parseACString(in,out,&in);
			++in;
		}

        if(out.size()==0) return FIND_BAD_BLOCK_NAME;
        (*cout)=in;
        return FIND_GOOD;
}
DFORCEINLINE uchar parseValueName(const char* in,std::string& out,const char** cout=NULL){
        out="";

		if((*in)!='\"')
			while(!jumpKeys(in) && (*in)!= lettersKey[TOKEN_SET_VAR] &&(*in)!='\0'){ out+=(*in); ++in; }
		else{
			parseACString(in,out,&in);
			++in;
		}

        if(out.size()==0) return FIND_BAD_VALUE_NAME;
        (*cout)=in;
        return FIND_GOOD;
}
DFORCEINLINE uchar isEndValue(const char* in,const char** out){
    jumpSpace(in,&in);
    if((*in)== lettersKey[TOKEN_END_VAR] ) { (*out)=in; return FIND_GOOD; } return FIND_BAD_END_VALUE;
}
DFORCEINLINE uchar isSeparetor(const char* in,const char** out){
    jumpSpace(in,&in);
    if((*in)== lettersKey[TOKEN_SET_VAR] ) { (*out)=in; return FIND_GOOD; } return FIND_BAD_SET_VALUE;
}
DFORCEINLINE uchar isStartBlock(const char* in,const char** out){
    jumpSpace(in,&in);
    if((*in)== lettersKey[TOKEN_OPEN_BLOCK] ) { (*out)=in; return FIND_GOOD; } return FIND_BAD_START_BLOCK;
}
DFORCEINLINE uchar isEndBlock(const char* in,const char** out){
    jumpSpace(in,&in);
    if((*in)== lettersKey[TOKEN_END_BLOCK]) { (*out)=in; return FIND_GOOD; } return FIND_BAD_END_BLOCK;
}
DFORCEINLINE uchar isString(const char* in){
    const char* tmp;
    if(jumpSpace(in,&tmp)==FIND_GOOD){
        if((*tmp)=='\"') return FIND_GOOD;
    }
    return FIND_BAD;
}
DFORCEINLINE uchar isArray(const char* in){
    const char* tmp;
    if(jumpSpace(in,&tmp)==FIND_GOOD){
        if((*tmp)==lettersKey[TOKEN_OPEN_ARRAY]) return FIND_GOOD;
    }
    return FIND_BAD;
}
DFORCEINLINE uchar isNextArrayValue(const char* in,const char** out){
    jumpSpace(in,&in);
    if((*in)== lettersKey[TOKEN_NEXT_ARRAY_VALUE] ) { (*out)=in; return FIND_GOOD; } return FIND_BAD_NEXT_ARRAY_VALUE;
}
DFORCEINLINE uchar isEndArray(const char* in,const char** out){
    jumpSpace(in,&in);
    if((*in)== lettersKey[TOKEN_END_ARRAY] ) { (*out)=in; return FIND_GOOD; } return FIND_BAD_END_ARRAY;
}

DFORCEINLINE uchar itIsA(const char* in,const char** out){

    if(jumpSpace(in,out)==FIND_GOOD){

        if((**out)== lettersKey[TOKEN_BLOCK]){
            return FIND_BLOCK_START;
        }

        if((**out)==lettersKey[TOKEN_END_BLOCK]){
            return FIND_BLOCK_END;
        }

        if((**out)!=' '){
            return FIND_VALUE;
        }
        //in other case:
        return FIND_BAD;
    }

    return FIND_ENDFILE;
}


DFORCEINLINE unsigned int countChar(const char* in,char c){
    int i=0;
    while((*in)!='\0'){ if((*in)==c) ++i; ++in;}
    return i;
}
DFORCEINLINE unsigned int countChar(const char* in,char c,const char* end){
    int i=0;
    while((*in)!='\0' && in!=end ){ if((*in)==c) ++i; ++in;}
    return i;
}

//String
float String::StringToFloat(std::string str){
    return (float)atof(str.c_str());
}

//Value
Value::Value(){ value._float=0; }
Value::Value(const std::string& name,float value)
            :name(name),type(VALUE_NUMBER){
    this->value._float=value;
}
Value::Value(const std::string& name,const std::string& value)
            :name(name),type(VALUE_STRING){
    this->value._string=(char*)malloc(value.size()+1);
    std::strcpy(this->value._string,value.c_str());
    this->value._string[value.size()]='\0';
}
Value::~Value(){
    if(type==VALUE_STRING){
		free(this->value._string);
	}
	if(type==VALUE_ARRAY){
		for(unsigned int i=0;i<value._array->size();i++) delete (*(value._array))[i];
		delete value._array;
	}
}

std::string Value::ToString() const {
	if(type==VALUE_STRING) return value._string;
	if(type==VALUE_NUMBER) return String::ToString<float>(value._float);
	if(type==VALUE_ARRAY){

		std::string tmp("[ ");
		for(unsigned int i=0;i<value._array->size();++i){ 
			if((*(value._array))[i]->type==Value::VALUE_STRING){
				std::string outstring;
				makeACString((*(value._array))[i]->value._string,outstring);
					tmp+=outstring+", ";
			}else
				tmp+=(*(value._array))[i]->ToString()+", ";
		}
		tmp[tmp.size()-2]=' ';
		tmp[tmp.size()-1]=']';
		return tmp;
	}

	return "";
}

uchar Value::ParseValue(Block *block,ErrorParse& error,const char* start,const char* in,const char** end){
            //tmp string
            std::string stmp,stmp2;
            //tmp float
            float ftmp;
            //in is at first letter [A]BC :
            if(parseValueName(in,stmp,&in)==FIND_BAD_VALUE_NAME){
                error.PushError(countChar(start,'\n',in),ErrorParse::ERROR_PARSE_NAME_VALUE);
                return FIND_BAD;
            }
            if(isSeparetor(in,&in)==FIND_BAD_SET_VALUE){
                error.PushError(countChar(start,'\n',in),ErrorParse::ERROR_FIND_SET_VALUE);
                return FIND_BAD;
            }
            // out is a ptr to ":"
            if(isString(in+1)==FIND_GOOD){
                //STRING
                if(parseACString(in+1,stmp2,&in)==FIND_BAD){
                    error.PushError(countChar(start,'\n',in),ErrorParse::ERROR_PARSE_STRING);
                    return FIND_BAD;
                }
                block->PushValue(stmp,stmp2);
            }
            else if(isArray(in+1)==FIND_GOOD){
                //Array
                Value *out;
                if(Value:: ParseArray(&out,error,start,in+1,&in)==FIND_BAD){
                    error.PushError(countChar(start,'\n',in),ErrorParse::ERROR_PARSE_ARRAY);
                    return FIND_BAD;
                }
				out->name=stmp;
                block->PushValue(stmp,out);
            }
            else{
                //FLOAT
                ftmp=(float)std::strtod(in+1,(char**)&(in));
                block->PushValue(stmp,ftmp);
                --in; //because strtod get first char after value
            }
            if(isEndValue(++in/*after \" or number or end array*/,&(in))==FIND_BAD_END_VALUE){
                error.PushError(countChar(start,'\n',in),ErrorParse::ERROR_FIND_END_VALUE);
                return FIND_BAD;
            }
            ++in; //jump next char (after ;)
            //save pointer
            (*end)=in;
            //
            return FIND_GOOD;
}
uchar Value::ParseArray(Value **out,ErrorParse& error,const char* start,const char* in,const char** end){

    (*out)=new Value();
    (*out)->type=Value::VALUE_ARRAY;
    (*out)->value._array=new std::vector<Value*>();
	//void array []
    if(isEndArray(in+1,&in)==FIND_GOOD){
		(*end)=in;
		return FIND_GOOD; 
	}
    //tmp string
    std::string stmp;
    //tmp float
    float ftmp;

    do{	
	   jumpSpace(in+1,&in); --in;
       //"...
       if(isString(in+1)==FIND_GOOD){
            //STRING
            if(parseACString(in+1,stmp,&in)==FIND_BAD){
                error.PushError(countChar(start,'\n',in),ErrorParse::ERROR_PARSE_STRING);
                return FIND_BAD;
            }
            (*out)->value._array->push_back(new Value("",stmp));
       } 
	   //[....
	   else if(isArray(in+1)==FIND_GOOD){
                //Array
                Value *newArrayOut;
                if(Value:: ParseArray(&newArrayOut,error,start,in+1,&in)==FIND_BAD){
                    error.PushError(countChar(start,'\n',in),ErrorParse::ERROR_PARSE_ARRAY);
                    return FIND_BAD;
                }
                (*out)->value._array->push_back(newArrayOut);
       }
       //0...
       else{
           //FLOAT
            ftmp=(float)std::strtod(in+1,(char**)&(in));
            (*out)->value._array->push_back(new Value("",ftmp));
            --in; //because strtod get first char after value
       }
       //next char ( ".."[ ] or 0..9[ ]  )
       ++in;
       //
    }while(isNextArrayValue(in,&in)==FIND_GOOD);
    //
    //find end array [ . . . . [ ] ]
    if(isEndArray(in,&in)!=FIND_GOOD){
        error.PushError(countChar(start,'\n',in),ErrorParse::ERROR_FIND_END_ARRAY);
        return FIND_BAD;
    }
    //save pointer
	(*end)=in;
    return FIND_GOOD;

}

//Block
Block::Block(Block *parent){ this->parent=parent; }
Block::Block(const std::string& name,Block *parent):name(name),parent(parent){}
Block::~Block(){

	for(MapValuesIterator itmap=this->values.begin();itmap!=this->values.end();itmap++)
		for(ValuesIterator it=(*itmap).second.begin();it!=(*itmap).second.end();it++)
			delete (*it);

	for(MapBlocksIterator itmap=this->blocks.begin();itmap!=this->blocks.end();itmap++)
		for(BlocksIterator it=(*itmap).second.begin();it!=(*itmap).second.end();it++)
			delete (*it);

}

uchar Block::PushBlocks(ErrorParse& error,const std::string& name,const char* start,const char* in,const char** end){
    Block *block=new Block(name,this);
    if(block->ParseValues(error,start,in,end)==FIND_GOOD){
        blocks[name].push_back(block);
        return FIND_GOOD;
    }
    return FIND_BAD;
}

uchar Block::ParseValues(ErrorParse& error,const char* start,const char* in,const char** end){

    std::string stmp;
    std::string stmp2;
    uchar tmptype=0;

    while((tmptype=itIsA(in,&in))!=FIND_ENDFILE){
        switch(tmptype){
            case FIND_BLOCK_START:
            //add block
            if(jumpSpace(++in /*after .*/,&in /*.-->\"....\" */)==FIND_ENDFILE){
                error.PushError(countChar(start,'\n',in),ErrorParse::ERROR_PARSE_NAME_BLOCK);
                return FIND_BAD;
            }
            if(parseBlockName(in,stmp,&in)==FIND_BAD_BLOCK_NAME){
                error.PushError(countChar(start,'\n',in),ErrorParse::ERROR_PARSE_NAME_BLOCK);
                return FIND_BAD;
            }
            if(isStartBlock(in,&in)==FIND_BAD_START_BLOCK){
                error.PushError(countChar(start,'\n',in),ErrorParse::ERROR_FIND_START_BLOCK);
                return FIND_BAD;
            }
            if(PushBlocks(error,stmp,start,in+1,&in)==FIND_GOOD){
                break;
            }
            error.PushError(countChar(start,'\n',in),ErrorParse::ERROR_IN_BLOCK);
            return FIND_BAD;

            break;

            case FIND_BLOCK_END:
                (*end)=in+1;
                return FIND_GOOD;
            break;

            case FIND_VALUE:
                if(Value::ParseValue(this,error,start,in,&in)==FIND_BAD){
                    return FIND_BAD;
                }
                break;
            default:
                if(tmptype==FIND_BAD)
                    error.PushError(countChar(start,'\n',in)+1,ErrorParse::ERROR_PARSE);
                if(tmptype==FIND_ENDFILE)
                    error.PushError(countChar(start,'\n',in),ErrorParse::ERROR_FIND_END_BLOCK);
                return FIND_BAD;
                break;
            }
        }
        if(tmptype==FIND_ENDFILE && this->parent) //if block isn't root
            error.PushError(countChar(start,'\n',in),ErrorParse::ERROR_FIND_END_BLOCK);

        if(tmptype==FIND_ENDFILE) //if block is root
            return FIND_GOOD;
        //
        error.PushError(countChar(start,'\n',in),ErrorParse::ERROR_PARSE);
            return FIND_BAD;

}

std::string Block::MakeScript(int livel){
    std::string space,out,outstring,outnamestring;
    char* jump=(char*)"";

    char* ifJumpTab=(char*)"\t";
    char* ifJumpBk=(char*)"\n";
    if(jumpKeys(ifJumpTab)) for(int i=0;i<livel;++i) space+=(char*)"\t";
    if(jumpKeys(ifJumpBk)) jump=(char*)"\n";


	for(MapValuesIterator itmap=this->values.begin();itmap!=this->values.end();itmap++){
		for(ValuesIterator it=(*itmap).second.begin();it!=(*itmap).second.end();it++){
			makeACString((*it)->name.c_str(),outnamestring);
			if((*it)->type==Value::VALUE_STRING){
				makeACString((*it)->value._string,outstring);
				out+=space+outnamestring+lettersKey[TOKEN_SET_VAR]+outstring+lettersKey[TOKEN_END_VAR]+jump;
			}
			else if((*it)->type==Value::VALUE_ARRAY){
				out+=space+outnamestring+lettersKey[TOKEN_SET_VAR]+(*it)->ToString()+lettersKey[TOKEN_END_VAR]+jump;
			}
			else{
				out+=space+outnamestring+lettersKey[TOKEN_SET_VAR]+String::ToString<float>((*it)->value._float)+lettersKey[TOKEN_END_VAR]+jump;
			}
		}
    }
	for(MapBlocksIterator itmap=this->blocks.begin();itmap!=this->blocks.end();itmap++){
		for(BlocksIterator it=(*itmap).second.begin();it!=(*itmap).second.end();it++){
			makeACString((*it)->name.c_str(),outnamestring);
			out+=space+lettersKey[TOKEN_BLOCK]+outnamestring+lettersKey[TOKEN_OPEN_BLOCK]+jump;
			out+=(*it)->MakeScript(livel+1);
			out+=space+lettersKey[TOKEN_END_BLOCK]+jump;
		}
	}
    return out;
}
std::string Block::MakeLiteScript(){
    std::string out,outstring,outnamestring;
	for(MapValuesIterator itmap=this->values.begin();itmap!=this->values.end();itmap++){
		for(ValuesIterator it=(*itmap).second.begin();it!=(*itmap).second.end();it++){
			makeACString((*it)->name.c_str(),outnamestring);
			if((*it)->type==Value::VALUE_STRING){
				makeACString((*it)->value._string,outstring);
				out+=outnamestring+lettersKey[TOKEN_SET_VAR]+outstring+lettersKey[TOKEN_END_VAR];
			}
			else if((*it)->type==Value::VALUE_ARRAY){
				out+=outnamestring+lettersKey[TOKEN_SET_VAR]+(*it)->ToString()+lettersKey[TOKEN_END_VAR];
			}
			else{
				out+=outnamestring+lettersKey[TOKEN_SET_VAR]+String::ToString<float>((*it)->value._float)+lettersKey[TOKEN_END_VAR];
			}
		}
    }
	for(MapBlocksIterator itmap=this->blocks.begin();itmap!=this->blocks.end();itmap++){
		for(BlocksIterator it=(*itmap).second.begin();it!=(*itmap).second.end();it++){
		makeACString((*it)->name.c_str(),outnamestring);
        out+=lettersKey[TOKEN_BLOCK]+outnamestring+lettersKey[TOKEN_OPEN_BLOCK];
        out+=(*it)->MakeLiteScript();
        out+=lettersKey[TOKEN_END_BLOCK];
		}
	}
    return out;
}


ObjPushArray Block::PushArray(const std::string& name){    
	Value *out;
	out=new Value();
	out->name=name;
    out->type=Value::VALUE_ARRAY;
    out->value._array=new std::vector<Value*>();
	this->PushValue(name,out);
	return ObjPushArray(this,out);
}
Block* Block::Push(const std::string& name){
    return this->LitePushBlock(name,this);
    }
Block* Block::Push(const std::string& name,const std::string& value){
    PushValue(name,value);
    return this;
    }
Block* Block::Push(const std::string& name,float value){
    PushValue(name,value);
    return this;
    }
Block* Block::Back(){
    return this->parent;
    }


Value* Block::GetFirstValue(const std::string& name){
	IteratorValues it=Block::GetValues(name);
    if(!it.IsEmpty()){
        return it.Get();
    }
    return NULL;
}
//////////////////////////////////////////////////////
IteratorValues Block::GetValues(const std::string& name){
	MapValuesIterator mapval=this->values.find(name);
	if(mapval!=this->values.end()){
		return IteratorValues(mapval);
	}
	return IteratorValues();
}
//////////////////////////////////////////////////////
IteratorValues Block::GetValues(){
	MapValuesIterator mapval=this->values.begin();
	if(mapval!=this->values.end()){
		return IteratorValues(mapval);
	}
	return IteratorValues();
}
//////////////////////////////////////////////////////
Block* Block::GetFirstBlock(const std::string& name){
	IteratorBlocks it=Block::GetBlocks(name);
    if(!it.IsEmpty()){
        return it.Get();
    }
    return NULL;
}
//////////////////////////////////////////////////////
IteratorBlocks Block::GetBlocks(const std::string& name){

	MapBlocksIterator mapblk=this->blocks.find(name);
	if(mapblk!=this->blocks.end()){
		return IteratorBlocks(mapblk);
	}
	return IteratorBlocks();
}
//////////////////////////////////////////////////////
IteratorBlocks Block::GetBlocks(){

	MapBlocksIterator mapblk=this->blocks.begin();
	if(mapblk!=this->blocks.end()){
		return IteratorBlocks(mapblk);
	}
	return IteratorBlocks();
}
//////////////////////////////////////////////////////


//Script
Script::Script():root("root"){}
//
bool Script::Parse(const std::string& filename){
    const char *endC;
    char* string=GetFile(filename);
    if(string){
        bool isGoodLoad=root.ParseValues(error,string,string,&endC)==FIND_GOOD;
        free(string);
        return isGoodLoad;
    }
	error.PushError(0,ErrorParse::ERROR_FILE_NOT_FOUND);
    return false;
}
bool Script::ParseString(const std::string& scode){
    const char *endC;
    if(scode.size()){
        bool isGoodLoad=root.ParseValues(error,scode.c_str(),scode.c_str(),&endC)==FIND_GOOD;
        return isGoodLoad;
    }
    return false;
}
std::string Script::MakeScript(){
    return root.MakeScript();
}
std::string Script::MakeLiteScript(){
    return root.MakeLiteScript();
}
std::string Script::OuputErrors(){
    std::string out;
    for(unsigned int i=0;i<error.errors.size();++i){
        out+=std::string("at line:")+
        String::ToString<unsigned int>(error.errors[i].line)+
        "\t error:"+
        ErrorParse::ErrorString[error.errors[i].error]+
        "\n";
    }
    return out;
}
Block* Script::GetRoot(){
    return &root;
}


////////////////////////////////////
std::ostream& LiteConf::operator<<(std::ostream &output,Value* value){ 
	output<< value->ToString(); return output;
}
std::ostream& LiteConf::operator<<(std::ostream &output,const Value::ValueArray& value){ 
	output<< value.ptr->ToString(); return output;
}
////////////////////////////////////