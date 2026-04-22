#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <utility>
#include <vector>

extern int rows;         // The count of rows of the game map.
extern int columns;      // The count of columns of the game map.
extern int total_mines;  // The count of mines of the game map.

// You MUST NOT use any other external variables except for rows, columns and total_mines.

/**
 * @brief The definition of function Execute(int, int, bool)
 */
void Execute(int r, int c, int type);

static std::vector<std::string> cur_map;

/**
 * @brief The definition of function InitGame()
 */
void InitGame() {
  cur_map.clear();
  int first_row, first_column;
  std::cin >> first_row >> first_column;
  Execute(first_row, first_column, 0);
}

/**
 * @brief The definition of function ReadMap()
 */
void ReadMap() {
  cur_map.clear();
  for (int i = 0; i < rows; ++i) {
    std::string line; std::cin >> line;
    cur_map.push_back(line);
  }
}

/**
 * @brief The definition of function Decide()
 */
void Decide() {
  // Simple baseline: pick first unknown, try visit; if any visited cell has number equal to unknown neighbors, mark them.
  // 1) Try marking obvious mines
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      char ch = cur_map[i][j];
      if (ch >= '1' && ch <= '8') {
        int num = ch - '0';
        int unknown = 0;
        int marked_count = 0;
        for (int dr = -1; dr <= 1; ++dr) {
          for (int dc = -1; dc <= 1; ++dc) {
            if (dr == 0 && dc == 0) continue;
            int nr = i + dr, nc = j + dc;
            if (nr < 0 || nr >= rows || nc < 0 || nc >= columns) continue;
            if (cur_map[nr][nc] == '?') unknown++;
            else if (cur_map[nr][nc] == '@') marked_count++;
          }
        }
        if (unknown > 0 && (num - marked_count) == unknown) {
          for (int dr = -1; dr <= 1; ++dr) {
            for (int dc = -1; dc <= 1; ++dc) {
              if (dr == 0 && dc == 0) continue;
              int nr = i + dr, nc = j + dc;
              if (nr < 0 || nr >= rows || nc < 0 || nc >= columns) continue;
              if (cur_map[nr][nc] == '?') { Execute(nr, nc, 1); return; }
            }
          }
        }
      }
    }
  }
  // 2) Try auto-explore where marked neighbors equal number (and there is at least one unknown neighbor)
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      char ch = cur_map[i][j];
      if ((ch >= '1' && ch <= '8') || ch == '0') {
        int num = (ch == '0') ? 0 : ch - '0';
        int marked_mines = 0, unknown = 0;
        for (int dr = -1; dr <= 1; ++dr) {
          for (int dc = -1; dc <= 1; ++dc) {
            if (dr == 0 && dc == 0) continue;
            int nr = i + dr, nc = j + dc;
            if (nr < 0 || nr >= rows || nc < 0 || nc >= columns) continue;
            if (cur_map[nr][nc] == '@') marked_mines++;
            else if (cur_map[nr][nc] == '?') unknown++;
          }
        }
        if (unknown > 0 && marked_mines == num) { Execute(i, j, 2); return; }
      }
    }
  }
  // 3) Prefer visiting a '?' adjacent to any '0' cell
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      if (cur_map[i][j] == '0') {
        for (int dr = -1; dr <= 1; ++dr) {
          for (int dc = -1; dc <= 1; ++dc) {
            if (dr == 0 && dc == 0) continue;
            int nr = i + dr, nc = j + dc;
            if (nr < 0 || nr >= rows || nc < 0 || nc >= columns) continue;
            if (cur_map[nr][nc] == '?') { Execute(nr, nc, 0); return; }
          }
        }
      }
    }
  }
  // 4) Otherwise visit first unknown
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      if (cur_map[i][j] == '?') { Execute(i, j, 0); return; }
    }
  }
  // 5) Fallback: ensure we call Execute exactly once per Decide
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      if (cur_map[i][j] >= '0' && cur_map[i][j] <= '8') { Execute(i, j, 2); return; }
    }
  }
  Execute(0, 0, 2);
}

#endif
