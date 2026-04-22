#ifndef SERVER_H
#define SERVER_H

#include <cstdlib>
#include <iostream>
#include <vector>
#include <queue>

/*
 * You may need to define some global variables for the information of the game map here.
 * Although we don't encourage to use global variables in real cpp projects, you may have to use them because the use of
 * class is not taught yet. However, if you are member of A-class or have learnt the use of cpp class, member functions,
 * etc., you're free to modify this structure.
 */
int rows;         // The count of rows of the game map. You MUST NOT modify its name.
int columns;      // The count of columns of the game map. You MUST NOT modify its name.
int total_mines;  // The count of mines of the game map. You MUST NOT modify its name. You should initialize this
                  // variable in function InitMap. It will be used in the advanced task.
int game_state;  // The state of the game, 0 for continuing, 1 for winning, -1 for losing. You MUST NOT modify its name.

// Internal game state
static bool is_mine[35][35];
static int adj_count[35][35];
static bool visited[35][35];
static bool marked[35][35];
static bool wrong_mark[35][35];
static int visited_nonmine_count;
static int correctly_marked_mines;

static const int dr[8] = {-1,-1,-1,0,0,1,1,1};
static const int dc[8] = {-1,0,1,-1,1,-1,0,1};

static bool in_bounds(int r, int c) { return r >= 0 && r < rows && c >= 0 && c < columns; }

static void compute_adj() {
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      int cnt = 0;
      for (int k = 0; k < 8; ++k) {
        int nr = i + dr[k], nc = j + dc[k];
        if (in_bounds(nr, nc) && is_mine[nr][nc]) cnt++;
      }
      adj_count[i][j] = cnt;
    }
  }
}

static void visit_nonmine(int r, int c) {
  if (!in_bounds(r,c)) return;
  if (visited[r][c]) return;
  if (marked[r][c]) return;
  if (is_mine[r][c]) return; // safety
  // BFS expansion for zeros
  std::queue<std::pair<int,int>> q;
  q.emplace(r,c);
  while (!q.empty()) {
    auto [cr, cc] = q.front(); q.pop();
    if (visited[cr][cc]) continue;
    if (marked[cr][cc]) continue;
    if (is_mine[cr][cc]) continue;
    visited[cr][cc] = true;
    visited_nonmine_count++;
    if (adj_count[cr][cc] == 0) {
      for (int k = 0; k < 8; ++k) {
        int nr = cr + dr[k], nc = cc + dc[k];
        if (in_bounds(nr, nc) && !visited[nr][nc] && !marked[nr][nc] && !is_mine[nr][nc]) {
          q.emplace(nr, nc);
        }
      }
    }
  }
}

static void check_victory() {
  int nonmine_total = rows * columns - total_mines;
  if (visited_nonmine_count == nonmine_total) {
    game_state = 1;
  }
}

/**
 * @brief The definition of function InitMap()
 */
void InitMap() {
  std::cin >> rows >> columns;
  total_mines = 0;
  game_state = 0;
  visited_nonmine_count = 0;
  correctly_marked_mines = 0;
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      is_mine[i][j] = false;
      visited[i][j] = false;
      marked[i][j] = false;
      wrong_mark[i][j] = false;
      adj_count[i][j] = 0;
    }
  }
  for (int i = 0; i < rows; ++i) {
    std::string line; std::cin >> line;
    for (int j = 0; j < columns; ++j) {
      is_mine[i][j] = (line[j] == 'X');
      if (is_mine[i][j]) total_mines++;
    }
  }
  compute_adj();
}

/**
 * @brief VisitBlock
 */
void VisitBlock(int r, int c) {
  if (!in_bounds(r,c)) return;
  if (visited[r][c] || marked[r][c]) { game_state = 0; return; }
  if (is_mine[r][c]) {
    visited[r][c] = true; // reveal the mine
    game_state = -1;
    return;
  }
  visit_nonmine(r, c);
  check_victory();
  if (game_state == 0) game_state = 0;
}

/**
 * @brief MarkMine
 */
void MarkMine(int r, int c) {
  if (!in_bounds(r,c)) return;
  if (visited[r][c] || marked[r][c]) { game_state = 0; return; }
  if (is_mine[r][c]) {
    marked[r][c] = true;
    correctly_marked_mines++;
    check_victory();
    if (game_state == 0) game_state = 0;
  } else {
    wrong_mark[r][c] = true; // show as X
    game_state = -1;
  }
}

/**
 * @brief AutoExplore
 */
void AutoExplore(int r, int c) {
  if (!in_bounds(r,c)) return;
  if (!visited[r][c]) return;
  if (is_mine[r][c]) return; // must be visited non-mine cell
  int need = adj_count[r][c];
  int marked_correct = 0;
  for (int k = 0; k < 8; ++k) {
    int nr = r + dr[k], nc = c + dc[k];
    if (in_bounds(nr,nc) && marked[nr][nc] && is_mine[nr][nc]) marked_correct++;
  }
  if (marked_correct != need) return;
  // visit all non-mine unvisited unmarked neighbors
  for (int k = 0; k < 8; ++k) {
    int nr = r + dr[k], nc = c + dc[k];
    if (in_bounds(nr,nc) && !is_mine[nr][nc] && !visited[nr][nc] && !marked[nr][nc]) {
      visit_nonmine(nr, nc);
    }
  }
  check_victory();
}

/**
 * @brief ExitGame()
 */
void ExitGame() {
  if (game_state == 1) {
    std::cout << "YOU WIN!" << std::endl;
    std::cout << visited_nonmine_count << " " << total_mines << std::endl;
  } else {
    std::cout << "GAME OVER!" << std::endl;
    std::cout << visited_nonmine_count << " " << correctly_marked_mines << std::endl;
  }
  exit(0);  // Exit the game immediately
}

/**
 * @brief PrintMap()
 */
void PrintMap() {
  bool victory = (game_state == 1);
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      char ch = '?';
      if (victory) {
        if (is_mine[i][j]) ch = '@';
        else ch = char('0' + adj_count[i][j]);
      } else {
        if (wrong_mark[i][j]) {
          ch = 'X';
        } else if (marked[i][j]) {
          ch = '@';
        } else if (visited[i][j]) {
          if (is_mine[i][j]) ch = 'X';
          else ch = char('0' + adj_count[i][j]);
        } else {
          ch = '?';
        }
      }
      std::cout << ch;
    }
    std::cout << std::endl;
  }
}

#endif
