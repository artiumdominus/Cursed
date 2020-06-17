#include <ncurses.h>
#include <iostream>
#include "minesweeper.hpp"

using namespace std;

inline GameDifficulty& operator++(GameDifficulty& difficulty, int)
{
  difficulty = GameDifficulty((static_cast<int>(difficulty) + 1) % 3);
  return difficulty;
}

inline GameDifficulty& operator--(GameDifficulty& difficulty, int)
{
  if (difficulty == GameDifficulty::Easy)
  {
    difficulty = GameDifficulty::Hard;
    return difficulty;
  }

  difficulty = GameDifficulty(static_cast<int>(difficulty) - 1);
  return difficulty;
}

inline MenuPosition& operator++(MenuPosition& position)
{
  position = MenuPosition((static_cast<int>(position) + 1) % 5);
  return position;
}

inline MenuPosition& operator--(MenuPosition& position)
{
  if (position == MenuPosition::Height)
  {
    position = MenuPosition::Return;
    return position;
  }

  position = MenuPosition(static_cast<int>(position) - 1);
  return position;
}

int MineSweeper::min_height = 2;
int MineSweeper::min_width = 2;
int MineSweeper::max_height = 2;
int MineSweeper::max_width = 2;

MineSweeper::MineSweeper(MineSweeperDTO dto)
{
  l = dto.height;
  c = dto.width;
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

  max_height = (max_y - 1) / 2;
  max_width  = (max_x - 1) / 4;

  dto.height = max_height;
  dto.width = max_width;

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
        switch (menu_position)
        {
          case MenuPosition::Height:
            if (dto.height > min_height)
            {
              dto.height--;
            }
            break;
          case MenuPosition::Width:
            if (dto.width > min_width)
            {
              dto.width--;
            }
            break;
          case MenuPosition::Difficulty:
            dto.difficulty--;
            break;
          case MenuPosition::Start:
            menu_position = MenuPosition::Return;
            break;
          case MenuPosition::Return:
            menu_position = MenuPosition::Start;
            break;
        }
        break;
      case KEY_RIGHT:
        switch (menu_position)
        {
          case MenuPosition::Height:
            if (dto.height < max_height)
            {
              dto.height++;
            }
            break;
          case MenuPosition::Width:
            if (dto.width < max_width)
            {
              dto.width++;
            }
            break;
          case MenuPosition::Difficulty:
            dto.difficulty++;
            break;
          case MenuPosition::Start:
            menu_position = MenuPosition::Return;
            break;
          case MenuPosition::Return:
            menu_position = MenuPosition::Start;
            break;
        }
        break;
      case 10:
        switch (menu_position)
        {
        case MenuPosition::Start:
          MineSweeper::play(dto);
          break;
        case MenuPosition::Return:
          MineSweeper::destroy_menu(menu_window);
          return;
          break;
        default:
          break;
        }
        break;
      default:
        break;
    }
    MineSweeper::print_menu(menu_window, menu_position, dto);
  }

  return;
}

void MineSweeper::print_menu(WINDOW *menu_window, MenuPosition menu_position, MineSweeperDTO dto)
{
  const string difficulty_text[] = {
    " easy ",
    "medium",
    " hard ",
  };

  box(menu_window, 0, 0);

  mvwprintw(menu_window, 1, 2, "- Minesweeper -");

  if (menu_position == MenuPosition::Height)
  {
    mvwprintw(menu_window, 3, 3, "height: ");
    wattron(menu_window, A_REVERSE);
    wprintw(
      menu_window,
      "%c %02d %c",
      (dto.height > min_height ? '<' : '['),
      dto.height,
      (dto.height < max_height ? '>' : ']')
    );
    wattroff(menu_window, A_REVERSE);
  }
  else
  {
    mvwprintw(
      menu_window,
      3,
      3,
      "height: %c %02d %c",
      (dto.height > min_height ? '<' : '['),
      dto.height,
      (dto.height < max_height ? '>' : ']')
    );
  }
  
  if (menu_position == MenuPosition::Width)
  {
    mvwprintw(menu_window, 5, 3, "width:  ");
    wattron(menu_window, A_REVERSE);
    wprintw(
      menu_window,
      "%c %02d %c",
      (dto.width > min_width ? '<' : '['),
      dto.width,
      (dto.width < max_width ? '>' : ']')
    );
    wattroff(menu_window, A_REVERSE);
  }
  else
  {
    mvwprintw(
      menu_window,
      5,
      3,
      "width:  %c %02d %c",
      (dto.width > min_width ? '<' : '['),
      dto.width,
      (dto.width < max_width ? '>' : ']')
    );
  }
  
  mvwprintw(menu_window, 7, 3, "difficulty:");
  if (menu_position == MenuPosition::Difficulty)
  {
    wattron(menu_window, A_REVERSE);
    mvwprintw(menu_window, 8, 7, "< %s >", difficulty_text[(int) dto.difficulty].c_str());
    wattroff(menu_window, A_REVERSE);
  }
  else
  {
    mvwprintw(menu_window, 8, 7, "< %s >", difficulty_text[(int) dto.difficulty].c_str());
  }

  if (menu_position == MenuPosition::Start)
  {
    wattron(menu_window, A_REVERSE);
    mvwprintw(menu_window, 10, 2, "[ start ]");
    wattroff(menu_window, A_REVERSE);
    wprintw(menu_window, "  [ « ]");
  }
  else if (menu_position == MenuPosition::Return)
  {
    mvwprintw(menu_window, 10, 2, "[ start ]  ");
    wattron(menu_window, A_REVERSE);
    wprintw(menu_window, "[ « ]");
    wattroff(menu_window, A_REVERSE);
  }
  else
  {
    mvwprintw(menu_window, 10, 2, "[ start ]  [ « ]");
  }

  wrefresh(menu_window);

  return;
}

void MineSweeper::destroy_menu(WINDOW *menu_window)
{
  wborder(menu_window, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
  wrefresh(menu_window);
  delwin(menu_window);
}

void MineSweeper::play(MineSweeperDTO dto)
{
  MineSweeper minesweeper = MineSweeper(dto);
}
