#include <iostream>
#include <conio.h>      // _kbhit(), _getch()
#include <windows.h>    // Sleep()
#include <vector>
#include <ctime>
#include <fstream>

using namespace std;
const int width = 50;
const int height = 20;
bool gameOver;
int x, y;           // Rắn (đầu)
int fruitX, fruitY; // Trái cây
int score = 0;
int lastScore = 0;
enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };
Direction dir;
bool useArrowKeys = true;  // biến để chọn cách điều khiển


// Đọc điểm từ file (nếu có)
void LoadLastScore() {
    ifstream fin("score.txt");
    if (fin.is_open()) {
        fin >> lastScore;
        fin.close();
    }
    else {
        lastScore = 0; // nếu file chưa tồn tại
    }
}

// Ghi điểm vào file sau khi chơi xong
void SaveScore() {
    ofstream fout("score.txt");
    if (fout.is_open()) {
        fout << score;
        fout.close();
    }
}


void ShowControlMenu() {
    cout << "=== CHON CACH DIEU KHIEN ===" << endl;
    cout << "1. Dung phim mui ten (↑ ↓ ← →)" << endl;
    cout << "2. Dung phim WASD" << endl;
    cout << "Nhap lua chon (1 hoac 2): ";
    int choice;
    cin >> choice;
    if (choice == 2)
        useArrowKeys = false;
    else
        useArrowKeys = true;
}
vector<pair<int, int>> snakeBody;
void HideCursor() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;

    GetConsoleCursorInfo(hOut, &cursorInfo);  // Lấy thông tin con trỏ hiện tại
    cursorInfo.bVisible = FALSE;              // Ẩn con trỏ
    SetConsoleCursorInfo(hOut, &cursorInfo);  // Cập nhật lại
}
void moveCursor(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void Setup() {
    gameOver = false;
    dir = UP;
    x = width / 2;
    y = height / 2;
    fruitX = rand() % width;
    fruitY = rand() % height;
    score = 0;
    snakeBody.clear();
}

void Draw() {
    moveCursor(0, 0);
    //system("cls"); // Clear console

    // In viền trên
    for (int i = 0; i < width + 2; i++)
        cout << "#";
    cout << endl;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (j == 0)
                cout << "#";

            if (i == y && j == x)
                cout << "O"; // đầu rắn
            else if (i == fruitY && j == fruitX)
                cout << "F"; // trái cây
            else {
                bool print = false;
                for (auto segment : snakeBody) {
                    if (segment.first == j && segment.second == i) {
                        cout << "o";
                        print = true;
                        break;
                    }
                }
                if (!print)
                    cout << " ";
            }

            if (j == width - 1)
                cout << "#";
        }
        cout << endl;
    }

    // In viền dưới
    for (int i = 0; i < width + 2; i++)
        cout << "#";
    cout << endl;
    cout << "Score: " << score << "  |  Diem lan truoc: " << lastScore << endl;


}

void Input() {
    if (_kbhit()) {
        int key = _getch();
        if (useArrowKeys) {
            if (key == 224) {
                int arrow = _getch();
                switch (arrow) {
                case 72: if (dir != DOWN) dir = UP; break;
                case 80: if (dir != UP) dir = DOWN; break;
                case 75: if (dir != RIGHT) dir = LEFT; break;
                case 77: if (dir != LEFT) dir = RIGHT; break;
                }
            }
        }
        else {
            switch (key) {
            case 'w': case 'W': if (dir != DOWN) dir = UP; break;
            case 's': case 'S': if (dir != UP) dir = DOWN; break;
            case 'a': case 'A': if (dir != RIGHT) dir = LEFT; break;
            case 'd': case 'D': if (dir != LEFT) dir = RIGHT; break;
            }
        }
    }
}


void Logic() {
    // Di chuyển thân rắn
    snakeBody.insert(snakeBody.begin(), { x, y });

    switch (dir) {
    case LEFT:  x--; break;
    case RIGHT: x++; break;
    case UP:    y--; break;
    case DOWN:  y++; break;
    default: break;
    }

    // Kiểm tra thua (tường)
    if (x >= width || x < 0 || y >= height || y < 0)
        gameOver = true;

    // Kiểm tra thua (cắn chính mình)
    for (int i = 0; i < snakeBody.size(); i++) {
        if (snakeBody[i].first == x && snakeBody[i].second == y) {
            gameOver = true;
            break;
        }
    }

    // Nếu ăn trái cây
    if (x == fruitX && y == fruitY) {
        score += 10;
        fruitX = rand() % width;
        fruitY = rand() % height;
    }
    else {
        // Không ăn thì cắt đuôi
        snakeBody.pop_back();
    }
}

int main() {
    LoadLastScore();
    ShowControlMenu();
    HideCursor();
    srand(time(0));
    Setup();
    while (!gameOver) {
        Draw();
        Input();
        Logic();
        Sleep(100); // tốc độ rắn
    }

    cout << "Game Over!" << endl;
    SaveScore();
    system("pause");
    return 0;
}
