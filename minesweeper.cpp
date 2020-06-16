#include <ncurses.h>
#include "minesweeper.hpp"

using namespace std;

MineField::MineField(int l, int c)
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

void MineField::menu(int max_y, int max_x)
{
  WINDOW *menu_window;
  
  int height = 12;
  int width = 20;
  
  int starty = (max_y - height) / 2;
  int startx = (max_x - width) / 2;
  
  menu_window = newwin(height, width, starty, startx);
  box(menu_window, 0, 0);

  mvwprintw(menu_window, 1, 2, "- Minesweeper -");

  mvwprintw(menu_window, 3, 3, "width:  < 00 >");
  mvwprintw(menu_window, 5, 3, "height: < 00 >");
  mvwprintw(menu_window, 7, 3, "difficulty:");
  mvwprintw(menu_window, 8, 7, "< medium >");
  mvwprintw(menu_window, 10, 2, "[ start ] [ << ]");

  wrefresh(menu_window);
  
  return;
}

