all:
	g++ -o app main.cpp bibutil.cpp -lGLU -lGL -lglut -lGLEW -lm
	./app