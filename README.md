
![Alt text](https://github.com/Gabriele91/LiteConf/blob/master/LiteConf.png?raw=true)
=============
##About
LiteConf is a lightweight and easy to use library written in C++ able to read and write configuration files.

######Version 1.6

EBNF
-------

    <number> is floating point value
    <string> like CString
    <word> is a singol word without special chars
    
    <value> := [<string>|<number>|<array>]
    <name> := <word>|<string> 
    
    <array> := "["  [<value> {[ "," <value> ]}] "]"
    <property> := <name> ":" [<value>] ";"
    <block> := "." <name> "{" { [<property>] }  { [<block>] } "}"
 
######Single-Line comments
    # it is a comment
 
######Multi-Line comments
    # line 1\
      line 2\
      line 3

######String Special Characters

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

######Multi-Line strings

	"\
	hello\
	world\
	"

Differences between LiteConf and JSON:
-------

- In JSON you can't use only a word for a value name or object name.
- In LiteConf you can add comments.
- LiteConf supports multiline strings.
- A object in LiteConf starts with point.
- LiteConf has arrays of values, JSON has arrays of objects.


Compiling
-------
###On Linux or Mac OS X (or MinGW)
Make all
 
    make
 

Make only libary or example
 
    make release
    make debug
    make example
 

###Visual Studio

In LiteConfigVS open LiteConfigVS.sln and compile all project.

###Code::Blocks

Open LiteConf.cbp select specific "build target" and click on build.

Example
-------

###C++ make a script:

 
	LiteConf::Script script;
	script->GetRoot()->Push("City", "New York city")
					 ->Push("main")
						->Push("Person")
						->Push("name","Luka")
						->Push("gender","woman")
							->Push("age",18)
							->Push("Other characteristics")
								->Push("eyes","blue")
							->Back()
						->Back()
						->Push("Person")
							->Push("name","Jim")
							->Push("gender","man")
							->Push("age",21)
							->Push("address")
								->Push("Postal code",90998)
								->Push("street","Jamestown")
								->Push("number",5)
	std::cout<<script.MakeScript();
 


###Script look like:
 
	City : "New York city";
	.main
	{
		.Person{

			name: "Luka";
			gender : "woman";
			age: 18;
			
			."Other characteristics"{
				eyes: "blue";
			}

		}

		.Person{

			name: "Jim";
			gender : "man";
			age: 21;
			
			.address {
				"Postal code": 90998;
				street:"Jamestown";
				number: 5;
			}
			
		}
	}
 

 
###C++ Read file

 
	LiteConf::Script script;

	if(!script.Parse("file.cf")){
		std::cout<< script.OuputErrors();
		return 0;
	}

	//print city value
	std::cout << script.GetRoot()->GetFirstValue("City")->ToString()<< std::endl;

	//get main block
	LiteConf::Block *bkMain=script.GetRoot()->GetFirstBlock("main");

	//get all "Person" block
	LiteConf::IteratorBlocks itBk=bkMain->GetBlocks("Person");

	while(itBk.HasNext()){
		//get block
		LiteConf::Block *bk=itBk.Get();
		std::cout << bk->GetFirstValue("name")->ToString() << std::endl;
		std::cout << bk->GetFirstValue("gender")->ToString() << std::endl;
		std::cout << bk->GetFirstValue("age")->ToFloat() << std::endl;
		
		//get Other characteristics values
		LiteConf::Block *bkOther=bk->GetFirstBlock("Other characteristics");
		
		if(bkOther){
			LiteConf::IteratorValues itValues=bkOther->GetValues();
			while(itValues.HasNext()){
				//if value is a number, ToString cast it in a string
				std::cout << itValues.Get()->name << ":" << itValues.Get()->ToString() << std::endl;
				itValues.Next();
			}
		}
		
		//get address values
		LiteConf::Block *bkAddress=bk->GetFirstBlock("address");
		
		if(bkAddress){
			LiteConf::IteratorValues itValues=bkAddress->GetValues();
			while(itValues.HasNext()){
				//if value is a number, ToString cast it in a string
				std::cout << itValues.Get()->GetName() << ":" << itValues.Get()->ToString() << std::endl;
				itValues.Next();
			}
		}

	//next block
	itBk.Next();
	
	}
 
 
MIT LICENSE
-------

 Copyright (c) 2012 Gabriele Di Bari

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
