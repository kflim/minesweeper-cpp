#ifndef MINESWEEPER_GRID
#define MINESWEEPER_GRID

#include <cstdlib>
#include <vector>

class Cell {
private:
  bool isMine;
  bool isRevealed;
  bool isFlagged;
  int adjacentMines;
public:
  Cell() : isMine(false), isRevealed(false), isFlagged(false), adjacentMines(0) {}

  void setMine();
  void setRevealed();
  void toggleFlagged();
  void setAdjacentMines(int);

  bool getIsMine();
  bool getIsRevealed();
  bool getIsFlagged();
  int getAdjacentMines();
};

class Grid {
private:
  int rows;
  int cols;
  int cellSize;
  int mineCount;
  std::vector<std::vector<Cell>> grid;

public:
  Grid(int rows, int cols, int cellSize, int mineCount) : rows(rows), cols(cols), cellSize(cellSize), mineCount(mineCount) {
    this->grid = std::vector<std::vector<Cell>>(rows, std::vector<Cell>(cols));
    this->generateMines();
    this->calculateAdjacentMines();
  }

  void generateMines();
  void calculateAdjacentMines();
  void countAdjacentMines(int, int);
  void revealCell(int, int);
  void printGrid();
  Cell& getCell(int row, int col);
};

#endif