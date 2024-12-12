#include <stdio.h>
#include <stdlib.h>
#include <termios.h> // mac환경이라 conio를 못썻습니다.
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define WIDTH 10
#define HEIGHT 10
#define MINES 10


char board[HEIGHT][WIDTH];
char display[HEIGHT][WIDTH];
int cursorX = 0, cursorY = 0;


int menuCursor = 0; // 0: 게임 시작, 1: 게임 종료

// 주변 8칸 좌표
int dx[8] = {-1, -1, -1, 0, 1, 1, 1, 0};
int dy[8] = {-1, 0, 1, 1, 1, 0, -1, -1};

int kbhit(void) {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

int getch(void) {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

// 지뢰 보드 초기화
void initialize_board() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            board[i][j] = '0';
            display[i][j] = '.';
        }
    }

    srand(time(NULL));
    for (int i = 0; i < MINES;) {
        int x = rand() % WIDTH;
        int y = rand() % HEIGHT;
        if (board[y][x] == '0') {
            board[y][x] = 'M';
            i++;
        }
    }

    // 지뢰 주변 칸에 숫자 표시
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (board[y][x] == 'M') continue;
            
            int mine_count = 0;
            for (int i = 0; i < 8; i++) {
                int nx = x + dx[i];
                int ny = y + dy[i];
                if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT && board[ny][nx] == 'M') {
                    mine_count++;
                }
            }
            if (mine_count > 0) {
                board[y][x] = '0' + mine_count; // 인접 지뢰 수를 숫자로 변환
            }
        }
    }
}

// 보드 출력
void print_board() {
    system("clear"); 
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i == cursorY && j == cursorX) {
                printf(" [X] "); // 현재 커서 위치
            } else {
                printf(" %c  ", display[i][j]);
            }
        }
        printf("\n");
    }
}

// 타일 선택 시 동작
void reveal_tile(int x, int y) {
    if (board[y][x] == 'M') {
        printf("지뢰를 밟았습니다!\n");
        exit(0);
    } else {
        display[y][x] = board[y][x]; // 인접한 지뢰 개수
    }
}

// 커서 이동
void move_cursor(char input) {
    if (input == 'w' && cursorY > 0) {
        cursorY--;
    } else if (input == 's' && cursorY < HEIGHT - 1) {
        cursorY++;
    } else if (input == 'a' && cursorX > 0) {
        cursorX--;
    } else if (input == 'd' && cursorX < WIDTH - 1) {
        cursorX++;
    }
}

// 메뉴 출력
void print_menu() {
    system("clear"); // 콘솔을 지우는 함수
    printf("===== 지뢰찾기 게임 =====\n");
    printf("%s 게임 시작\n", menuCursor == 0 ? "->" : "  ");
    printf("%s 게임 종료\n", menuCursor == 1 ? "->" : "  ");
}

// 메뉴 커서 이동
void move_menu_cursor(char input) {
    if (input == 'w' && menuCursor > 0) {
        menuCursor--;
    } else if (input == 's' && menuCursor < 1) {
        menuCursor++;
    }
}

// 지뢰찾기 게임 시작
void start_game() {
    initialize_board();

    while (1) {
        print_board();
        if (kbhit()) {
            char input = getch(); // 키 입력
            if (input == '\n') {
                reveal_tile(cursorX, cursorY); // 엔터키로 타일 선택
            } else {
                move_cursor(input); // w, a, s, d 키로 커서 이동
            }
        }
    }
}

// 메인 함수
int main() {
    while (1) {
        print_menu();
        if (kbhit()) {
            char input = getch(); // 키 입력
            if (input == '\n') {
                if (menuCursor == 0) {
                    start_game(); // 게임 시작
                } else if (menuCursor == 1) {
                    printf("게임을 종료합니다.\n");
                    break; // 게임 종료
                }
            } else {
                move_menu_cursor(input); // w, s 키로 메뉴 이동
            }
        }
    }

    return 0;
}
