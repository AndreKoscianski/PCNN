opt1: main-pcnn.o vector.o  otimizador.o mlp.o slp.o
	g++ -o opt1 main-pcnn.o vector.o  otimizador.o mlp.o slp.o

opt2: main-pcrbnn.o vector.o  otimizador.o pcrbnn.o 
	g++ -o opt2 main-pcrbnn.o vector.o  otimizador.o pcrbnn.o

main-pcnn: main-pcnn.cpp
	g++ -c main-pcnn.cpp

main-pcrbnn.o: main-pcrbnn.cpp
	g++ -c main-pcrbnn.cpp

vector.o: vector.cpp
	g++ -c vector.cpp

mlp.o: mlp.cpp
	g++ -c mlp.cpp

slp.o: slp.cpp
	g++ -c slp.cpp

pcrbnn.o: pcrbnn.cpp
	g++ -c pcrbnn.cpp

otimizador.o: otimizador.cpp
	g++ -c otimizador.cpp


