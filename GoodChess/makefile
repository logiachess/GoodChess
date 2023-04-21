SRCS=$(wildcard *.cpp)
NAME=GoodChess

all:
	g++ -oFast $(SRCS) -o $(NAME)
	x86_64-w64-mingw32-g++ -oFast $(SRCS) -o $(NAME)

debug:
	g++ $(SRCS) -o $(NAME)
	x86_64-w64-mingw32-g++ $(SRCS) -o $(NAME)