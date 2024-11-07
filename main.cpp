#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <stdio.h>

#include "./src/grid.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int GRID_ROWS = 9;
const int GRID_COLS = 9;
const int CELL_SIZE = 50;

void renderGame(SDL_Renderer*, TTF_Font*, Grid&);
void renderGrid(SDL_Renderer*, TTF_Font*, Grid&);
void renderCellContents(SDL_Renderer*, TTF_Font*, Grid&, int, int);
bool handleMouseClick(SDL_Event, Grid&);

int main() {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    printf("Error initializing SDL: %s\n", SDL_GetError());
  }

  SDL_Window* window = SDL_CreateWindow("Dual Horizon", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
  if (!window) {
    std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return -1;
  }

  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    std::cout << "Failed to create renderer: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return -1;
  }

  if (TTF_Init() == -1) {
    std::cout << "Failed to initialize TTF: " << TTF_GetError() << std::endl;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return -1;
  }

  TTF_Font* font = TTF_OpenFont("assets/arial.ttf", 24);
  if (!font) {
    std::cout << "Failed to load font: " << TTF_GetError() << std::endl;
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return -1;
  }

  SDL_Event windowEvent;
  Grid grid(GRID_ROWS, GRID_COLS, CELL_SIZE, 10);
  while (1) {
    if (SDL_PollEvent(&windowEvent)) {
      if (SDL_QUIT == windowEvent.type) {
        break;
      }
    }

    renderGame(renderer, font, grid);
    if (handleMouseClick(windowEvent, grid)) {
      break;
    }
  }

  TTF_CloseFont(font);
  TTF_Quit();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}

void renderGame(SDL_Renderer* renderer, TTF_Font* font, Grid& grid) {
  renderGrid(renderer, font, grid);
  SDL_RenderPresent(renderer);
}

void renderGrid(SDL_Renderer* renderer, TTF_Font* font, Grid& grid) {
  int gridWidth = GRID_COLS * CELL_SIZE;
  int gridHeight = GRID_ROWS * CELL_SIZE;

  int startX = (WINDOW_WIDTH - gridWidth) / 2;
  int startY = (WINDOW_HEIGHT - gridHeight) / 2;

  for (int i = 0; i < GRID_ROWS; i++) {
    for (int j = 0; j < GRID_COLS; j++) {
      renderCellContents(renderer, font, grid, i, j);
      SDL_Rect cell = {startX + j * CELL_SIZE, startY + i * CELL_SIZE, CELL_SIZE, CELL_SIZE};
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      SDL_RenderDrawRect(renderer, &cell);
    }
  }
}

void renderCellContents(SDL_Renderer* renderer, TTF_Font* font, Grid& grid, int row, int col) {
  int gridWidth = GRID_COLS * CELL_SIZE;
  int gridHeight = GRID_ROWS * CELL_SIZE;

  int startX = (WINDOW_WIDTH - gridWidth) / 2;
  int startY = (WINDOW_HEIGHT - gridHeight) / 2;

  Cell cell = grid.getCell(row, col);
  SDL_Rect cellRect = {startX + col * CELL_SIZE, startY + row * CELL_SIZE, CELL_SIZE, CELL_SIZE};

  if (cell.getIsRevealed()) {
    if (cell.getIsMine()) {
      // Dark color for mines
      SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // Dark gray
      SDL_RenderFillRect(renderer, &cellRect);
    } else {
      int adjacentMines = cell.getAdjacentMines();
      if (adjacentMines > 0) {
        // Draw light gray background for cells with adjacent mines
        SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255); // Light gray
        SDL_RenderFillRect(renderer, &cellRect);

        // Render the number of adjacent mines
        std::string mineCount = std::to_string(adjacentMines);
        SDL_Color color = {0, 0, 0, 255};

        switch (adjacentMines) {
          case 1: color = {0, 0, 255, 255}; break;       // Blue
          case 2: color = {0, 255, 0, 255}; break;       // Green
          case 3: color = {255, 0, 0, 255}; break;       // Red
          case 4: color = {0, 0, 128, 255}; break;       // Dark Blue
          case 5: color = {128, 0, 0, 255}; break;       // Dark Red
          case 6: color = {0, 128, 128, 255}; break;     // Teal
          case 7: color = {0, 0, 0, 255}; break;         // Black
          case 8: color = {128, 128, 128, 255}; break;   // Gray
        }

        // Render the text surface
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, mineCount.c_str(), color);
        if (textSurface) {
          SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
          if (textTexture) {
            SDL_Rect textRect = {cellRect.x + CELL_SIZE / 2 - textSurface->w / 2, cellRect.y + CELL_SIZE / 2 - textSurface->h / 2, textSurface->w, textSurface->h};
            SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
            SDL_DestroyTexture(textTexture);
          }
          SDL_FreeSurface(textSurface);
        }
      } else {
        // Light gray background for empty revealed cells (no mines nearby)
        SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255); // Lighter gray
        SDL_RenderFillRect(renderer, &cellRect);
      }
    }
  } else {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &cellRect);
  }
}

bool handleMouseClick(SDL_Event event, Grid& grid) {
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    int x, y;
    SDL_GetMouseState(&x, &y);

    int gridWidth = GRID_COLS * CELL_SIZE;
    int gridHeight = GRID_ROWS * CELL_SIZE;
    int startX = (WINDOW_WIDTH - gridWidth) / 2;
    int startY = (WINDOW_HEIGHT - gridHeight) / 2;

    int col = (x - startX) / CELL_SIZE;
    int row = (y - startY) / CELL_SIZE;

    if (row >= 0 && row < GRID_ROWS && col >= 0 && col < GRID_COLS) {
      grid.revealCell(row, col); // Reveal the cell on click
      if (grid.getCell(row, col).getIsMine()) {
        std::cout << "Game over!" << std::endl;
        return true;  // End the game
      }
    }
  }
  return false;
}

