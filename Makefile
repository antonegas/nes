include .env

SOURCE_FOLDERS=source source/SDL source/mappers

default :
	g++ $(foreach dir,$(SOURCE_FOLDERS),$(wildcard $(dir)/*.cpp)) -o main -I $(LIBRARIES)/include/ -I headers -L $(LIBRARIES)/lib/ -l SDL3 $(FLAGS) -std=c++20

run : default
	./main.exe