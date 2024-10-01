CXX=g++
CXXFLAGS = -g -Wall

main:
	$(CXX) $(CXXFLAGS) -o main main.cpp 

clean:
	rm -rf main path robot
