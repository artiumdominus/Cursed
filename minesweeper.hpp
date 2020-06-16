struct Point {
  int i, j;
};

enum class GameDifficulty
{
  Easy,
  Medium,
  Hard
};

enum class MenuPosition
{
  Height,
  Width,
  Difficulty,
  Start,
  Return
};

struct MineSweeperDTO
{
  int height;
  int width;
  GameDifficulty difficulty;

  MineSweeperDTO() :
    height(0),
    width(0),
    difficulty(GameDifficulty::Easy)
  {}
};

class MineSweeper
{
private:

  int l;
  int c;
  int mines;
  char **cells;
  char **curtain;
  bool **checked;
  int n_checked;
  bool locked;
  Point cursor;

public:

  MineSweeper(int, int);

  static void menu();

private:
  static void print_menu(WINDOW*, MenuPosition, MineSweeperDTO);
};
