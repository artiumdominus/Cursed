#include <locale.h>
#include <ncurses.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include "minesweeper.hpp"

using namespace std;

void print_menu(WINDOW *menu_win, int highlight, string *games, int n_games);

int main(int argc, const char *argv[])
{
  setlocale(LC_ALL, "");

  string games[] = {
    "Minesweeper",
    "Sokoban    ",
    "- 2048 -   ",
    "  Exit     "
  };
  
  int n_games = 4; //sizeof(games) / sizeof(string *);
  
  WINDOW *menu_window;
  const int width = 17;
  const int height = 11;
  int highlight = 1, choice = 0;
  int max_y, max_x, start_y, start_x, c;

  // Init and setup ncurses
  initscr();
  clear();
  noecho();
  cbreak();
  curs_set(FALSE);
  
  getmaxyx(stdscr, max_y, max_x);
  
  start_y = (max_y - height) / 2;
  start_x = (max_x - width) / 2;

  menu_window = newwin(height, width, start_y, start_x);
  keypad(menu_window, TRUE);
  refresh();
  
  print_menu(menu_window, highlight, games, n_games);
  while (1)
  {
    c = wgetch(menu_window);
    switch (c)
    {
      case KEY_UP:
        if (highlight == 1)
        {
          highlight = n_games;
        }
        else
        {
          --highlight;
        }
        break;
      case KEY_DOWN:
        if (highlight == n_games)
        {
          highlight = 1;
        }
        else
        {
          ++highlight;
        }
        break;
      case 10:
        choice = highlight;
        break;
    }

    if (choice)
    {
      switch (choice)
      {
      case 1:
        MineSweeper::menu();
        break;
      
      case 2:
      case 3:
      case 4:
        endwin();
        return 0;
      default:
        break;
      }

      highlight = 1;
      choice = 0;
    }
    
    print_menu(menu_window, highlight, games, n_games);
  }
  
  endwin();
  return 0;
}

void print_menu(WINDOW *menu_window, int highlight, string *games, int n_games)
{
  int x = 2;
  int y = 2;

  box(menu_window, 0, 0);
  
  for (int i = 0; i < n_games; ++i)
  {
    if (highlight == i + 1)
    {
      mvwprintw(menu_window, y, x, "›");
      wattron(menu_window, A_REVERSE);
      mvwprintw(menu_window, y, x+2, "%s", games[i].c_str());
      wattroff(menu_window, A_REVERSE);
    }
    else
    {
      mvwprintw(menu_window, y, x, "  %s", games[i].c_str());
    }
    y += 2;
  }
  wrefresh(menu_window);
}
