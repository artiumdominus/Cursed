#include <ncurses.h>
#include <iostream>
#include <math.h>
#include "minesweeper.hpp"

using namespace std;

inline GameDifficulty& operator++(GameDifficulty& difficulty, int)
{
  difficulty = GameDifficulty((static_cast<int>(difficulty) + 1) % 4);
  return difficulty;
}

inline GameDifficulty& operator--(GameDifficulty& difficulty, int)
{
  if (difficulty == GameDifficulty::Easy)
  {
    difficulty = GameDifficulty::Hell;
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
  lines = dto.height;
  columns = dto.width;

  cells = new Cell *[lines];

  for (int i = 0; i < lines; ++i)
  {
    cells[i] = new Cell [columns];

    for (int j = 0; j < columns; ++j)
    {
      cells[i][j].value = ' ';
      cells[i][j].curtain = '+';
      cells[i][j].checked = false;
    }
  }

  const double mines_rate[] = { 0.1, 0.15, 0.2, 0.3 };

  mines = ceil( lines * columns * mines_rate[(int) dto.difficulty] );

  int neighborhood[lines][columns];

  for (int i = 0; i < lines; ++i)
  {
    for (int j = 0; j < columns; ++j)
    {
      neighborhood[i][j] = 0;
    }
  }

  srand(time(NULL));
  for (int n = 1; n <= mines; ++n)
  {
    bool ok = false;
    do
    {
      int i = rand() % lines;
      int j = rand() % columns;

      if (cells[i][j].value != 'o')
      {
        cells[i][j].value = 'o';

        if (i > 0)
        {
          neighborhood[i-1][j]++;

          if (j > 0)
          {
            neighborhood[i-1][j-1]++;
          }

          if (j < columns-1)
          {
            neighborhood[i-1][j+1]++;
          }
        }

        if (i < lines-1)
        {
          neighborhood[i+1][j]++;

          if (j > 0)
          {
            neighborhood[i+1][j-1]++;
          }

          if (j < columns-1)
          {
            neighborhood[i+1][j+1]++;
          }
        }

        if (j > 0)
        {
          neighborhood[i][j-1]++;
        }

        if (j < columns-1)
        {
          neighborhood[i][j+1]++;
        }

        ok = true;
      }
    } while(!ok);
  }

  for (int i = 0; i < lines; ++i)
  {
    for (int j = 0; j < columns; ++j)
    {
      if (neighborhood[i][j] > 0 && cells[i][j].value != 'o')
      {
        cells[i][j].value = '0' + neighborhood[i][j];
      }
    }
  }

  n_checked = 0;
  cursor = Point(lines/2, columns/2);
  locked = false;
}

void MineSweeper::menu()
{
  //WINDOW *menu_window;
  
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

  WINDOW *menu_window = newwin(menu_height, menu_width, starty, startx);
  keypad(menu_window, TRUE);

  MineSweeper::print_menu(menu_window, menu_position, dto);

  int c;
  while (1)
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
    " hell "
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
  MineSweeper minesweeper(dto);

  int game_height = (dto.height * 2) + 1;
  int game_width = (dto.width * 4) + 2;

  int max_y, max_x;

  getmaxyx(stdscr, max_y, max_x);

  int starty = (max_y - game_height) / 2;
  int startx = (max_x - game_width) / 2;

  WINDOW *game_window = newwin(game_height, game_width, starty, startx); // alterar posição
  keypad(game_window, TRUE);

  minesweeper.render(game_window);

  int c;
  while (1)
  {
    c = wgetch(game_window);
    switch (c)
    {
      case KEY_UP:
        minesweeper.move_up();
        break;
      case KEY_DOWN:
        minesweeper.move_down();
        break;
      case KEY_LEFT:
        minesweeper.move_left();
        break;
      case KEY_RIGHT:
        minesweeper.move_right();
        break;
      case 10:
        break;
      case ' ':
        minesweeper.curtain_set();
        break;
    }
    minesweeper.render(game_window);
  }
}

void MineSweeper::render(WINDOW *game_window)
{
  int i, j;

  wmove(game_window, 0, 0);

  wprintw(game_window, "┌───");
  for (j = 1; j < columns; ++j)
  {
    wprintw(game_window, "┬───");
  }
  wprintw(game_window, "┐\n");

  for (i = 0; i < lines; ++i)
  {
    wprintw(game_window, "│");
    for (j = 0; j < columns; ++j)
    {
      wprintw(
        game_window,
        "%c%c%c│",
        (cursor.i == i && cursor.j == j ? '[' : ' '),
        (cells[i][j].checked ? cells[i][j].value : cells[i][j].curtain),
        (cursor.i == i && cursor.j == j ? ']' : ' ')
      );
    }
    wprintw(game_window, "\n");

    if (i < lines-1)
    {
      wprintw(game_window, "├───");
      for (j = 1; j < columns; ++j)
      {
        wprintw(game_window, "┼───");
      }
      wprintw(game_window, "┤\n");
    }
  }

  wprintw(game_window, "└───");
  for (j = 1; j < columns; ++j)
  {
    wprintw(game_window, "┴───");
  }
  wprintw(game_window, "┘\n");
}

void MineSweeper::move_up()
{
  if (cursor.i == 0)
  {
    cursor.i = lines - 1;
  }
  else
  {
    cursor.i--;
  }
}

void MineSweeper::move_down()
{
  if (cursor.i == lines - 1)
  {
    cursor.i = 0;
  }
  else
  {
    cursor.i++;
  }
}

void MineSweeper::move_left()
{
  if (cursor.j == 0)
  {
    cursor.j = columns - 1;
  }
  else
  {
    cursor.j--;
  }
}

void MineSweeper::move_right()
{
  if (cursor.j == columns - 1)
  {
    cursor.j = 0;
  }
  else
  {
    cursor.j++;
  }
}

void MineSweeper::curtain_set()
{
  switch (cells[cursor.i][cursor.j].curtain)
  {
    case '+':
      cells[cursor.i][cursor.j].curtain = '!';
      break;
    case '!':
      cells[cursor.i][cursor.j].curtain = '?';
      break;
    case '?':
      cells[cursor.i][cursor.j].curtain = '+';
      break;
  }
}
