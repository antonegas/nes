include .env

default : 
	g++ $(wildcard source/*.cpp) $(wildcard source/SDL/*.cpp) $(wildcard source/mappers/*.cpp) -o main -I $(LIBRARIES)/include/ -I headers -L $(LIBRARIES)/lib/ -l SDL3 $(FLAGS) -std=c++20