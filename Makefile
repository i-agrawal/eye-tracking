make : main.cpp neural-network/NeuralNetwork.h neural-network/NeuralNetwork.cpp
	g++ main.cpp neural-network/NeuralNetwork.cpp -o eyed

run : make eyed
	./eyed