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
  string games[] = {
    "Minesweeper",
    "Sokoban    ",
    "- 2048 -   ",
    "  Exit     "
  };
  
  int n_games = 4; //sizeof(games) / sizeof(string *);
  
  WINDOW *menu_win;
  const int width = 17;
  const int height = 11;
  int highlight = 1;
  int choice = 0;
  int max_y, max_x;
  int start_y, start_x, c;
  
  //setlocale(LC_ALL, "");

  // Init and setup ncurses
  initscr();
  clear();
  noecho();
  cbreak();
  curs_set(FALSE);
  
  getmaxyx(stdscr, max_y, max_x);
  
  start_y = (max_y - height) / 2;
  start_x = (max_x - width) / 2;

  menu_win = newwin(height, width, start_y, start_x);
  keypad(menu_win, TRUE);
  refresh();
  print_menu(menu_win, highlight, games, n_games);
  
  while (1)
  {
    c = wgetch(menu_win);
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

    if (!choice)
    {
      print_menu(menu_win, highlight, games, n_games);
    }
    else
    {
      switch (choice)
      {
      case 1:
        MineField::menu(max_y, max_x);
        break;
      
      case 2:
      case 3:
      case 4:
        break;
      
      default:
        break;
      }
    }
  }
  
  c = getch();
  endwin();

  return 0;
}

void print_menu(WINDOW *menu_win, int highlight, string *games, int n_games)
{
  int x = 2;
  int y = 2;

  box(menu_win, 0, 0);
  
  for (int i = 0; i < n_games; ++i)
  {
    if (highlight == i + 1)
    {
      mvwprintw(menu_win, y, x, ">");
      wattron(menu_win, A_REVERSE);
      mvwprintw(menu_win, y, x+2, "%s", games[i].c_str());
      wattroff(menu_win, A_REVERSE);
    }
    else
    {
      mvwprintw(menu_win, y, x, "  %s", games[i].c_str());
    }
    y += 2;
  }
  wrefresh(menu_win);
}
