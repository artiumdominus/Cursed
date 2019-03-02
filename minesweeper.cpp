#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <cstdlib>
#include <ctime>
#include <list>
#include <string>

using namespace std;

void init_attr(struct termios *old_attr,struct termios *new_attr) {
    tcgetattr(0,&*old_attr);

    *new_attr=*old_attr;
    new_attr->c_lflag &=~ICANON;
    new_attr->c_cc[VMIN]=1;
    new_attr->c_cc[VTIME]=0;
}

int getch() {
   struct termios old_attr, new_attr;
   int c;
   init_attr(&old_attr,&new_attr);
   new_attr.c_lflag &=~ECHO;
   tcsetattr(STDIN_FILENO,TCSANOW,&new_attr);
   c=getchar();
   tcsetattr(STDIN_FILENO,TCSANOW,&old_attr);
   return c;
}

int getche() {
   struct termios old_attr, new_attr;
   int c;
   init_attr(&old_attr,&new_attr);
   new_attr.c_lflag &=ECHO;
   tcsetattr(STDIN_FILENO,TCSANOW,&new_attr);
   c=getchar();
   tcsetattr(STDIN_FILENO,TCSANOW,&old_attr);
   return c;
}

#define UP 1001
#define DOWN 1002
#define LEFT 1003
#define RIGHT 1004

int tecla() {
    char c;
    c = getch();
    if (c == 27) {
        getch();
        c = getch();
        switch (c) {
            case 'A':
                return UP;
            case 'B':
                return DOWN;
            case 'C':
                return RIGHT;
            case 'D':
                return LEFT;
        }
    }
    return c;
}

#define ENTER '\n'
#define SPACE ' '

class shit_happened {
    string shit;

public:
    shit_happened(string shit) {
        this->shit = shit;
    }

    string get_shit() {
        return shit;
    }
};

using namespace std;

struct Tuple {
    int i;
    int j;
};

Tuple Tuple_(int i, int j) {
    Tuple tuple;
    tuple.i = i;
    tuple.j = j;
    return tuple;
}

class Minefield {
private:
    int l;
    int c;
    int minas;
    char **cells;
    char **curtain;
    bool **checked;
    int n_checked;
    bool locked;
    Tuple cursor;

public:

    Minefield(int l, int c) {
        this->l = l;
        this->c = c;
        cells = new char *[l];
        curtain = new char *[l];
        checked = new bool *[l];
        for (int i = 0; i < l; ++i) {
            cells[i] = new char [c];
            curtain[i] = new char[c];
            checked[i] = new bool [c];
            for (int j = 0; j < c; ++j) {
                cells[i][j] = ' ';
                curtain[i][j] = '+';
                checked[i][j] = false;
            }
        }

        minas = 0;
        locked = true;

        cout << "Minefield generated" << endl;
    }

    void clear() {
        for (int i = 0; i < l; ++i) {
            for (int j = 0; j < c; ++j) {
                cells[i][j] = ' ';
                curtain[i][j] = '+';
                checked[i][j] = false;
            }
        }

        cout << "Minefield cleared" << endl;
    }

    void init(float danger) {
        minas = l * c * danger;
        int i, j, a, b;
        bool ok;

        this->clear();

        srand(time(NULL));
        for (int n = 1; n <= minas; ++n) {
            ok = false;
            do {
                i = rand() % l;
                j = rand() % c;
                if (cells[i][j] != 'o') {
                    cells[i][j] = 'o';
                    ok = true;
                }
            } while(!ok);
        }

        int neighborhood;
        for (i = 0; i < l; ++i) {
            for (j = 0; j < c; ++j) {
                if (cells[i][j] != 'o') {
                    neighborhood = 0;
                    for (a = i - 1; a <= i + 1; ++a) {
                        for (b = j - 1; b <= j + 1; ++b) {
                            if((a >= 0 && a < l) && (b >= 0 && b < c) && (a != i || b != j) && cells[a][b] == 'o') {
                                ++neighborhood;
                            }
                        }
                    }
                    switch (neighborhood) {
                        case 1:
                            cells[i][j] = '1';
                            break;
                        case 2:
                            cells[i][j] = '2';
                            break;
                        case 3:
                            cells[i][j] = '3';
                            break;
                        case 4:
                            cells[i][j] = '4';
                            break;
                        case 5:
                            cells[i][j] = '5';
                            break;
                        case 6:
                            cells[i][j] = '6';
                            break;
                        case 7:
                            cells[i][j] = '7';
                            break;
                        case 8:
                            cells[i][j] = '8';
                            break;
                        case 9:
                            cells[i][j] = '9';
                            break;
                    }
                }
            }
        }

        n_checked = 0;
        locked = false;
        cursor = Tuple_(l/2,c/2);

        cout << "Minefield initialized" << endl;
    }

    void mv_up() {
        if (cursor.i == 0) {
            cursor.i = l - 1;
        } else {
            cursor.i--;
        }
    }

    void mv_down() {
        if (cursor.i == l - 1) {
            cursor.i = 0;
        } else {
            cursor.i++;
        }
    }

    void mv_left() {
        if (cursor.j == 0) {
            cursor.j = c - 1;
        } else {
            cursor.j--;
        }
    }

    void mv_right() {
        if (cursor.j == c - 1) {
            cursor.j = 0;
        } else {
            cursor.j++;
        }
    }

    void print() {
        int i, j;
        system("clear");
        cout << "┌───";
        for (j = 1; j < c; ++j) {
            cout << "┬───";
        }
        cout << "┐" << endl;

        for (i = 0; i < l; ++i) {
            cout << "│";
            for (j = 0; j < c; ++j) {
                if (cursor.i == i && cursor.j == j) {
                    cout << "[";
                } else {
                    cout << " ";
                }
                if (checked[i][j]) {
                    cout << cells[i][j];
                } else {
                    cout << curtain[i][j];
                }
                if (cursor.i == i && cursor.j == j) {
                    cout << "]";
                } else {
                    cout << " ";
                }
                cout << "│";
            }
            cout << endl;
            if (i < l - 1) {
                cout << "├───";
                for (j = 1; j < c; ++j) {
                    cout << "┼───";
                }
                cout << "┤" << endl;
            }
        }

        cout << "└───";
        for (j = 1; j < c; ++j) {
            cout << "┴───";
        }
        cout << "┘" << endl;
    }

    void uncover() {
        for (int i = 0; i < l; ++i) {
            for (int j = 0; j < c; ++j) {
                if (cells[i][j] == 'o') {
                    checked[i][j] = true;
                }
            }
        }
        print();
    }

    void check() {
        check(cursor.i, cursor.j);
    }

    void check(int i, int j) {
        if (locked) {
            throw shit_happened("O campo está bloqueado para jogar, por favor reinicialize.");
        }

        if (i < 0 || i >= l || j < 0 || j >= c) {
            throw shit_happened("Célula inexistente.");
        }

        if (checked[i][j]) {
            throw shit_happened("Esta Célula já foi checada.");
        }

        checked[i][j] = true;
        if (cells[i][j] == 'o') {
            cells[i][j] = '@';
            uncover();
            cout << "Explodiu! - fim de jogo" << endl;
            locked = true;
        } else {
            ++n_checked;
            list<Tuple> discovered;
            if (cells[i][j] == ' ') {
                discovered.push_back(Tuple_(i,j));
                while (!discovered.empty()) {
                    Tuple t = discovered.front();
                        for (int a = t.i-1; a <= t.i+1; ++a) {
                            for (int b = t.j-1; b <= t.j+1; ++b) {
                                if ((a >= 0 && a < l) && (b >= 0 && b < c) && (a != t.i || b != t.j) && !checked[a][b]) {
                                    checked[a][b] = true;
                                    ++n_checked;
                                    if (cells[a][b] == ' ') {
                                        discovered.push_back(Tuple_(a,b));
                                    }
                                }
                            }
                        }
                    discovered.pop_front();
                }
            }
            if (win()) {
                uncover();
                cout << "Vitória! - fim de jogo" << endl;
                locked = true;
            } else {
                print();
            }
        }
    }

    bool win() {
        return (c * l) - minas == n_checked;
    }

    void curtain_set() {
        curtain_set(cursor.i, cursor.j);
    }

    void curtain_set(int i, int j) {
        if (curtain[i][j] == '+') {
            curtain[i][j] = '!';
        } else if (curtain[i][j] == '!') {
            curtain[i][j] = '?';
        } else {
            curtain[i][j] = '+';
        }
        print();
    }

    void play() {
        print();
        int command;
        do {
            command = tecla();
            switch (command) {
                case UP:
                    mv_up();
                    print();
                    break;
                case DOWN:
                    mv_down();
                    print();
                    break;
                case LEFT:
                    mv_left();
                    print();
                    break;
                case RIGHT:
                    mv_right();
                    print();
                    break;
                case ENTER:
                    try {
                        check();
                    } catch (shit_happened x){
                        cout << x.get_shit() << endl;
                    }
                    break;
                case SPACE:
                    curtain_set();
            }
        } while (!locked);
    }

};

int main(int argc, char const *argv[]) {
    Minefield field(20, 15);
    field.init(0.1);
    cout << "pip!" << endl;
    field.play();
    return 0;
}
