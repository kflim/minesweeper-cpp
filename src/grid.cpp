#include <iostream>

#include "grid.h"

void Cell::setMine() {
  this->isMine = true;
}

void Cell::setRevealed() {
  this->isRevealed = true;
}

void Cell::toggleFlagged() {
  this->isFlagged = !this->isFlagged;
}

void Cell::setAdjacentMines(int adjacentMines) {
  this->adjacentMines = adjacentMines;
}

bool Cell::getIsMine() {
  return this->isMine;
}

bool Cell::getIsRevealed() {
  return this->isRevealed;
}

bool Cell::getIsFlagged() {
  return this->isFlagged;
}

int Cell::getAdjacentMines() {
  return this->adjacentMines;
}

void Grid::generateMines() {
  for (int i = 0; i < this->mineCount; i++) {
    int row = rand() % this->rows;
    int col = rand() % this->cols;
    if (this->grid[row][col].getIsMine()) {
      i--;
    } else {
      this->grid[row][col].setMine();
      std::cout << "Mine at " << row << ", " << col << std::endl;
    }
  }
}

void Grid::calculateAdjacentMines() {
  for (int i = 0; i < this->rows; i++) {
    for (int j = 0; j < this->cols; j++) {
      this->countAdjacentMines(i, j);
    }
  }
}

void Grid::countAdjacentMines(int row, int col) {
  int count = 0;
  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) {
      if (row + i >= 0 && row + i < this->rows && col + j >= 0 && col + j < this->cols) {
        if (this->grid[row + i][col + j].getIsMine()) {
          count++;
        }
      }
    }
  }
  this->grid[row][col].setAdjacentMines(count);
}

void Grid::revealCell(int row, int col) {
  // Boundary check
  if (row < 0 || row >= rows || col < 0 || col >= cols) {
      return;
  }

  Cell& cell = this->getCell(row, col);

  // Stop if cell is already revealed or contains a mine
  if (cell.getIsRevealed() || cell.getIsMine()) {
      return;
  }

  // Reveal the current cell
  cell.setRevealed();

  // If there are adjacent mines, stop further propagation
  if (cell.getAdjacentMines() > 0) {
      return;
  }

  // If no adjacent mines, reveal neighboring cells
  for (int dr = -1; dr <= 1; ++dr) {
    for (int dc = -1; dc <= 1; ++dc) {
      if ((dr == 0 || dc == 0) && (dr != 0 || dc != 0)) { // Exclude diagonals and self
        revealCell(row + dr, col + dc);
      }
    }
  }
}

void Grid::printGrid() {
  for (int i = 0; i < this->rows; i++) {
    for (int j = 0; j < this->cols; j++) {
      if (this->grid[i][j].getIsMine()) {
        std::cout << "X ";
      } else {
        std::cout << this->grid[i][j].getAdjacentMines() << " ";
      }
    }
    std::cout << std::endl;
  }
}

Cell& Grid::getCell(int row, int col) {
    return this->grid[row][col];
}

