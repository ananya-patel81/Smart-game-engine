CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -Iinclude

.PHONY: all clean test

all: connect4 tictactoe sudoku test_connect4 test_tictactoe test_sudoku

connect4: src/main_connect4.cpp src/Connect4.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

tictactoe: src/main_tictactoe.cpp src/TicTacToe.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

sudoku: src/main_sudoku.cpp src/Sudoku.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

test_connect4: tests/test_connect4.cpp src/Connect4.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

test_tictactoe: tests/test_tictactoe.cpp src/TicTacToe.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

test_sudoku: tests/test_sudoku.cpp src/Sudoku.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

test: test_connect4 test_tictactoe test_sudoku
	./test_connect4
	./test_tictactoe
	./test_sudoku

clean:
	rm -f connect4 tictactoe sudoku test_connect4 test_tictactoe test_sudoku
