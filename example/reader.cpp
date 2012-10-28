#include "../LiteConfig.h"
#include <iostream>

int main(){

	LiteConf::Script script;
	if(!script.Parse("file.cf")){
		std::cout<< script.OuputErrors();
		return 0;
	}

	//std::vector<LiteConf::Value*> *marray=script.GetRoot()->GetFirstValue("array")->GetArray();
	//or
	for(unsigned int i=0;i<script.GetRoot()->GetFirstValue("array")->Size(); ++i)
		std::cout<< "array:["<< script.GetRoot()->GetFirstValue("array")->At(i) << ", ";
	std::cout<< "]\n";
	//add an element into sub array
	script.GetRoot()->GetFirstValue("array")->At(3)->Push("add from c++");


	std::cout << script.GetRoot()->GetFirstValue("Root value")<< std::endl;

	LiteConf::Block *bkMain=script.GetRoot()->GetFirstBlock("main");

	LiteConf::IteratorBlocks itBk=bkMain->GetBlocks("Name");

	while(itBk.HasNext()){
		LiteConf::Block *bk=itBk.Get();

		LiteConf::IteratorValues itCicago=bk->GetValues("Chicago");
		while(itCicago.HasNext()){
			std::cout << itCicago.Get()->ToString() <<"\n";
			itCicago.Next();
		}
		
		LiteConf::Value *value=bk->GetFirstValue("value1");
		if(value)
			std::cout << value->ToFloat() << "\n";

		LiteConf::IteratorValues itValues=bk->GetValues("values");
		while(itValues.HasNext()){
			std::cout << itValues.Get()->ToFloat() <<"\n";
			itValues.Next();
		}

		value=bk->GetFirstValue("New York");
		if(value)
			std::cout << value->ToString() << "\n";



		itBk.Next();
	}


	std::cout << script.MakeScript();
	system("PAUSE");
	return 0;
}
