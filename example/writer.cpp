#include "../LiteConfig.h"
#include <iostream>

int main(){

	LiteConf::Script script;
	script.GetRoot()->Push("Root value","I'm in...\nroot!!\n\n")
					->Push("main")
						->Push("Name")
                            ->Push("Chicago","Jim")
                            ->Push("Chicago","Other")
                            ->Push("New York","Kim")
                        ->Back()
						->Push("Name")
                            ->Push("value1",1)
                            ->Push("values",0)
                            ->Push("values",1)
                            ->Push("values",2);

	script.GetRoot()->PushArray("Test")
					 ->Push(1)
					 ->Push("String")
					 ->PushArray()
							->Push(1)
							->Push("String sub array")
							->Closure()
					 ->Push("String2")
					 ->Back()
			->Push("int",1);

	std::cout << script.MakeScript();

	return 0;
}
