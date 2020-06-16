#include <ncurses.h>
#include <iostream>
#include "minesweeper.hpp"

using namespace std;

inline MenuPosition& operator++(MenuPosition& position)
{
  position = MenuPosition(static_cast<int>(position) + 1);
  return position;
}

inline MenuPosition& operator--(MenuPosition& position)
{
  position = MenuPosition(static_cast<int>(position) - 1);
  return position;
}

MineSweeper::MineSweeper(int l, int c)
{
  this->l = l;
  this->c = c;
  cells = new char *[l];
  curtain = new char *[l];
  checked = new bool *[l];

  for (int i = 0; i < l; ++i)
  {
    cells[i] = new char [c];
    curtain[i] = new char [c];
    checked[i] = new bool [c];
    for (int j = 0; j < c; ++j)
    {
      cells[i][j] = ' ';
      curtain[i][j] = '+';
      checked[i][j] = false;
    }

    mines = 0;
    locked = true;
  }
}

void MineSweeper::menu()
{
  WINDOW *menu_window;
  
  int menu_height = 12;
  int menu_width = 20;
  int max_y, max_x;

  getmaxyx(stdscr, max_y, max_x);
  
  int starty = (max_y - menu_height) / 2;
  int startx = (max_x - menu_width) / 2;

  MenuPosition menu_position = MenuPosition::Height;
  MineSweeperDTO dto;

  menu_window = newwin(menu_height, menu_width, starty, startx);
  keypad(menu_window, TRUE);

  MineSweeper::print_menu(menu_window, menu_position, dto);

  int c;
  while(1)
  {
    c = wgetch(menu_window);
    switch (c)
    {
      case KEY_UP:
        --menu_position;
        break;
      case KEY_DOWN:
        ++menu_position;
        break;
      case KEY_LEFT:
        break;
      case KEY_RIGHT:
        break;
      case 10:
        break;
      default:
        break;
    }
    MineSweeper::print_menu(menu_window, menu_position, dto);
  }

  return;
}

void MineSweeper::print_menu(WINDOW *menu_window, MenuPosition menu_position, MineSweeperDTO DTO)
{
  box(menu_window, 0, 0);

  mvwprintw(menu_window, 1, 2, "- Minesweeper -");

  if (menu_position == MenuPosition::Height)
  {
    mvwprintw(menu_window, 3, 3, "height: ");
    wattron(menu_window, A_REVERSE);
    wprintw(menu_window, "< 00 >");
    wattroff(menu_window, A_REVERSE);
  }
  else
  {
    mvwprintw(menu_window, 3, 3, "height: < 00 >");
  }
  

  if (menu_position == MenuPosition::Width)
  {
    mvwprintw(menu_window, 5, 3, "width:  ");
    wattron(menu_window, A_REVERSE);
    wprintw(menu_window, "< 00 >");
    wattroff(menu_window, A_REVERSE);
  }
  else
  {
    mvwprintw(menu_window, 5, 3, "width:  < 00 >");
  }
  
  mvwprintw(menu_window, 7, 3, "difficulty:");
  if (menu_position == MenuPosition::Difficulty)
  {
    wattron(menu_window, A_REVERSE);
    mvwprintw(menu_window, 8, 7, "< medium >");
    wattroff(menu_window, A_REVERSE);
  }
  else
  {
    mvwprintw(menu_window, 8, 7, "< medium >");
  }

  if (menu_position == MenuPosition::Start)
  {
    wattron(menu_window, A_REVERSE);
    mvwprintw(menu_window, 10, 2, "[ start ]");
    wattroff(menu_window, A_REVERSE);
    wprintw(menu_window, " [ << ]");
  }
  else if (menu_position == MenuPosition::Return)
  {
    mvwprintw(menu_window, 10, 2, "[ start ] ");
    wattron(menu_window, A_REVERSE);
    wprintw(menu_window, "[ << ]");
    wattroff(menu_window, A_REVERSE);
  }
  else
  {
    mvwprintw(menu_window, 10, 2, "[ start ] [ << ]");
  }

  wrefresh(menu_window);

  return;
}
