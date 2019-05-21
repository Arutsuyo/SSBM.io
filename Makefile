CXXFLAGS= -W -Wall -std=c++14

SOURCE_DIR=SSBM.io

Types.o:
	gcc $(CXXFLAGS) -c -o $(SOURCE_DIR)/Types.o $(SOURCE_DIR)/Types.h

Config.o: Types.o
	gcc $(CXXFLAGS) -c -o $(SOURCE_DIR)/Config.o $(SOURCE_DIR)/Config.cpp $(SOURCE_DIR)/Types.o

Controller.o: Types.o
	gcc $(CXXFLAGS) -c -o $(SOURCE_DIR)/Controller.o $(SOURCE_DIR)/Controller.cpp $(SOURCE_DIR)/Types.o

Handler.o: Types.o
	gcc $(CXXFLAGS) -c -o $(SOURCE_DIR)/Handler.o $(SOURCE_DIR)/Handler.cpp $(SOURCE_DIR)/Types.o


OBJECTS=$(SOURCE_DIR)/Types.o $(SOURCE_DIR)/Config.o $(SOURCE_DIR)/Controller.o $(SOURCE_DIR)/Handler.o

clean:
	rm $(SOURCE_DIR)/*.o ssbm

PROXY:
ssbm: $(OBJECTS)
	gcc -o ssbm $(CXXFLAGS) $(OBJECTS)
	