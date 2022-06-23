#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <process.h>
#include <string.h>
#include<mmsystem.h>
#pragma comment(lib, "winmm.lib")



#define LEFT 75    // 방향키 ← 정의 ( 좌로 이동 )																int main() {
#define RIGHT 77   // 방향키 → 정의 ( 우로 이동 )																int key;
#define UP 72	   // 방향키 ↑ 정의 ( 회전 )																	key = _getch();
#define DOWN 80    // 방향키 ↓ 정의 ( Soft Drop )																if (key == 224) {          다음 코드를 통해
#define SPACE 32   // SPACEBAR 키 정의 ( Hard Drop )															key = _getch();            KEY 값을 얻어냄
#define ESC 27     // ESC 키 정의 ( 일시정지 )																	}
#define ENTER 13   //																							printf("%d", key);			
//																												return 0;
//																												}5

#define SPECIAL_MOTION 777 // 스페셜모션. 블록이 땅이나 블록과 닿은 상태에서 한 칸 위로 올려 회전이 가능 할 때 

#define TRUE 1 
#define FALSE 0

#define ACTIVE_BLOCK -2 // 블럭
#define CEILING -1	// 천장
#define EMPTY 0		// 빈 공간
#define WALL 1		// 벽
#define INACTIVE_BLOCK 2 // 이동이 완료된 블럭

#define GAMEBOARD_X 12		// 게임판 가로 크기
#define GAMEBOARD_Y 25		// 게임판 세로 크기
#define GAMEBOARD_X_ADJ	5	// 게임판 X값 위치 조정
#define GAMEBOARD_Y_ADJ	2	// 게임판 Y값 위치 조정

#define STATUS_X 2 * (GAMEBOARD_X_ADJ + GAMEBOARD_X + 1) // 게임 정보 위치 설정

// 함수선언 -----------------------------------------------------------------------------------


void GameStart(); // 게임 정보 초기화, 게임 첫 시작하는 함수
void TitleMenuDraw(); // 타이틀 메뉴를 그림
void WINAPI AnimationStar(void* arg); // 별 애니메이션 쓰레드 함수
void InfoMenu(); // 도움말 메뉴
void ResetGameBoard(); // 게임보드를 초기화
void ResetGameBoardCopy(); // copy 게임보드를 초기화
void DrawGameBoard(); // 게임보드를 그림
void GameInfo(); // 게임 부가 정보를 그림
void StartLevelDraw(); // 현재 레벨을 그림
void WINAPI StopwatchStart(void* arg); // 스탑워치 함수

void DrawBlock(); // 블럭을 그림
void CheckKeyhit(); // 키 입력을 확인하고 키 정보를 받아냄
void checkLine(); // 라인 클리어 하는 함수
void checkLevelUp(); // 레벨 업 시키는 함수
void moveBlock(); // 블럭을 움직임
void dropBlock(); // 블럭을 떨어트림

void rankBoard(); // 랭킹정보를 그림
void AddRank(int level, int score, int lines); // 랭크 정보를 저장함
void RankSort(void); // 랭킹 순위를 정함

void rankPutCheck(); // 랭킹 입력할 것인지에 대한 유무 확인
void setting(); // 랭킹 정보를 파일로부터 불러옴 
void rankFileSave(); // 랭킹 정보를 파일로 저장함
void getCurPath(bgm); // 현재 디렉토리 경로를 불러오고 bgm 경로와 이어붙임
int pause(); // 일시정지
int TitleMenuText(); // 타이틀 메뉴의 텍스트를 그림
int CrushBlock(); // 블럭 충돌 확인

int gameOver(); // 게임오버


// ----------------------------------------------------------------------------------------------

char path[400]; // appdata 경로 저장
char path2cpy[400]; // 현재경로와 bgm 경로 저장
char* bgm[8] = { "\\Sound\\Bradinsky.wav", "\\Sound\\Karinka.wav","\\Sound\\Loginska.wav", "\\Sound\\4.wav", "\\Sound\\LevelClear.wav","\\Sound\\GameOver.wav", 
				 "\\Sound\\select.wav", "\\Sound\\menu.wav" }; // bgm 경로 저장

int STATUS_GOAL_Y;  // 목표 라인 Y 위치 저장
int STATUS_SCORE_Y; // 현재 점수 Y 위치 저장
int STATUS_LEVEL_Y; // 현재 레벨 Y 위치 저장

int level;		// 게임 레벨
int score;			// 게임 스코어
int speed;			// 게임 속도
int goal;		// 목표 라인

int curBlockRotation;		// 도형 방향
int absBlock_X, absBlock_Y;		// 도형 절대 좌표
int curBlock_X, curBlock_Y;		// 도형 현재 좌표
int curBlock;		// 현재 도형
int curBlockColor;  // 현재 도형의 색깔을 저장
int nextBlock;		// 다음 도형
int linecnt;		// 제거한 줄의 갯수 저장
int acclinecnt;		// 제거한 줄의 갯수 누적 저장
int NewBlockOn = 0; // 새로운 블록을 만들지에 대한 신호
int CrushOn = 0; // 현재 블록이 충돌상태 인지에 대한 신호
int spaceOn = 0; // harddrop을 할 것인지에 대한 신호
int LevelUpOn = 0; // 레벨업을 할 것인지에 대한 신호
int gameOverOn = 0;		// 게임 오버 신호

int best_score = 0; // 최고 점수
int last_score = 0; // 최근 마지막 점수
int soundOnePlay = 1; // 사운드를 한번만 실행

int keyhit; // 키 감지

int pauseSelectMenu; // pause에서 어떤 메뉴 선택했는지 저장

int gameboard[GAMEBOARD_Y][GAMEBOARD_X];		// 게임 판을 배열로 통해 만듬
int gameboard_cpy[GAMEBOARD_Y][GAMEBOARD_X];	// 게임 판의 정보를 배열로 출력하고, gameboard_cpy 에 복사된다.
												// gameboard 와 gameboard_cpy 를 비교 해 cpy 와 값이 달라진 곳만 맵을 고친다.

HANDLE thread1;	// 별 반짝이는 애니메이션과 메뉴 선택 기능에서 gotoxy 함수를 동시에 사용할 수 있도록 쓰레드 사용
HANDLE Stopwatch; // 게임 플레이와 스탑워치를 동시에 사용할 수 있도록 쓰레드 사용

int Blocks[7][4][4][4] = {
	// ㅁ
	{{0,0,0,0, 0,1,1,0, 0,1,1,0, 0,0,0,0},{0,0,0,0, 0,1,1,0, 0,1,1,0, 0,0,0,0},
	 {0,0,0,0, 0,1,1,0, 0,1,1,0, 0,0,0,0},{0,0,0,0, 0,1,1,0, 0,1,1,0, 0,0,0,0}},
	 // ㅡ
	 {{0,0,0,0, 0,0,0,0, 1,1,1,1, 0,0,0,0},{0,1,0,0, 0,1,0,0, 0,1,0,0, 0,1,0,0},
	  {0,0,0,0, 0,0,0,0, 1,1,1,1, 0,0,0,0},{0,1,0,0, 0,1,0,0, 0,1,0,0, 0,1,0,0}},
	  // ㄱㄴ
	  {{0,0,0,0, 1,1,0,0, 0,1,1,0, 0,0,0,0},{0,0,0,0, 0,0,1,0, 0,1,1,0, 0,1,0,0},
	   {0,0,0,0, 1,1,0,0, 0,1,1,0, 0,0,0,0},{0,0,0,0, 0,0,1,0, 0,1,1,0, 0,1,0,0}},
	   // ㄱㄴ 반대
	   {{0,0,0,0, 0,1,1,0, 1,1,0,0, 0,0,0,0},{0,0,0,0, 1,0,0,0, 1,1,0,0, 0,1,0,0},
		{0,0,0,0, 0,1,1,0, 1,1,0,0, 0,0,0,0},{0,0,0,0, 1,0,0,0, 1,1,0,0, 0,1,0,0}},
		// ㄴ 반대
		{{0,0,0,0, 0,0,1,0, 1,1,1,0, 0,0,0,0},{0,0,0,0, 1,1,0,0, 0,1,0,0, 0,1,0,0},
		 {0,0,0,0, 0,0,0,0, 1,1,1,0, 1,0,0,0},{0,0,0,0, 0,1,0,0, 0,1,0,0, 0,1,1,0}},
		 // ㄴ
		 {{0,0,0,0, 1,0,0,0, 1,1,1,0, 0,0,0,0},{0,0,0,0, 0,1,0,0, 0,1,0,0, 1,1,0,0},
		  {0,0,0,0, 0,0,0,0, 1,1,1,0, 0,0,1,0},{0,0,0,0, 0,1,1,0, 0,1,0,0, 0,1,0,0}},
		  // ㅗ
		  {{0,0,0,0, 0,1,0,0, 1,1,1,0, 0,0,0,0},{0,0,0,0, 0,1,0,0, 0,1,1,0, 0,1,0,0},
		   {0,0,0,0, 0,0,0,0, 1,1,1,0, 0,1,0,0},{0,0,0,0, 0,1,0,0, 1,1,0,0, 0,1,0,0}}
};


struct info {
	char name[10];
	int level;
	int score;
	int lines;
};

struct info rank[11];

void ConsoleSetting() {
	system("title Tetris");					// Console 제목을 Tetris로 변경
	system("mode con cols=80 lines=30");	// Console Size를 가로 80, 세로 30으로 지정
}

void gotoxy(int x, int y) {												// gotoxy 함수를 정의.
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

typedef enum { BLACK, BLUE, GREEN, CYAN, RED, PURPLE, BROWN, LIGHTGRAY, DARKGRAY, LIGHTBLUE, LIGHTGREEN, LIGHTCYAN, LIGHTRED, LIGHTPURPLE, YELLOW, WHITE } Color;

void SetColor(unsigned short text, unsigned short back) {				// 텍스트 혹은 배경색 Color 사용
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), text | (back << 4));
}

typedef enum { NOCURSOR, SOLIDCURSOR, NORMALCURSOR } CursorType;   // 열거형을 통해 커서타입 설정 (커서 숨기기 함수에 사용)

void CursorTypeOption(CursorType c) {	// 커서타입 설정
	CONSOLE_CURSOR_INFO CurInfo;
	switch (c) {
	case NOCURSOR:
		CurInfo.dwSize = 1;
		CurInfo.bVisible = FALSE;
		break;
	case SOLIDCURSOR:
		CurInfo.dwSize = 100;
		CurInfo.bVisible = TRUE;
		break;
	case NORMALCURSOR:
		CurInfo.dwSize = 20;
		CurInfo.bVisible = TRUE;
		break;
	}
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CurInfo);
}

int main() {
	setting();
	srand((unsigned)time(NULL));

	ConsoleSetting();

	while (1) {
		TitleMenuDraw();
		do {
			thread1 = CreateThread(NULL, 0, AnimationStar, NULL, 0, NULL); 	// 별 애니메이션을 위한 스레드 지정
		} while (FALSE);


		int MenuCode = TitleMenuText();
		if (MenuCode == 0) {
			srand((unsigned)time(NULL));
			CursorTypeOption(NOCURSOR);
			GameStart(); // 게임시작
			curBlockColor = curBlock;
			while (1) {
				for (int i = 0; i < 5; i++) {

					CheckKeyhit();
					DrawGameBoard();
					Sleep(speed);

					if (CrushOn && CrushBlock(curBlock_X, curBlock_Y + 1, curBlockRotation) == FALSE) Sleep(100);

					if (spaceOn == 1) {
						spaceOn = 0;
						break;
					}

				}

				dropBlock();

				checkLevelUp();

				if (pauseSelectMenu == 4) {
					GameStart();
					pauseSelectMenu = 0;
				}
				else if (pauseSelectMenu == 6) {
					pauseSelectMenu = 0;
					break;
				}

				int gameOverMenu = gameOver();

				if (gameOverMenu == 0) GameStart(); 
				else if (gameOverMenu == 1) rankPutCheck();
				else if (gameOverMenu == 2) break;
				else if (gameOverMenu == 3) exit(0);

				if (NewBlockOn == 1) DrawBlock();
			}

		}
		else if (MenuCode == 1) {
			SuspendThread(thread1);
			rankBoard();
			_getch();
			getCurPath(bgm[6]);
			PlaySound(path2cpy, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);

		}
		else if (MenuCode == 2) {
			InfoMenu(); // 도움말
		}
		else if (MenuCode == 3) {
			return 0; // 게임 종료
		}
		system("cls");
	}
	return 0;
}

int TitleMenuText() {
	int x = 35;
	int y = 16;

	gameOverOn = 0;

	gotoxy(x - 2, y); printf("> 게임시작");
	gotoxy(x - 2, y + 1); printf("    랭킹    ");
	gotoxy(x - 2, y + 2); printf("   도움말  ");
	gotoxy(x - 2, y + 3); printf("    종료   ");
	gotoxy(x - 22, y + 12); printf("< Copyright 2019. PARK JIN SUNG All Rights Reserved >");
	gotoxy(x - 6, y + 8); printf("Made by PARK JIN SUNG");
	gotoxy(x - 10, y + 9); printf("@email : jerry03122@naver.com");
	gotoxy(x - 6, y + 10); printf("Discord : Reibeu#4227");

	Sleep(1);

	CursorTypeOption(0);
	while (1) {							// 무한루프를 통해 방향키 입력 유무를 확인
		int key = _getch();
		//SuspendThread(thread1);

		switch (key) {
		case UP:
			getCurPath(bgm[7]);
			PlaySound(path2cpy, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
			if (y > 16) {				// UP을 했을 경우 >를 지우고 한칸 밑에 > 를 생성함
				SuspendThread(thread1);
				gotoxy(x - 2, y);
				printf(" ");
				gotoxy(x - 2, --y);
				printf("> ");
				ResumeThread(thread1);
				
			}
			break;

		case DOWN:
			getCurPath(bgm[7]);
			PlaySound(path2cpy, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
			if (y < 19) {
				SuspendThread(thread1);
				gotoxy(x - 2, y);
				printf(" ");
				gotoxy(x - 2, ++y);
				printf("> ");
				ResumeThread(thread1);
			}
			break;
		case ENTER:						// ENTER -> 선택
			getCurPath(bgm[6]);
			PlaySound(path2cpy, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
			SuspendThread(thread1);
			while (_kbhit()) _getch();
			return y - 16;				// 시작 위치가 17이므로 0,1,2 값을 받는다.
		}
		//ResumeThread(thread1);
	}

}

void TitleMenuDraw() {


	gotoxy(14, 3); printf("┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓");
	Sleep(100);
	gotoxy(14, 4); printf("┃  ◆◆◆  ◆◆◆  ◆◆◆   ◆◆     ◆   ◆◆  ┃");
	Sleep(100);
	gotoxy(14, 5); printf("┃    ◆    ◆        ◆     ◆ ◆    ◆  ◆     ┃");
	Sleep(100);
	gotoxy(14, 6); printf("┃    ◆    ◆◆◆    ◆     ◆◆     ◆   ◆◆  ┃");
	Sleep(100);
	gotoxy(14, 7); printf("┃    ◆    ◆        ◆     ◆ ◆    ◆      ◆ ┃");
	Sleep(100);
	gotoxy(14, 8); printf("┃    ◆    ◆◆◆    ◆     ◆  ◆   ◆   ◆◆  ┃");
	Sleep(100);
	gotoxy(14, 9); printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
	gotoxy(14, 10); printf(" Ver 0.2");


}

typedef struct _Area {  // 별 좌표 구조체 설정
	int x1;
	int y1;
} Area;

Area MakingStar(int Area_X, int Area_Y, int startX, int startY) { // 별을 만드는 함수
																	// 시작좌표와 끝 좌표를 받아 구역을 설정하고, 랜덤함수를 통해
	Area xy;														// 랜덤 좌표를 받아 그 좌표에 별을 생성한다.
	xy.x1 = (rand() % Area_X) + startX;
	xy.y1 = (rand() % Area_Y) + startY;

	gotoxy(xy.x1, xy.y1);

	printf("*");

	return xy;
}

void RemoveStar(Area xy) {	// 별 지우는 함수 별이 출력됐던 xy(좌표)를 받아 별을 지운다.
	gotoxy(xy.x1, xy.y1);
	printf(" ");
	Sleep(50);
}

void WINAPI AnimationStar(void* arg) {  // 별 지우는 함수와 만드는 함수를 받아 실행하는 함수
	Area xy[10];						// cnt를 0~ 5 까지 랜덤으로 받아 별이 출력 될 구역을 랜덤으로 실행
	srand(time(NULL));
	while (1) {
		for (int i = 0; i <= 9; i++) {
			Sleep(10);
			int cnt = rand() % 6;
			switch (cnt) {
			case 0:
				xy[i] = MakingStar(78, 3, 0, 0);
				break;
			case 1:
				xy[i] = MakingStar(6, 28, 0, 0);
				break;
			case 2:
				xy[i] = MakingStar(16, 28, 63, 0);
				break;
			case 3:
				xy[i] = MakingStar(17, 11, 13, 11);
				break;
			case 4:
				xy[i] = MakingStar(10, 4, 33, 11);
				break;
			case 5:
				xy[i] = MakingStar(18, 10, 44, 11);
				break;
			}
		}
		for (int i = 0; i <= 9; i++) {
			RemoveStar(xy[i]);
		}
	}

	_endthread();
}

void GameStart() {
	level = 1;
	getCurPath(bgm[0]);
	//strcat(path2cpy, bgm);
	score = 0;
	goal = 10;
	keyhit = 0;
	linecnt = 0;
	acclinecnt = 0;
	speed = 100;
	CrushOn = 0;
	soundOnePlay = 1;

	SuspendThread(thread1);

	StartLevelDraw();
	system("cls");

	PlaySound(path2cpy, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NODEFAULT);
	
	ResetGameBoard();
	GameInfo();
	// Stopwatch = CreateThread(NULL, 0, StopwatchStart, NULL, 0, NULL); // 스톱워치 스레드. 게임진행과 동시에 실행
	Sleep(1);
	DrawGameBoard();


	nextBlock = rand() % 7;
	DrawBlock();
}

void InfoMenu() {  // 도움말 메뉴
	SuspendThread(thread1);
	system("cls");
	int x = 15, y = 5;
	gotoxy(x - 7, y - 4); printf("< 조작법 >");
	gotoxy(x, y - 2); printf("                △   : 회전               ");
	gotoxy(x, y - 1); printf("              ◁  ▷ : 왼쪽 / 오른쪽        ");
	gotoxy(x, y + 0); printf("                ▽   : Soft Drop           ");
	gotoxy(x, y + 2); printf("               SPACE : Hard Drop           ");
	gotoxy(x, y + 3); printf("                ESC   : 일시정지           ");


	gotoxy(x - 7, y + 5); printf("<  Tip  >");
	gotoxy(x - 5, y + 7); printf("① 게임을 시작합니다.");
	gotoxy(x - 5, y + 9); printf("② 블럭을 왼쪽, 오른쪽 키로 움직일 수 있습니다.");
	gotoxy(x - 5, y + 11); printf("③ 윗 방향키를 사용하여 블럭을 회전하고 구멍에 맞추세요.");
	gotoxy(x - 5, y + 13); printf("④ 액션 키를 이용하여 블럭키를 빨리 떨어트릴 수 있습니다");
	gotoxy(x - 5, y + 14); printf("    + Hard Drop을 하면 보너스 점수가 주어집니다.");
	gotoxy(x - 5, y + 16); printf("⑤ 각 라인을 클리어 하여 점수를 얻으세요");
	gotoxy(x - 5, y + 17); printf("    + 라인을 한꺼번에 많이 클리어 할 수록");
	gotoxy(x - 5, y + 18); printf("      더 많은 보너스 점수가 주어집니다.");
	gotoxy(x - 5, y + 20); printf("⑥ 다음 올 블럭을 고려하여 설계 하세요.");
	SetColor(LIGHTRED, BLACK);
	gotoxy(x - 2, y + 22); printf("마지막으로, 게임을 마음껏 즐기세요 !");
	SetColor(WHITE, BLACK);


	while (1) {						// 무한루프를 돌면서 반복한 키 값이 ENTER일 경우 반복을 중지하고 함수를 종료
		int key = _getch();
		if (key == ENTER) {
			getCurPath(bgm[6]);
			PlaySound(path2cpy, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
			ResumeThread(thread1);
			break;
		}
	}
}

void ResetGameBoard() {		// 게임판
	int i, j;
	for (i = 0; i < GAMEBOARD_Y; i++) {
		for (j = 0; j < GAMEBOARD_X; j++) {
			gameboard[i][j] = 0;
			gameboard_cpy[i][j] = 100;
		}
	}

	for (i = 1; i < GAMEBOARD_Y - 1; i++) {			// 양쪽 벽 설정
		gameboard[i][0] = WALL;
		gameboard[i][GAMEBOARD_X - 1] = WALL;
	}
	for (j = 0;j < GAMEBOARD_X;j++) {				// 바닥 설정 
		gameboard[GAMEBOARD_Y - 1][j] = WALL;
	}
}

void ResetGameBoardCopy() {
	int i, j;

	for (i = 0; i < GAMEBOARD_X * 2; i++) {
		for (j = 0; j < GAMEBOARD_Y * 2; j++) {
			gameboard_cpy[i][j] = 100;
		}
	}
}

void DrawGameBoard() {					// 판을 그림
	int i, j;
	for (j = 1; j < GAMEBOARD_X - 1; j++) {
		if (gameboard[3][j] == EMPTY) gameboard[3][j] = CEILING;
	}

	for (i = 0; i < GAMEBOARD_Y; i++) {
		for (j = 0; j < GAMEBOARD_X; j++) {
			if (gameboard_cpy[i][j] != gameboard[i][j]) {
				gotoxy(2 * (GAMEBOARD_X_ADJ + j), GAMEBOARD_Y_ADJ + i);
				switch (gameboard[i][j]) {
				case EMPTY:
					printf("  ");
					break;
				case WALL:
					SetColor(WHITE, WHITE);
					printf("▒");
					SetColor(WHITE, BLACK);
					break;
				case CEILING:
					printf("..");
					break;
				case ACTIVE_BLOCK:
					switch (curBlock + 1) {
					case 1:
						SetColor(BLUE, BLUE);
						printf("■");
						SetColor(WHITE, BLACK);
						break;
					case 2:
						SetColor(CYAN, CYAN);
						printf("■");
						SetColor(WHITE, BLACK);
						break;
					case 3:
						SetColor(BROWN, BROWN);
						printf("■");
						SetColor(WHITE, BLACK);
						break;
					case 4:
						SetColor(YELLOW, YELLOW);
						printf("■");
						SetColor(WHITE, BLACK);
						break;
					case 5:
						SetColor(GREEN, GREEN);
						printf("■");
						SetColor(WHITE, BLACK);
						break;
					case 6:
						SetColor(RED, RED);
						printf("■");
						SetColor(WHITE, BLACK);
						break;
					case 7:
						SetColor(LIGHTPURPLE, LIGHTPURPLE);
						printf("■");
						SetColor(WHITE, BLACK);
						break;
					}
					break;
				case INACTIVE_BLOCK:
					switch (curBlockColor + 1) {
					case 1:
						SetColor(BLUE, BLUE);
						printf("▣");
						SetColor(WHITE, BLACK);
						break;
					case 2:
						SetColor(CYAN, CYAN);
						printf("▣");
						SetColor(WHITE, BLACK);
						break;
					case 3:
						SetColor(BROWN, BROWN);
						printf("▣");
						SetColor(WHITE, BLACK);
						break;
					case 4:
						SetColor(YELLOW, YELLOW);
						printf("▣");
						SetColor(WHITE, BLACK);
						break;
					case 5:
						SetColor(GREEN, GREEN);
						printf("▣");
						SetColor(WHITE, BLACK);
						break;
					case 6:
						SetColor(RED, RED);
						printf("▣");
						SetColor(WHITE, BLACK);
						break;
					case 7:
						SetColor(LIGHTPURPLE, LIGHTPURPLE);
						printf("▣");
						SetColor(WHITE, BLACK);
						break;
					}
				}
			}
		}
	}

	for (i = 0; i < GAMEBOARD_Y; i++) {				// 게임판의 정보를 복사함.
		for (j = 0; j < GAMEBOARD_X; j++) {
			gameboard_cpy[i][j] = gameboard[i][j];
		}
	}
}

void GameInfo() {								//게임 상태 표시를 나타내는 함수  
	int y = 3;									// level, goal, score만 게임에서 값이 바뀔 수 있기에, 그 y 값을 따로 저장한다.
												// 게임 상태 표시 위치가 바뀌더라도 해당 함수에서 변경하지 않아도 되도록 설정.
	best_score = rank[0].score;
	gotoxy(STATUS_X + 24, y - 1); printf("┌──── ====────┐ ");
	gotoxy(STATUS_X + 24, y + 0); printf("◆   TETRIS   ◆ ");
	gotoxy(STATUS_X + 24, y + 1); printf("└──── ====────┘ ");

	gotoxy(STATUS_X, y + 0); printf("┌─ Lv ─┐ ");
	gotoxy(STATUS_X, y + 1); printf("│      │ ");
	gotoxy(STATUS_X + 2, STATUS_LEVEL_Y = y + 1); printf("%3d", level);
	gotoxy(STATUS_X, y + 2); printf("└──────┘ ");

	gotoxy(STATUS_X, STATUS_GOAL_Y = y + 4); printf(" GOAL  : %5d", goal - linecnt);

	gotoxy(STATUS_X, y + 6); printf("+-  N E X T  -+ ");
	gotoxy(STATUS_X, y + 7); printf("|             | ");
	gotoxy(STATUS_X, y + 8); printf("|             | ");
	gotoxy(STATUS_X, y + 9); printf("|             | ");
	gotoxy(STATUS_X, y + 10); printf("|             | ");
	gotoxy(STATUS_X, y + 11); printf("+-- -  -  - --+ ");

	gotoxy(STATUS_X, y + 13); printf(" YOUR SCORE :");
	gotoxy(STATUS_X, STATUS_SCORE_Y = y + 14); printf("        %6d", score);
	gotoxy(STATUS_X, y + 16); printf(" LAST SCORE :");
	gotoxy(STATUS_X, y + 17); printf("        %6d", last_score);
	gotoxy(STATUS_X, y + 19); printf(" BEST SCORE :");
	gotoxy(STATUS_X, y + 20); printf("        %6d", best_score);

	gotoxy(STATUS_X, y + 23);printf("     HANSEI SYBER SECURITY HIGHSCHOOL");
	gotoxy(STATUS_X, y + 24);printf("              PARK JIN SUNG");
}

void StartLevelDraw() {		// 게임 시작전 레벨 표시
	system("cls");

	gotoxy(30, 12); printf("┏━━━━━━━━━━━━━━━━┓");
	gotoxy(30, 13); printf("┃                ┃");
	gotoxy(30, 14); printf("┃    Level %3d   ┃", level);
	gotoxy(30, 15); printf("┃                ┃");
	gotoxy(30, 16); printf("┗━━━━━━━━━━━━━━━━┛");

	if (LevelUpOn) {
		for (int i = 0; i < 4;i++) {
			gotoxy(31, 11);
			printf("                 ");
			gotoxy(31, 17);
			printf("                 ");
			Sleep(200);

			gotoxy(31, 11);
			printf("☆☆LEVEL UP!☆☆");
			gotoxy(31, 17);
			printf("☆☆SPEED UP!☆☆");
			Sleep(200);
		}
	}

	Sleep(4000);
}

void WINAPI StopwatchStart(void* arg) {		// 스톱워치 함수
	char ch = 0;
	int hh = 0, mm = 0, ss = 0;

	while (1) {
		gotoxy(65, 3);
		printf("%02d:%02d:%02d", hh, mm, ss);
		ss++;
		if (ss == 59 && mm != 59) {
			mm++;
			ss = 0;
		}
		if (ss == 59 && mm == 59) {
			hh++;
			mm = 0;
			ss = 0;
		}
		Sleep(1000);
	}
}

void DrawBlock() {  // 새로운 블록 생성
	int i, j;

	curBlock_X = (GAMEBOARD_X / 2) - 2;  // 게임판 가운데
	curBlock_Y = 0;

	curBlock = nextBlock;		// 다음 블록 -> 현재블록

	nextBlock = rand() % 7;		// 다음 블록을 랜덤하게 가져옴

	curBlockRotation = 0;
	NewBlockOn = 0;		// 새로운 블록을 만들지 않음

	for (i = 0; i < 4; i++) {		// 현재 블록 생성
		for (j = 0; j < 4; j++) {
			if (Blocks[curBlock][curBlockRotation][i][j] == 1) gameboard[curBlock_Y + i][curBlock_X + j] = ACTIVE_BLOCK;
		}
	}
	for (i = 1; i < 3; i++) {		// 다음 블록 생성
		for (j = 0; j < 4; j++) {
			if (Blocks[nextBlock][0][i][j] == 1) {
				switch (nextBlock + 1) {	// 블록 타입에 따라 색 설정
				case 1:
					SetColor(BLUE, BLUE);
					gotoxy(STATUS_X + 4 + (2 * j), i + 10);
					printf("■");
					SetColor(WHITE, BLACK);
					break;
				case 2:
					SetColor(CYAN, CYAN);
					gotoxy(STATUS_X + 4 + (2 * j), i + 10);
					printf("■");
					SetColor(WHITE, BLACK);
					break;
				case 3:
					SetColor(BROWN, BROWN);
					gotoxy(STATUS_X + 4 + (2 * j), i + 10);
					printf("■");
					SetColor(WHITE, BLACK);
					break;
				case 4:
					SetColor(YELLOW, YELLOW);
					gotoxy(STATUS_X + 4 + (2 * j), i + 10);
					printf("■");
					SetColor(WHITE, BLACK);
					break;
				case 5:
					SetColor(GREEN, GREEN);
					gotoxy(STATUS_X + 4 + (2 * j), i + 10);
					printf("■");
					SetColor(WHITE, BLACK);
					break;
				case 6:
					SetColor(RED, RED);
					gotoxy(STATUS_X + 4 + (2 * j), i + 10);
					printf("■");
					SetColor(WHITE, BLACK);
					break;
				case 7:
					SetColor(LIGHTPURPLE, LIGHTPURPLE);
					gotoxy(STATUS_X + 4 + (2 * j), i + 10);
					printf("■");
					SetColor(WHITE, BLACK);
					break;
				}
			}
			else {
				gotoxy(STATUS_X + 4 + (2 * j), i + 10);
				printf("  ");
			}
		}
	}
}

int CrushBlock(int curBlock_X, int curBlock_Y, int curBlockRotation) { // 지정된 좌표와 회전값으로 인한 충돌이 있는지 검사.
	int i, j;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			if (Blocks[curBlock][curBlockRotation][i][j] == 1 && gameboard[curBlock_Y + i][curBlock_X + j] > 0) return FALSE; // 지정된 위치의 게임판과 블럭을 비교 후 겹치면 FALSE 리턴
		}
	}
	return TRUE; // 안겹치면 TRUE 리턴
}

void CheckKeyhit() {
	keyhit = 0; // 키 값 초기화

	if (_kbhit()) { // 키 입력이 있을 경우
		keyhit = _getch(); // 키 값을 받음
		if (keyhit == 224) { // 키가 방향키 인경우
			do {
				keyhit = _getch();
			} while (keyhit == 224); // 방향 지시값을 버림
			switch (keyhit) {
			case LEFT: // 왼쪽 방향 키
				if (CrushBlock(curBlock_X - 1, curBlock_Y, curBlockRotation) == TRUE) moveBlock(LEFT); // 왼쪽으로 갈 수 있는지 확인 후 가능하면 이동
				break;
			case RIGHT: // 오른쪽 방향 키
				if (CrushBlock(curBlock_X + 1, curBlock_Y, curBlockRotation) == TRUE) moveBlock(RIGHT); // 오른쪽으로 갈 수 있는지 확인 후 가능하면 이동
				break;
			case UP: // 위쪽 방향 키
				if (CrushBlock(curBlock_X, curBlock_Y, (curBlockRotation + 1) % 4) == TRUE)moveBlock(UP); // 회전 할 수 있는지 확인 후 가능하면 이동

				else if (CrushOn == 1 && CrushBlock(curBlock_X, curBlock_Y - 1, (curBlockRotation + 1) % 4) == TRUE) moveBlock(SPECIAL_MOTION); // 특수 동작
				break;
			case DOWN: // 아래쪽 방향 키
				if (CrushBlock(curBlock_X, curBlock_Y + 1, curBlockRotation) == TRUE) moveBlock(DOWN); // Soft Drop 이 가능한 지 확인 후 가능하면 이동
				break;
			}
		}
		else { // 방향 키가 아닌 경우
			switch (keyhit) {
			case SPACE: // 스페이스바
				spaceOn = 1; // 스페이스바 신호를 킴
				while (CrushOn == 0) { // 바닥에 닿을 때 까지 실행
					dropBlock();
					score += level; // Hard Drop 보너스
					gotoxy(STATUS_X, STATUS_SCORE_Y); printf("        %6d", score); // 점수 갱신
				}
				break;
			case ESC: // ESC
				pauseSelectMenu = pause(); // 게임 일시 정지
				break;
			}
		}
	}
	while (_kbhit()) _getch(); // 키 버퍼를 지움
}

void checkLine() {
	int BlockCount;
	int Combo = 0;

	for (int i = GAMEBOARD_Y - 2; i > 3;) {
		BlockCount = 0;
		for (int j = 1; j < GAMEBOARD_X - 1; j++) {
			if (gameboard[i][j] > 0) BlockCount++;
		}
		if (BlockCount == GAMEBOARD_X - 2) {
			if (LevelUpOn == 0) {
				score += 100 * level;
				linecnt++;
				acclinecnt++;
				Combo++;
			}
			for (int k = i; k > 1; k--) {
				for (int l = 1; l < GAMEBOARD_X - 1; l++) {
					if (gameboard[k - 1][l] != CEILING) gameboard[k][l] = gameboard[k - 1][l];
					if (gameboard[k - 1][l] == CEILING) gameboard[k][l] = EMPTY;
				}
			}
		}
		else i--;
	}
	if (Combo) {
		gotoxy(STATUS_X, STATUS_GOAL_Y); printf(" GOAL  : %5d", goal - linecnt);
		if (goal - linecnt < 0) 
			gotoxy(STATUS_X, STATUS_GOAL_Y); printf(" GOAL  : %5d", 0);
		gotoxy(STATUS_X, STATUS_SCORE_Y); printf("        %6d", score);
	}
}

void checkLevelUp() {
	if (goal - linecnt <= 0) {
		LevelUpOn = 1;
		level += 1;
		linecnt = 0;
		PlaySound(NULL, 0, 0);

		getCurPath(bgm[4]);
		PlaySound(path2cpy, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
		StartLevelDraw();
		PlaySound(NULL, 0, 0);

		switch (level) { //레벨별로 속도를 조절해줌. 
		case 2:
			getCurPath(bgm[1]);
			speed = 50;
			goal += 10;
			break;
		case 3:
			getCurPath(bgm[2]);
			speed = 25;
			goal += 10;
			break;
		case 4:
			getCurPath(bgm[3]);
			speed = 10;
			goal += 10;
			break;
		case 5:
			getCurPath(bgm[0]);
			speed = 5;
			goal += 10;
			break;
		case 6:
			getCurPath(bgm[1]);
			speed = 4;
			goal += 10;
			break;
		case 7:
			getCurPath(bgm[2]);
			speed = 3;
			goal += 10;
			break;
		case 8:
			getCurPath(bgm[3]);
			speed = 2;
			goal += 10;
			break;
		case 9:
			getCurPath(bgm[0]);
			speed = 1;
			goal += 10;
			break;
		case 10:
			getCurPath(bgm[2]);
			speed = 0;
			goal += 10;
			break;
		}


		system("cls");
		ResetGameBoard();
		GameInfo();

		Sleep(1);
		DrawGameBoard();
		PlaySound(path2cpy, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NODEFAULT);

		nextBlock = rand() % 7;
		DrawBlock();

		LevelUpOn = 0; //레벨업 신호 끔

		gotoxy(STATUS_X, STATUS_GOAL_Y); printf(" GOAL  : %5d", goal - linecnt);
		gotoxy(STATUS_X, STATUS_SCORE_Y); printf("        %6d", score);
	}
}

void moveBlock(int key) {
	int i, j;

	switch (key) {
	case LEFT: // 왼쪽 방향
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (Blocks[curBlock][curBlockRotation][i][j] == 1) gameboard[curBlock_Y + i][curBlock_X + j] = EMPTY;  // 현재 좌표의 블럭을 지움
			}
		}
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (Blocks[curBlock][curBlockRotation][i][j] == 1) gameboard[curBlock_Y + i][curBlock_X + j - 1] = ACTIVE_BLOCK; // 한 칸 왼쪽 옆에 블록 생성
			}
		}
		curBlock_X--;
		break;
	case RIGHT: // 오른쪽 방향
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (Blocks[curBlock][curBlockRotation][i][j] == 1) gameboard[curBlock_Y + i][curBlock_X + j] = EMPTY; // 현재 좌표의 블럭을 지움
			}
		}
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (Blocks[curBlock][curBlockRotation][i][j] == 1) gameboard[curBlock_Y + i][curBlock_X + j + 1] = ACTIVE_BLOCK; // 한 칸 오른쪽 옆에 블록 생성
			}
		}
		curBlock_X++;
		break;
	case UP: // 회전
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (Blocks[curBlock][curBlockRotation][i][j] == 1) gameboard[curBlock_Y + i][curBlock_X + j] = EMPTY; // 현재 좌표의 블럭을 지움
			}
		}
		curBlockRotation = (curBlockRotation + 1) % 4; // 회전 값 1을 증가시키고, 0~3의 사이의 값을 받는다

		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (Blocks[curBlock][curBlockRotation][i][j] == 1) gameboard[curBlock_Y + i][curBlock_X + j] = ACTIVE_BLOCK; // 회전된 블럭 생성
			}
		}
		break;

	case DOWN: // Soft Drop
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (Blocks[curBlock][curBlockRotation][i][j] == 1) gameboard[curBlock_Y + i][curBlock_X + j] = EMPTY; // 현재 좌표의 블럭을 지움
			}
		}
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (Blocks[curBlock][curBlockRotation][i][j] == 1) gameboard[curBlock_Y + i + 1][curBlock_X + j] = ACTIVE_BLOCK; // 한 칸 밑에 블록 생성
			}
		}
		curBlock_Y++;
		break;

	case SPECIAL_MOTION: // 특수 동작 ( 다른 블록 혹은 바닥과 맞닿은 상태에서 한 칸 위로 올렸을 때 회전이 가능한 경우 동작시키는 특수 동작 )
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (Blocks[curBlock][curBlockRotation][i][j] == 1) gameboard[curBlock_Y + i][curBlock_X + j] = EMPTY; // 현재 좌표의 블럭을 지움
			}
		}
		curBlockRotation = (curBlockRotation + 1) % 4; // 다음 회전 값을 받는다.
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (Blocks[curBlock][curBlockRotation][i][j] == 1) gameboard[curBlock_Y + i - 1][curBlock_X + j] = ACTIVE_BLOCK; // 한 칸 위로 올라가 블럭을 생성
			}
		}
		curBlock_Y--;
		break;
	}
}

void dropBlock() {	// 블록을 떨어트림.
	if (CrushOn && CrushBlock(curBlock_X, curBlock_Y + 1, curBlockRotation) == TRUE) CrushOn = 0;	// 밑이 비어있으면 CrushOn 신호를 끈다.
	if (CrushOn && CrushBlock(curBlock_X, curBlock_Y + 1, curBlockRotation) == FALSE) {				// 밑이 비어있지 않고, CrushOn이 켜져 있으면
		for (int i = 0; i < GAMEBOARD_Y; i++) {
			for (int j = 0; j < GAMEBOARD_X; j++) {
				if (gameboard[i][j] == ACTIVE_BLOCK) gameboard[i][j] = INACTIVE_BLOCK;				// 현재 블록을 굳히고,
				curBlockColor = curBlock;															// 현재 블록의 색상을 저장하고,
			}
		}
		CrushOn = 0;			// CrushOn 끔
		checkLine();
		NewBlockOn = 1;																				// 새로운 블록생성 신호를 킴
		return;
	}

	if (CrushBlock(curBlock_X, curBlock_Y + 1, curBlockRotation) == TRUE) moveBlock(DOWN);			// 밑이 비어있으면 한 칸 이동
	if (CrushBlock(curBlock_X, curBlock_Y + 1, curBlockRotation) == FALSE) CrushOn++;				// 밑이 비어있지 않으면 CrushOn 켬
}

int gameOver() {
	int i;
	char* Text[4] = { "[1] 새 게임", "[2] 랭킹", "[3] 메인메뉴", "[4] 게임종료" };

	int x1 = 17, x2 = 19, y = 2;
	int X = 34, Y = 20;
	int textY = 20;

	for (i = 1; i < GAMEBOARD_X - 1; i++) {
		if (gameboard[3][i] > 0) {
			gameOverOn = 1;
			system("cls");
			PlaySound(NULL, 0, 0);
			if (soundOnePlay == 1) {
				getCurPath(bgm[5]);
				PlaySound(path2cpy, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
			}

			soundOnePlay = 0;
			gotoxy(x1, y + 0); printf("  ■■■■"); //게임오버 메세지 
			gotoxy(x1, y + 1); printf("■");
			gotoxy(x1, y + 2); printf("■    ■■");
			gotoxy(x1, y + 3); printf("■      ■");
			gotoxy(x1, y + 4); printf("  ■■■■");
			gotoxy(x1 + 12, y + 0); printf(" ■■■■");
			gotoxy(x1 + 12, y + 1); printf("■      ■");
			gotoxy(x1 + 12, y + 2); printf("■■■■■");
			gotoxy(x1 + 12, y + 3); printf("■      ■");
			gotoxy(x1 + 12, y + 4); printf("■      ■");
			gotoxy(x1 + 24, y + 0); printf("■■  ■■");
			gotoxy(x1 + 24, y + 1); printf("■  ■  ■");
			gotoxy(x1 + 24, y + 2); printf("■  ■  ■");
			gotoxy(x1 + 24, y + 3); printf("■      ■");
			gotoxy(x1 + 24, y + 4); printf("■      ■");
			gotoxy(x1 + 36, y + 0); printf("■■■■");
			gotoxy(x1 + 36, y + 1); printf("■");
			gotoxy(x1 + 36, y + 2); printf("■■■");
			gotoxy(x1 + 36, y + 3); printf("■");
			gotoxy(x1 + 36, y + 4); printf("■■■■■");

			gotoxy(x2, y + 6); printf("   ■■");
			gotoxy(x2, y + 7); printf(" ■    ■");
			gotoxy(x2, y + 8); printf("■      ■");
			gotoxy(x2, y + 9); printf(" ■    ■");
			gotoxy(x2, y + 10); printf("   ■■");
			gotoxy(x2 + 11, y + 6); printf("■      ■");
			gotoxy(x2 + 11, y + 7); printf(" ■    ■");
			gotoxy(x2 + 11, y + 8); printf("  ■  ■");
			gotoxy(x2 + 11, y + 9); printf("   ■■");
			gotoxy(x2 + 11, y + 10); printf("    ■");
			gotoxy(x2 + 23, y + 6); printf("■■■■");
			gotoxy(x2 + 23, y + 7); printf("■");
			gotoxy(x2 + 23, y + 8); printf("■■■");
			gotoxy(x2 + 23, y + 9); printf("■");
			gotoxy(x2 + 23, y + 10); printf("■■■■");
			gotoxy(x2 + 33, y + 6); printf("■■■");
			gotoxy(x2 + 33, y + 7); printf("■   ■");
			gotoxy(x2 + 33, y + 8); printf("■■■");
			gotoxy(x2 + 33, y + 9); printf("■   ■");
			gotoxy(x2 + 33, y + 10); printf("■    ■");

			gotoxy(x2 + 4, y + 13); printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
			gotoxy(x2 + 4, y + 14); printf("┃       YOUR SCORE : %6d     ┃", score);
			gotoxy(x2 + 4, y + 15); printf("┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓");
			SetColor(BLACK, WHITE);
			gotoxy(X, Y); printf("[1] 새 게임");
			SetColor(WHITE, BLACK);
			gotoxy(X, Y + 2); printf("[2] 랭킹");
			gotoxy(X, Y + 4); printf("[3] 메인메뉴");
			gotoxy(X, Y + 6); printf("[4] 게임종료");

			last_score = score;

			while (1) {							// 무한루프를 통해 방향키 입력 유무를 확인
				int key = _getch();

				switch (key) {
				case UP:
					getCurPath(bgm[7]);
					PlaySound(path2cpy, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
					if (Y > 20) {				// UP을 했을 경우
						gotoxy(X, Y);
						SetColor(WHITE, BLACK);
						printf("%s", Text[Y - textY]);
						gotoxy(X, Y -= 2);
						--textY;
						SetColor(BLACK, WHITE);
						printf("%s", Text[Y - textY]);
						SetColor(WHITE, BLACK);
					}
					break;

				case DOWN:
					getCurPath(bgm[7]);
					PlaySound(path2cpy, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
					if (Y < 26) {
						gotoxy(X, Y);
						SetColor(WHITE, BLACK);
						printf("%s", Text[Y - textY]);
						gotoxy(X, Y += 2);
						++textY;
						SetColor(BLACK, WHITE);
						printf("%s", Text[Y - textY]);
						SetColor(WHITE, BLACK);
					}
					break;
				case ENTER:						// ENTER -> 선택
					getCurPath(bgm[6]);
					PlaySound(path2cpy, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
					while (_kbhit()) _getch();
					return Y - textY;
					break;
				}
			}
		}
	}
}

int pause() {
	int x = 32, y = 9;
	char* pauseText[3] = { "[1] 계속하기","[2] 새 게임","[3] 메인메뉴" };
	int textY = 13;
	int Y = 13;

	gotoxy(x, y + 0); printf("┏━━━━━━━━━━━━━━━━━━━━━━━┓");
	gotoxy(x, y + 1); printf("┃        일시정지       ┃");
	gotoxy(x, y + 2); printf("┃───────────────────────┃");
	gotoxy(x, y + 3); printf("┃                       ┃");
	gotoxy(x, y + 4); printf("┃                       ┃");
	gotoxy(x, y + 5); printf("┃                       ┃");
	gotoxy(x, y + 6); printf("┃                       ┃");
	gotoxy(x, y + 7); printf("┃                       ┃");
	gotoxy(x, y + 8); printf("┃                       ┃");
	gotoxy(x, y + 9); printf("┃                       ┃");
	gotoxy(x, y + 10); printf("┗━━━━━━━━━━━━━━━━━━━━━━━┛");

	SetColor(BLACK, WHITE);
	gotoxy(x + 6, Y); printf("[1] 계속하기");
	SetColor(WHITE, BLACK);
	gotoxy(x + 6, Y + 2); printf("[2] 새 게임");
	gotoxy(x + 6, Y + 4); printf("[3] 메인메뉴");

	while (1) {							// 무한루프를 통해 방향키 입력 유무를 확인
		int key = _getch();

		switch (key) {
		case UP:
			if (Y > 13) {				// UP을 했을 경우
				gotoxy(x + 6, Y);
				SetColor(WHITE, BLACK);
				printf("%s", pauseText[Y - textY]);
				gotoxy(x + 6, Y -= 2);
				--textY;
				SetColor(BLACK, WHITE);
				printf("%s", pauseText[Y - textY]);
				SetColor(WHITE, BLACK);
			}
			break;

		case DOWN:
			if (Y < 17) {
				gotoxy(x + 6, Y);
				SetColor(WHITE, BLACK);
				printf("%s", pauseText[Y - textY]);
				gotoxy(x + 6, Y += 2);
				++textY;
				SetColor(BLACK, WHITE);
				printf("%s", pauseText[Y - textY]);
				SetColor(WHITE, BLACK);
			}
			break;
		case ENTER:						// ENTER -> 선택
			if (Y - 11 == 2) {
				for (int i = 3; i < GAMEBOARD_Y - 1; i++) {			// 오른쪽 벽 다시 그림
					gotoxy(2 * (GAMEBOARD_X + GAMEBOARD_X_ADJ - 1), i);
					SetColor(WHITE, WHITE);
					printf("▒");
					SetColor(WHITE, BLACK);
				}
				for (int i = 9; i < GAMEBOARD_Y - 5; i++) {			// PAUSE 창을 지움
					gotoxy(2 * (GAMEBOARD_X + GAMEBOARD_X_ADJ), i);
					printf("                       ");

				}
				GameInfo();

				for (int i = 1; i < 3; i++) {		// 다음 블록 생성
					for (int j = 0; j < 4; j++) {
						if (Blocks[nextBlock][0][i][j] == 1) {
							switch (nextBlock + 1) {	// 블록 타입에 따라 색 설정
							case 1:
								SetColor(BLUE, BLUE);
								gotoxy(STATUS_X + 4 + (2 * j), i + 10);
								printf("■");
								SetColor(WHITE, BLACK);
								break;
							case 2:
								SetColor(CYAN, CYAN);
								gotoxy(STATUS_X + 4 + (2 * j), i + 10);
								printf("■");
								SetColor(WHITE, BLACK);
								break;
							case 3:
								SetColor(BROWN, BROWN);
								gotoxy(STATUS_X + 4 + (2 * j), i + 10);
								printf("■");
								SetColor(WHITE, BLACK);
								break;
							case 4:
								SetColor(YELLOW, YELLOW);
								gotoxy(STATUS_X + 4 + (2 * j), i + 10);
								printf("■");
								SetColor(WHITE, BLACK);
								break;
							case 5:
								SetColor(GREEN, GREEN);
								gotoxy(STATUS_X + 4 + (2 * j), i + 10);
								printf("■");
								SetColor(WHITE, BLACK);
								break;
							case 6:
								SetColor(RED, RED);
								gotoxy(STATUS_X + 4 + (2 * j), i + 10);
								printf("■");
								SetColor(WHITE, BLACK);
								break;
							case 7:
								SetColor(LIGHTPURPLE, LIGHTPURPLE);
								gotoxy(STATUS_X + 4 + (2 * j), i + 10);
								printf("■");
								SetColor(WHITE, BLACK);
								break;
							}
						}
						else {
							gotoxy(STATUS_X + 4 + (2 * j), i + 10);
							printf("  ");
						}
					}
				}
			}
			return Y - 11;
			break;

		case ESC:
			for (int i = 3; i < GAMEBOARD_Y - 1; i++) {			// 오른쪽 벽 다시 그림
				gotoxy(2 * (GAMEBOARD_X + GAMEBOARD_X_ADJ - 1), i);
				SetColor(WHITE, WHITE);
				printf("▒");
				SetColor(WHITE, BLACK);
			}
			for (int i = 9; i < GAMEBOARD_Y - 5; i++) {			// PAUSE 창을 지움
				gotoxy(2 * (GAMEBOARD_X + GAMEBOARD_X_ADJ), i);
				printf("                       ");
				GameInfo();

				for (int i = 1; i < 3; i++) {		// 다음 블록 생성
					for (int j = 0; j < 4; j++) {
						if (Blocks[nextBlock][0][i][j] == 1) {
							switch (nextBlock + 1) {	// 블록 타입에 따라 색 설정
							case 1:
								SetColor(BLUE, BLUE);
								gotoxy(STATUS_X + 4 + (2 * j), i + 10);
								printf("■");
								SetColor(WHITE, BLACK);
								break;
							case 2:
								SetColor(CYAN, CYAN);
								gotoxy(STATUS_X + 4 + (2 * j), i + 10);
								printf("■");
								SetColor(WHITE, BLACK);
								break;
							case 3:
								SetColor(BROWN, BROWN);
								gotoxy(STATUS_X + 4 + (2 * j), i + 10);
								printf("■");
								SetColor(WHITE, BLACK);
								break;
							case 4:
								SetColor(YELLOW, YELLOW);
								gotoxy(STATUS_X + 4 + (2 * j), i + 10);
								printf("■");
								SetColor(WHITE, BLACK);
								break;
							case 5:
								SetColor(GREEN, GREEN);
								gotoxy(STATUS_X + 4 + (2 * j), i + 10);
								printf("■");
								SetColor(WHITE, BLACK);
								break;
							case 6:
								SetColor(RED, RED);
								gotoxy(STATUS_X + 4 + (2 * j), i + 10);
								printf("■");
								SetColor(WHITE, BLACK);
								break;
							case 7:
								SetColor(LIGHTPURPLE, LIGHTPURPLE);
								gotoxy(STATUS_X + 4 + (2 * j), i + 10);
								printf("■");
								SetColor(WHITE, BLACK);
								break;
							}
						}
						else {
							gotoxy(STATUS_X + 4 + (2 * j), i + 10);
							printf("  ");
						}
					}
				}
			}
			return;
			break;
		}
	}
}

void rankBoard() {

	char* Number[10] = { "１","２","３","４","５","６","７","８","９","10" };
	const x = 5;
	int i;

	system("cls");
	gotoxy(3, 1); printf("┏━━━━━━━━━━━━━━━┓");
	gotoxy(3, 2); printf("┃ R A N K I N G ┃");
	gotoxy(3, 3); printf("┗━━━━━━━━━━━━━━━┛");

	gotoxy(3, 5); printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
	gotoxy(3, 6); printf("No       ID              SCORE              LEVEL           LINES");
	gotoxy(3, 7); printf("－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－");

	for (int i = 1; i < 11; i++) {
		gotoxy(3, 6 + 2 * i); printf("%s", Number[i - 1]);

	}

	gotoxy(3, 27); printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
	RankSort();
	
	for (i = 0; i < 10; i++) {
			gotoxy(x + 6, (i + 1) * 2 + 6); printf("--");
			gotoxy(x + 26, (i + 1) * 2 + 6); printf("--");
			gotoxy(x + 43, (i + 1) * 2 + 6); printf("--");
			gotoxy(x + 60, (i + 1) * 2 + 6); printf("--");
	}

	for (i = 0; i < 10; i++) {
		if (rank[i].level != 0) {
			gotoxy(x + 6, (i + 1) * 2 + 6); printf("%-10s", rank[i].name);
			gotoxy(x + 18, (i + 1) * 2 + 6); printf("%10d", rank[i].score);
			gotoxy(x + 40, (i + 1) * 2 + 6); printf("%5d", rank[i].level);
			gotoxy(x + 52, (i + 1) * 2 + 6); printf("%10d", rank[i].lines);
			Sleep(200);
		}
	}
}

void AddRank(int level, int score, int lines)
{
	gotoxy(34, 13);
	fgets(rank[10].name, 10, stdin);

	rank[10].level = level;
	rank[10].score = score;
	rank[10].lines = lines;
	RankSort();
	rankFileSave();
}

void RankSort(void)
{
	int i, j, cnt = 0;
	struct info temp;

	for (i = 0; i < 10; i++)
	{
		for (j = 0; j < 11; j++)
		{
			if (rank[j].score < rank[j + 1].score)
			{
				temp = rank[j];
				rank[j] = rank[j + 1];
				rank[j + 1] = temp;
			}
		}
	}
	rankFileSave();
}


void rankPutCheck() {

	int x = 26, y = 9;
	char* rankPutCheckText[3] = { "[1] YES","[2] NO" };
	int textX = 32;
	int X = 32, Y = 13;

	gotoxy(x, y - 1); printf("┏━━━━━━━━━━━━━━━━━━━━━━━┓");
	gotoxy(x, y + 0); printf("┃                       ┃");
	gotoxy(x, y + 1); printf("┃       현재 기록을     ┃");
	gotoxy(x, y + 2); printf("┃    저장하시겠습니까?  ┃");
	gotoxy(x, y + 3); printf("┃                       ┃");
	gotoxy(x, y + 4); printf("┃                       ┃");
	gotoxy(x, y + 5); printf("┃                       ┃");
	gotoxy(x, y + 6); printf("┗━━━━━━━━━━━━━━━━━━━━━━━┛");

	SetColor(BLACK, WHITE);
	gotoxy(X, Y); printf("[1] YES");
	SetColor(WHITE, BLACK);
	gotoxy(X + 9, Y); printf("[2] NO");

	while (1) {							// 무한루프를 통해 방향키 입력 유무를 확인
		int key = _getch();

		switch (key) {
		case LEFT:
			getCurPath(bgm[7]);
			PlaySound(path2cpy, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
			if (X > 32) {				// LEFT을 했을 경우
				gotoxy(X, Y);
				SetColor(WHITE, BLACK);
				printf("%s", rankPutCheckText[X - textX]);
				gotoxy(X -= 9, Y);
				textX -= 8;
				SetColor(BLACK, WHITE);
				printf("%s", rankPutCheckText[X - textX]);
				SetColor(WHITE, BLACK);
			}
			break;
		case RIGHT:
			getCurPath(bgm[7]);
			PlaySound(path2cpy, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
			if (X < 41) {
				gotoxy(X, Y);
				SetColor(WHITE, BLACK);
				printf("%s", rankPutCheckText[X - textX]);
				gotoxy(X += 9, Y);
				textX += 8;
				SetColor(BLACK, WHITE);
				printf("%s", rankPutCheckText[X - textX]);
				SetColor(WHITE, BLACK);
			}
			break;
		case ENTER:						// ENTER -> 선택
			getCurPath(bgm[6]);
			PlaySound(path2cpy, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
			if (X - textX == 0) {
				gotoxy(x, y - 1); printf("┏━━━━━━━━━━━━━━━━━━━━━━━┓");
				gotoxy(x, y + 0); printf("┃                       ┃");
				gotoxy(x, y + 1); printf("┃   이름을 입력하세요   ┃");
				gotoxy(x, y + 2); printf("┃    ( 최대 10 자리 )   ┃");
				gotoxy(x, y + 3); printf("┃                       ┃");
				gotoxy(x, y + 4); printf("┃  ID :                 ┃");
				gotoxy(x, y + 5); printf("┃                       ┃");
				gotoxy(x, y + 6); printf("┗━━━━━━━━━━━━━━━━━━━━━━━┛");

				AddRank(level, score, acclinecnt);
				getCurPath(bgm[6]);
				PlaySound(path2cpy, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
				rankBoard();
				rankFileSave();
				_getch();
				PlaySound(path2cpy, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);

				system("cls");
				return;

			}
			else if (X - textX == 1) {
				rankBoard();
				_getch();
				getCurPath(bgm[6]);
				PlaySound(path2cpy, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);

				system("cls");
				return;
			}
			break;
		}
	}
}


void setting() {
	char* number[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" };
	char score[25];
	char level[25];
	char lines[25];
	char playername[10];

	GetEnvironmentVariable("APPDATA", path, 200);
	strcat_s(path, sizeof(path), "\\Tetris");
	mkdir(path);
	strcat_s(path, sizeof(path), "\\Ranking.ini");
	if (_access(path, 0) == 0)
	{
		for (int i = 0; i < 10; i++)
		{
			GetPrivateProfileString(number[i], TEXT("score"), "error", score, sizeof(score), path);
			GetPrivateProfileString(number[i], TEXT("level"), "error", level, sizeof(level), path);
			GetPrivateProfileString(number[i], TEXT("lines"), "error", lines, sizeof(lines), path);
			GetPrivateProfileString(number[i], TEXT("name"), "error", playername, sizeof(playername), path);

			rank[i].score = atoi(score);
			rank[i].level = atoi(level);
			rank[i].lines = atoi(lines);
			strcpy_s(rank[i].name, 10, playername);

			if (strcmp(score, "error") == 0) {
				strcpy_s(score, sizeof(score), "--");
				strcpy_s(level, sizeof(level), "--");
				strcpy_s(lines, sizeof(lines), "--");
				strcpy_s(playername, sizeof(playername), "--");

			}
		}
	}
}

void rankFileSave() {
	char* number[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" };
	char score[25];
	char level[25];
	char lines[25];
	char playername[10];

	for (int i = 0; i < 10; i++)
	{
		_itoa(rank[i].score, score, 10);
		_itoa(rank[i].level, level, 10);
		_itoa(rank[i].lines, lines, 10);
		strcpy_s(playername, 10, rank[i].name);
		WritePrivateProfileString(number[i], TEXT("score"), score, path);
		WritePrivateProfileString(number[i], TEXT("level"), level, path);
		WritePrivateProfileString(number[i], TEXT("lines"), lines, path);
		WritePrivateProfileString(number[i], TEXT("name"), playername, path);
	}
}

void getCurPath(bgm) {

	FILE* fp;
	char* cmd[100];

	sprintf_s(cmd, sizeof(cmd), "cd");
	fp = _popen(cmd, "r");
	if (NULL == fp)
	{
		printf("popen() 실패");
		return;
	}

	fgets(path2cpy, 100, fp);

	strncpy_s(path2cpy, sizeof(path2cpy), path2cpy, strlen(path2cpy) - 1);

	if (bgm != NULL) {
		strcat(path2cpy, bgm);
	}
}