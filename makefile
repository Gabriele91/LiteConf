UNAME := $(shell uname)

all: release debug example


example: release example/writer.cpp example/reader.cpp LiteConfig.h libLiteConfig.a
ifeq ($(UNAME), Linux)
	#linux
	gcc -Wall example/writer.cpp libLiteConfig.a -O2 -o writer -lstdc++
	gcc -Wall example/reader.cpp libLiteConfig.a -O2 -o reader -lstdc++
	chmod +x writer
	chmod +x reader
else
ifeq ($(UNAME), Darwin)
	#mac os x
	gcc -Wall example/writer.cpp libLiteConfig.a -O2 -o writer -lstdc++
	gcc -Wall example/reader.cpp libLiteConfig.a -O2 -o reader -lstdc++
	chmod +x writer
	chmod +x reader
else
	#windows
	gcc -Wall example/writer.cpp libLiteConfig.a -O2 -o writer.exe -lstdc++
	gcc -Wall example/reader.cpp libLiteConfig.a -O2 -o reader.exe -lstdc++
endif
endif

release: LiteConfig.o 
	ar  rcs libLiteConfig.a      LiteConfig.o
	rm -rf *o	
	
debug: LiteConfig_d.o 
	ar  rcs libLiteConfig_d.a      LiteConfig_d.o
	rm -rf *o
	
LiteConfig.o: LiteConfig.h LiteConfig.cpp
	g++ -c LiteConfig.cpp -O2 -o LiteConfig.o
	
LiteConfig_d.o: LiteConfig.h LiteConfig.cpp
	g++ -c LiteConfig.cpp -g -o LiteConfig_d.o
	
clean:
	rm -rf *o