struct Point {
  int i, j;
};

class MineField
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

  MineField(int, int);

  static void menu(int max_y, int max_x);
};
