#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <process.h>
#include <string.h>
#include<mmsystem.h>
#pragma comment(lib, "winmm.lib")



#define LEFT 75    // ����Ű �� ���� ( �·� �̵� )																int main() {
#define RIGHT 77   // ����Ű �� ���� ( ��� �̵� )																int key;
#define UP 72	   // ����Ű �� ���� ( ȸ�� )																	key = _getch();
#define DOWN 80    // ����Ű �� ���� ( Soft Drop )																if (key == 224) {          ���� �ڵ带 ����
#define SPACE 32   // SPACEBAR Ű ���� ( Hard Drop )															key = _getch();            KEY ���� ��
#define ESC 27     // ESC Ű ���� ( �Ͻ����� )																	}
#define ENTER 13   //																							printf("%d", key);			
//																												return 0;
//																												}5

#define SPECIAL_MOTION 777 // ����ȸ��. ����� ���̳� ��ϰ� ���� ���¿��� �� ĭ ���� �÷� ȸ���� ���� �� �� 

#define TRUE 1 
#define FALSE 0

#define ACTIVE_BLOCK -2 // ��
#define CEILING -1	// õ��
#define EMPTY 0		// �� ����
#define WALL 1		// ��
#define INACTIVE_BLOCK 2 // �̵��� �Ϸ�� ��

#define GAMEBOARD_X 12		// ������ ���� ũ��
#define GAMEBOARD_Y 25		// ������ ���� ũ��
#define GAMEBOARD_X_ADJ	5	// ������ X�� ��ġ ����
#define GAMEBOARD_Y_ADJ	2	// ������ Y�� ��ġ ����

#define STATUS_X 2 * (GAMEBOARD_X_ADJ + GAMEBOARD_X + 1) // ���� ���� ��ġ ����

// �Լ����� -----------------------------------------------------------------------------------


void GameStart(); // ���� ���� �ʱ�ȭ, ���� ù �����ϴ� �Լ�
void TitleMenuDraw(); // Ÿ��Ʋ �޴��� �׸�
void WINAPI AnimationStar(void* arg); // �� �ִϸ��̼� ������ �Լ�
void InfoMenu(); // ���� �޴�
void ResetGameBoard(); // ���Ӻ��带 �ʱ�ȭ
void ResetGameBoardCopy(); // copy ���Ӻ��带 �ʱ�ȭ
void DrawGameBoard(); // ���Ӻ��带 �׸�
void GameInfo(); // ���� �ΰ� ������ �׸�
void StartLevelDraw(); // ���� ������ �׸�
void WINAPI StopwatchStart(void* arg); // ��ž��ġ �Լ�

void DrawBlock(); // ���� �׸�
void CheckKeyhit(); // Ű �Է��� Ȯ���ϰ� Ű ������ �޾Ƴ�
void checkLine(); // ���� Ŭ���� �ϴ� �Լ�
void checkLevelUp(); // ���� �� ��Ű�� �Լ�
void moveBlock(); // ���� ������
void dropBlock(); // ���� ����Ʈ��

void rankBoard(); // ��ŷ������ �׸�
void AddRank(int level, int score, int lines); // ��ũ ������ ������
void RankSort(void); // ��ŷ ������ ����

void rankPutCheck(); // ��ŷ �Է��� �������� ���� ���� Ȯ��
void setting(); // ��ŷ ������ ���Ϸκ��� �ҷ��� 
void rankFileSave(); // ��ŷ ������ ���Ϸ� ������
void getCurPath(bgm); // ���� ���丮 ��θ� �ҷ����� bgm ��ο� �̾����
int pause(); // �Ͻ�����
int TitleMenuText(); // Ÿ��Ʋ �޴��� �ؽ�Ʈ�� �׸�
int CrushBlock(); // �� �浹 Ȯ��

int gameOver(); // ���ӿ���


// ----------------------------------------------------------------------------------------------

char path[400]; // appdata ��� ����
char path2cpy[400]; // �����ο� bgm ��� ����
char* bgm[8] = { "\\Sound\\Bradinsky.wav", "\\Sound\\Karinka.wav","\\Sound\\Loginska.wav", "\\Sound\\4.wav", "\\Sound\\LevelClear.wav","\\Sound\\GameOver.wav", 
				 "\\Sound\\select.wav", "\\Sound\\menu.wav" }; // bgm ��� ����

int STATUS_GOAL_Y;  // ��ǥ ���� Y ��ġ ����
int STATUS_SCORE_Y; // ���� ���� Y ��ġ ����
int STATUS_LEVEL_Y; // ���� ���� Y ��ġ ����

int level;		// ���� ����
int score;			// ���� ���ھ�
int speed;			// ���� �ӵ�
int goal;		// ��ǥ ����

int curBlockRotation;		// ���� ����
int absBlock_X, absBlock_Y;		// ���� ���� ��ǥ
int curBlock_X, curBlock_Y;		// ���� ���� ��ǥ
int curBlock;		// ���� ����
int curBlockColor;  // ���� ������ ������ ����
int nextBlock;		// ���� ����
int linecnt;		// ������ ���� ���� ����
int acclinecnt;		// ������ ���� ���� ���� ����
int NewBlockOn = 0; // ���ο� ����� �������� ���� ��ȣ
int CrushOn = 0; // ���� ����� �浹���� ������ ���� ��ȣ
int spaceOn = 0; // harddrop�� �� �������� ���� ��ȣ
int LevelUpOn = 0; // �������� �� �������� ���� ��ȣ
int gameOverOn = 0;		// ���� ���� ��ȣ

int best_score = 0; // �ְ� ����
int last_score = 0; // �ֱ� ������ ����
int soundOnePlay = 1; // ���带 �ѹ��� ����

int keyhit; // Ű ����

int pauseSelectMenu; // pause���� � �޴� �����ߴ��� ����

int gameboard[GAMEBOARD_Y][GAMEBOARD_X];		// ���� ���� �迭�� ���� ����
int gameboard_cpy[GAMEBOARD_Y][GAMEBOARD_X];	// ���� ���� ������ �迭�� ����ϰ�, gameboard_cpy �� ����ȴ�.
												// gameboard �� gameboard_cpy �� �� �� cpy �� ���� �޶��� ���� ���� ��ģ��.

HANDLE thread1;	// �� ��¦�̴� �ִϸ��̼ǰ� �޴� ���� ��ɿ��� gotoxy �Լ��� ���ÿ� ����� �� �ֵ��� ������ ���
HANDLE Stopwatch; // ���� �÷��̿� ��ž��ġ�� ���ÿ� ����� �� �ֵ��� ������ ���

int Blocks[7][4][4][4] = {
	// ��
	{{0,0,0,0, 0,1,1,0, 0,1,1,0, 0,0,0,0},{0,0,0,0, 0,1,1,0, 0,1,1,0, 0,0,0,0},
	 {0,0,0,0, 0,1,1,0, 0,1,1,0, 0,0,0,0},{0,0,0,0, 0,1,1,0, 0,1,1,0, 0,0,0,0}},
	 // ��
	 {{0,0,0,0, 0,0,0,0, 1,1,1,1, 0,0,0,0},{0,1,0,0, 0,1,0,0, 0,1,0,0, 0,1,0,0},
	  {0,0,0,0, 0,0,0,0, 1,1,1,1, 0,0,0,0},{0,1,0,0, 0,1,0,0, 0,1,0,0, 0,1,0,0}},
	  // ����
	  {{0,0,0,0, 1,1,0,0, 0,1,1,0, 0,0,0,0},{0,0,0,0, 0,0,1,0, 0,1,1,0, 0,1,0,0},
	   {0,0,0,0, 1,1,0,0, 0,1,1,0, 0,0,0,0},{0,0,0,0, 0,0,1,0, 0,1,1,0, 0,1,0,0}},
	   // ���� �ݴ�
	   {{0,0,0,0, 0,1,1,0, 1,1,0,0, 0,0,0,0},{0,0,0,0, 1,0,0,0, 1,1,0,0, 0,1,0,0},
		{0,0,0,0, 0,1,1,0, 1,1,0,0, 0,0,0,0},{0,0,0,0, 1,0,0,0, 1,1,0,0, 0,1,0,0}},
		// �� �ݴ�
		{{0,0,0,0, 0,0,1,0, 1,1,1,0, 0,0,0,0},{0,0,0,0, 1,1,0,0, 0,1,0,0, 0,1,0,0},
		 {0,0,0,0, 0,0,0,0, 1,1,1,0, 1,0,0,0},{0,0,0,0, 0,1,0,0, 0,1,0,0, 0,1,1,0}},
		 // ��
		 {{0,0,0,0, 1,0,0,0, 1,1,1,0, 0,0,0,0},{0,0,0,0, 0,1,0,0, 0,1,0,0, 1,1,0,0},
		  {0,0,0,0, 0,0,0,0, 1,1,1,0, 0,0,1,0},{0,0,0,0, 0,1,1,0, 0,1,0,0, 0,1,0,0}},
		  // ��
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
	system("title Tetris");					// Console ������ Tetris�� ����
	system("mode con cols=80 lines=30");	// Console Size�� ���� 80, ���� 30���� ����
}

void gotoxy(int x, int y) {												// gotoxy �Լ��� ����.
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

typedef enum { BLACK, BLUE, GREEN, CYAN, RED, PURPLE, BROWN, LIGHTGRAY, DARKGRAY, LIGHTBLUE, LIGHTGREEN, LIGHTCYAN, LIGHTRED, LIGHTPURPLE, YELLOW, WHITE } Color;

void SetColor(unsigned short text, unsigned short back) {				// �ؽ�Ʈ Ȥ�� ���� Color ���
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), text | (back << 4));
}

typedef enum { NOCURSOR, SOLIDCURSOR, NORMALCURSOR } CursorType;   // �������� ���� Ŀ��Ÿ�� ���� (Ŀ�� ����� �Լ��� ���)

void CursorTypeOption(CursorType c) {	// Ŀ��Ÿ�� ����
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
			thread1 = CreateThread(NULL, 0, AnimationStar, NULL, 0, NULL); 	// �� �ִϸ��̼��� ���� ������ ����
		} while (FALSE);


		int MenuCode = TitleMenuText();
		if (MenuCode == 0) {
			srand((unsigned)time(NULL));
			CursorTypeOption(NOCURSOR);
			GameStart(); // ���ӽ���
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
			InfoMenu(); // ����
		}
		else if (MenuCode == 3) {
			return 0; // ���� ����
		}
		system("cls");
	}
	return 0;
}

int TitleMenuText() {
	int x = 35;
	int y = 16;

	gameOverOn = 0;

	gotoxy(x - 2, y); printf("> ���ӽ���");
	gotoxy(x - 2, y + 1); printf("    ��ŷ    ");
	gotoxy(x - 2, y + 2); printf("   ����  ");
	gotoxy(x - 2, y + 3); printf("    ����   ");
	gotoxy(x - 22, y + 12); printf("< Copyright 2019. PARK JIN SUNG All Rights Reserved >");
	gotoxy(x - 6, y + 8); printf("Made by PARK JIN SUNG");
	gotoxy(x - 10, y + 9); printf("@email : jerry03122@naver.com");
	gotoxy(x - 6, y + 10); printf("Discord : Reibeu#4227");

	Sleep(1);

	CursorTypeOption(0);
	while (1) {							// ���ѷ����� ���� ����Ű �Է� ������ Ȯ��
		int key = _getch();
		//SuspendThread(thread1);

		switch (key) {
		case UP:
			getCurPath(bgm[7]);
			PlaySound(path2cpy, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
			if (y > 16) {				// UP�� ���� ��� >�� ����� ��ĭ �ؿ� > �� ������
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
		case ENTER:						// ENTER -> ����
			getCurPath(bgm[6]);
			PlaySound(path2cpy, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
			SuspendThread(thread1);
			while (_kbhit()) _getch();
			return y - 16;				// ���� ��ġ�� 17�̹Ƿ� 0,1,2 ���� �޴´�.
		}
		//ResumeThread(thread1);
	}

}

void TitleMenuDraw() {


	gotoxy(14, 3); printf("��������������������������������������������������������������������������������������������������");
	Sleep(100);
	gotoxy(14, 4); printf("��  �ߡߡ�  �ߡߡ�  �ߡߡ�   �ߡ�     ��   �ߡ�  ��");
	Sleep(100);
	gotoxy(14, 5); printf("��    ��    ��        ��     �� ��    ��  ��     ��");
	Sleep(100);
	gotoxy(14, 6); printf("��    ��    �ߡߡ�    ��     �ߡ�     ��   �ߡ�  ��");
	Sleep(100);
	gotoxy(14, 7); printf("��    ��    ��        ��     �� ��    ��      �� ��");
	Sleep(100);
	gotoxy(14, 8); printf("��    ��    �ߡߡ�    ��     ��  ��   ��   �ߡ�  ��");
	Sleep(100);
	gotoxy(14, 9); printf("��������������������������������������������������������������������������������������������������");
	gotoxy(14, 10); printf(" Ver 0.2");


}

typedef struct _Area {  // �� ��ǥ ����ü ����
	int x1;
	int y1;
} Area;

Area MakingStar(int Area_X, int Area_Y, int startX, int startY) { // ���� ����� �Լ�
																	// ������ǥ�� �� ��ǥ�� �޾� ������ �����ϰ�, �����Լ��� ����
	Area xy;														// ���� ��ǥ�� �޾� �� ��ǥ�� ���� �����Ѵ�.
	xy.x1 = (rand() % Area_X) + startX;
	xy.y1 = (rand() % Area_Y) + startY;

	gotoxy(xy.x1, xy.y1);

	printf("*");

	return xy;
}

void RemoveStar(Area xy) {	// �� ����� �Լ� ���� ��µƴ� xy(��ǥ)�� �޾� ���� �����.
	gotoxy(xy.x1, xy.y1);
	printf(" ");
	Sleep(50);
}

void WINAPI AnimationStar(void* arg) {  // �� ����� �Լ��� ����� �Լ��� �޾� �����ϴ� �Լ�
	Area xy[10];						// cnt�� 0~ 5 ���� �������� �޾� ���� ��� �� ������ �������� ����
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
	// Stopwatch = CreateThread(NULL, 0, StopwatchStart, NULL, 0, NULL); // �����ġ ������. ��������� ���ÿ� ����
	Sleep(1);
	DrawGameBoard();


	nextBlock = rand() % 7;
	DrawBlock();
}

void InfoMenu() {  // ���� �޴�
	SuspendThread(thread1);
	system("cls");
	int x = 15, y = 5;
	gotoxy(x - 7, y - 4); printf("< ���۹� >");
	gotoxy(x, y - 2); printf("                ��   : ȸ��               ");
	gotoxy(x, y - 1); printf("              ��  �� : ���� / ������        ");
	gotoxy(x, y + 0); printf("                ��   : Soft Drop           ");
	gotoxy(x, y + 2); printf("               SPACE : Hard Drop           ");
	gotoxy(x, y + 3); printf("                ESC   : �Ͻ�����           ");


	gotoxy(x - 7, y + 5); printf("<  Tip  >");
	gotoxy(x - 5, y + 7); printf("�� ������ �����մϴ�.");
	gotoxy(x - 5, y + 9); printf("�� ���� ����, ������ Ű�� ������ �� �ֽ��ϴ�.");
	gotoxy(x - 5, y + 11); printf("�� �� ����Ű�� ����Ͽ� ���� ȸ���ϰ� ���ۿ� ���߼���.");
	gotoxy(x - 5, y + 13); printf("�� �׼� Ű�� �̿��Ͽ� ��Ű�� ���� ����Ʈ�� �� �ֽ��ϴ�");
	gotoxy(x - 5, y + 14); printf("    + Hard Drop�� �ϸ� ���ʽ� ������ �־����ϴ�.");
	gotoxy(x - 5, y + 16); printf("�� �� ������ Ŭ���� �Ͽ� ������ ��������");
	gotoxy(x - 5, y + 17); printf("    + ������ �Ѳ����� ���� Ŭ���� �� ����");
	gotoxy(x - 5, y + 18); printf("      �� ���� ���ʽ� ������ �־����ϴ�.");
	gotoxy(x - 5, y + 20); printf("�� ���� �� ���� ����Ͽ� ���� �ϼ���.");
	SetColor(LIGHTRED, BLACK);
	gotoxy(x - 2, y + 22); printf("����������, ������ ������ ��⼼�� !");
	SetColor(WHITE, BLACK);


	while (1) {						// ���ѷ����� ���鼭 �ݺ��� Ű ���� ENTER�� ��� �ݺ��� �����ϰ� �Լ��� ����
		int key = _getch();
		if (key == ENTER) {
			getCurPath(bgm[6]);
			PlaySound(path2cpy, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
			ResumeThread(thread1);
			break;
		}
	}
}

void ResetGameBoard() {		// ������
	int i, j;
	for (i = 0; i < GAMEBOARD_Y; i++) {
		for (j = 0; j < GAMEBOARD_X; j++) {
			gameboard[i][j] = 0;
			gameboard_cpy[i][j] = 100;
		}
	}

	for (i = 1; i < GAMEBOARD_Y - 1; i++) {			// ���� �� ����
		gameboard[i][0] = WALL;
		gameboard[i][GAMEBOARD_X - 1] = WALL;
	}
	for (j = 0;j < GAMEBOARD_X;j++) {				// �ٴ� ���� 
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

void DrawGameBoard() {					// ���� �׸�
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
					printf("��");
					SetColor(WHITE, BLACK);
					break;
				case CEILING:
					printf("..");
					break;
				case ACTIVE_BLOCK:
					switch (curBlock + 1) {
					case 1:
						SetColor(BLUE, BLUE);
						printf("��");
						SetColor(WHITE, BLACK);
						break;
					case 2:
						SetColor(CYAN, CYAN);
						printf("��");
						SetColor(WHITE, BLACK);
						break;
					case 3:
						SetColor(BROWN, BROWN);
						printf("��");
						SetColor(WHITE, BLACK);
						break;
					case 4:
						SetColor(YELLOW, YELLOW);
						printf("��");
						SetColor(WHITE, BLACK);
						break;
					case 5:
						SetColor(GREEN, GREEN);
						printf("��");
						SetColor(WHITE, BLACK);
						break;
					case 6:
						SetColor(RED, RED);
						printf("��");
						SetColor(WHITE, BLACK);
						break;
					case 7:
						SetColor(LIGHTPURPLE, LIGHTPURPLE);
						printf("��");
						SetColor(WHITE, BLACK);
						break;
					}
					break;
				case INACTIVE_BLOCK:
					switch (curBlockColor + 1) {
					case 1:
						SetColor(BLUE, BLUE);
						printf("��");
						SetColor(WHITE, BLACK);
						break;
					case 2:
						SetColor(CYAN, CYAN);
						printf("��");
						SetColor(WHITE, BLACK);
						break;
					case 3:
						SetColor(BROWN, BROWN);
						printf("��");
						SetColor(WHITE, BLACK);
						break;
					case 4:
						SetColor(YELLOW, YELLOW);
						printf("��");
						SetColor(WHITE, BLACK);
						break;
					case 5:
						SetColor(GREEN, GREEN);
						printf("��");
						SetColor(WHITE, BLACK);
						break;
					case 6:
						SetColor(RED, RED);
						printf("��");
						SetColor(WHITE, BLACK);
						break;
					case 7:
						SetColor(LIGHTPURPLE, LIGHTPURPLE);
						printf("��");
						SetColor(WHITE, BLACK);
						break;
					}
				}
			}
		}
	}

	for (i = 0; i < GAMEBOARD_Y; i++) {				// �������� ������ ������.
		for (j = 0; j < GAMEBOARD_X; j++) {
			gameboard_cpy[i][j] = gameboard[i][j];
		}
	}
}

void GameInfo() {								//���� ���� ǥ�ø� ��Ÿ���� �Լ�  
	int y = 3;									// level, goal, score�� ���ӿ��� ���� �ٲ� �� �ֱ⿡, �� y ���� ���� �����Ѵ�.
												// ���� ���� ǥ�� ��ġ�� �ٲ���� �ش� �Լ����� �������� �ʾƵ� �ǵ��� ����.
	best_score = rank[0].score;
	gotoxy(STATUS_X + 24, y - 1); printf("���������� ====���������� ");
	gotoxy(STATUS_X + 24, y + 0); printf("��   TETRIS   �� ");
	gotoxy(STATUS_X + 24, y + 1); printf("���������� ====���������� ");

	gotoxy(STATUS_X, y + 0); printf("���� Lv ���� ");
	gotoxy(STATUS_X, y + 1); printf("��      �� ");
	gotoxy(STATUS_X + 2, STATUS_LEVEL_Y = y + 1); printf("%3d", level);
	gotoxy(STATUS_X, y + 2); printf("���������������� ");

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

void StartLevelDraw() {		// ���� ������ ���� ǥ��
	system("cls");

	gotoxy(30, 12); printf("������������������������������������");
	gotoxy(30, 13); printf("��                ��");
	gotoxy(30, 14); printf("��    Level %3d   ��", level);
	gotoxy(30, 15); printf("��                ��");
	gotoxy(30, 16); printf("������������������������������������");

	if (LevelUpOn) {
		for (int i = 0; i < 4;i++) {
			gotoxy(31, 11);
			printf("                 ");
			gotoxy(31, 17);
			printf("                 ");
			Sleep(200);

			gotoxy(31, 11);
			printf("�١�LEVEL UP!�١�");
			gotoxy(31, 17);
			printf("�١�SPEED UP!�١�");
			Sleep(200);
		}
	}

	Sleep(4000);
}

void WINAPI StopwatchStart(void* arg) {		// �����ġ �Լ�
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

void DrawBlock() {  // ���ο� ��� ����
	int i, j;

	curBlock_X = (GAMEBOARD_X / 2) - 2;  // ������ ���
	curBlock_Y = 0;

	curBlock = nextBlock;		// ���� ��� -> ������

	nextBlock = rand() % 7;		// ���� ����� �����ϰ� ������

	curBlockRotation = 0;
	NewBlockOn = 0;		// ���ο� ����� ������ ����

	for (i = 0; i < 4; i++) {		// ���� ��� ����
		for (j = 0; j < 4; j++) {
			if (Blocks[curBlock][curBlockRotation][i][j] == 1) gameboard[curBlock_Y + i][curBlock_X + j] = ACTIVE_BLOCK;
		}
	}
	for (i = 1; i < 3; i++) {		// ���� ��� ����
		for (j = 0; j < 4; j++) {
			if (Blocks[nextBlock][0][i][j] == 1) {
				switch (nextBlock + 1) {	// ��� Ÿ�Կ� ���� �� ����
				case 1:
					SetColor(BLUE, BLUE);
					gotoxy(STATUS_X + 4 + (2 * j), i + 10);
					printf("��");
					SetColor(WHITE, BLACK);
					break;
				case 2:
					SetColor(CYAN, CYAN);
					gotoxy(STATUS_X + 4 + (2 * j), i + 10);
					printf("��");
					SetColor(WHITE, BLACK);
					break;
				case 3:
					SetColor(BROWN, BROWN);
					gotoxy(STATUS_X + 4 + (2 * j), i + 10);
					printf("��");
					SetColor(WHITE, BLACK);
					break;
				case 4:
					SetColor(YELLOW, YELLOW);
					gotoxy(STATUS_X + 4 + (2 * j), i + 10);
					printf("��");
					SetColor(WHITE, BLACK);
					break;
				case 5:
					SetColor(GREEN, GREEN);
					gotoxy(STATUS_X + 4 + (2 * j), i + 10);
					printf("��");
					SetColor(WHITE, BLACK);
					break;
				case 6:
					SetColor(RED, RED);
					gotoxy(STATUS_X + 4 + (2 * j), i + 10);
					printf("��");
					SetColor(WHITE, BLACK);
					break;
				case 7:
					SetColor(LIGHTPURPLE, LIGHTPURPLE);
					gotoxy(STATUS_X + 4 + (2 * j), i + 10);
					printf("��");
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

int CrushBlock(int curBlock_X, int curBlock_Y, int curBlockRotation) { // ������ ��ǥ�� ȸ�������� ���� �浹�� �ִ��� �˻�.
	int i, j;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			if (Blocks[curBlock][curBlockRotation][i][j] == 1 && gameboard[curBlock_Y + i][curBlock_X + j] > 0) return FALSE; // ������ ��ġ�� �����ǰ� ���� �� �� ��ġ�� FALSE ����
		}
	}
	return TRUE; // �Ȱ�ġ�� TRUE ����
}

void CheckKeyhit() {
	keyhit = 0; // Ű �� �ʱ�ȭ

	if (_kbhit()) { // Ű �Է��� ���� ���
		keyhit = _getch(); // Ű ���� ����
		if (keyhit == 224) { // Ű�� ����Ű �ΰ��
			do {
				keyhit = _getch();
			} while (keyhit == 224); // ���� ���ð��� ����
			switch (keyhit) {
			case LEFT: // ���� ���� Ű
				if (CrushBlock(curBlock_X - 1, curBlock_Y, curBlockRotation) == TRUE) moveBlock(LEFT); // �������� �� �� �ִ��� Ȯ�� �� �����ϸ� �̵�
				break;
			case RIGHT: // ������ ���� Ű
				if (CrushBlock(curBlock_X + 1, curBlock_Y, curBlockRotation) == TRUE) moveBlock(RIGHT); // ���������� �� �� �ִ��� Ȯ�� �� �����ϸ� �̵�
				break;
			case UP: // ���� ���� Ű
				if (CrushBlock(curBlock_X, curBlock_Y, (curBlockRotation + 1) % 4) == TRUE)moveBlock(UP); // ȸ�� �� �� �ִ��� Ȯ�� �� �����ϸ� �̵�

				else if (CrushOn == 1 && CrushBlock(curBlock_X, curBlock_Y - 1, (curBlockRotation + 1) % 4) == TRUE) moveBlock(SPECIAL_MOTION); // Ư�� ����
				break;
			case DOWN: // �Ʒ��� ���� Ű
				if (CrushBlock(curBlock_X, curBlock_Y + 1, curBlockRotation) == TRUE) moveBlock(DOWN); // Soft Drop �� ������ �� Ȯ�� �� �����ϸ� �̵�
				break;
			}
		}
		else { // ���� Ű�� �ƴ� ���
			switch (keyhit) {
			case SPACE: // �����̽���
				spaceOn = 1; // �����̽��� ��ȣ�� Ŵ
				while (CrushOn == 0) { // �ٴڿ� ���� �� ���� ����
					dropBlock();
					score += level; // Hard Drop ���ʽ�
					gotoxy(STATUS_X, STATUS_SCORE_Y); printf("        %6d", score); // ���� ����
				}
				break;
			case ESC: // ESC
				pauseSelectMenu = pause(); // ���� �Ͻ� ����
				break;
			}
		}
	}
	while (_kbhit()) _getch(); // Ű ���۸� ����
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

		switch (level) { //�������� �ӵ��� ��������. 
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

		LevelUpOn = 0; //������ ��ȣ ��

		gotoxy(STATUS_X, STATUS_GOAL_Y); printf(" GOAL  : %5d", goal - linecnt);
		gotoxy(STATUS_X, STATUS_SCORE_Y); printf("        %6d", score);
	}
}

void moveBlock(int key) {
	int i, j;

	switch (key) {
	case LEFT: // ���� ����
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (Blocks[curBlock][curBlockRotation][i][j] == 1) gameboard[curBlock_Y + i][curBlock_X + j] = EMPTY;  // ���� ��ǥ�� ���� ����
			}
		}
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (Blocks[curBlock][curBlockRotation][i][j] == 1) gameboard[curBlock_Y + i][curBlock_X + j - 1] = ACTIVE_BLOCK; // �� ĭ ���� ���� ��� ����
			}
		}
		curBlock_X--;
		break;
	case RIGHT: // ������ ����
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (Blocks[curBlock][curBlockRotation][i][j] == 1) gameboard[curBlock_Y + i][curBlock_X + j] = EMPTY; // ���� ��ǥ�� ���� ����
			}
		}
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (Blocks[curBlock][curBlockRotation][i][j] == 1) gameboard[curBlock_Y + i][curBlock_X + j + 1] = ACTIVE_BLOCK; // �� ĭ ������ ���� ��� ����
			}
		}
		curBlock_X++;
		break;
	case UP: // ȸ��
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (Blocks[curBlock][curBlockRotation][i][j] == 1) gameboard[curBlock_Y + i][curBlock_X + j] = EMPTY; // ���� ��ǥ�� ���� ����
			}
		}
		curBlockRotation = (curBlockRotation + 1) % 4; // ȸ�� �� 1�� ������Ű��, 0~3�� ������ ���� �޴´�

		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (Blocks[curBlock][curBlockRotation][i][j] == 1) gameboard[curBlock_Y + i][curBlock_X + j] = ACTIVE_BLOCK; // ȸ���� �� ����
			}
		}
		break;

	case DOWN: // Soft Drop
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (Blocks[curBlock][curBlockRotation][i][j] == 1) gameboard[curBlock_Y + i][curBlock_X + j] = EMPTY; // ���� ��ǥ�� ���� ����
			}
		}
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (Blocks[curBlock][curBlockRotation][i][j] == 1) gameboard[curBlock_Y + i + 1][curBlock_X + j] = ACTIVE_BLOCK; // �� ĭ �ؿ� ��� ����
			}
		}
		curBlock_Y++;
		break;

	case SPECIAL_MOTION: // Ư�� ���� ( �ٸ� ��� Ȥ�� �ٴڰ� �´��� ���¿��� �� ĭ ���� �÷��� �� ȸ���� ������ ��� ���۽�Ű�� Ư�� ���� )
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (Blocks[curBlock][curBlockRotation][i][j] == 1) gameboard[curBlock_Y + i][curBlock_X + j] = EMPTY; // ���� ��ǥ�� ���� ����
			}
		}
		curBlockRotation = (curBlockRotation + 1) % 4; // ���� ȸ�� ���� �޴´�.
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (Blocks[curBlock][curBlockRotation][i][j] == 1) gameboard[curBlock_Y + i - 1][curBlock_X + j] = ACTIVE_BLOCK; // �� ĭ ���� �ö� ���� ����
			}
		}
		curBlock_Y--;
		break;
	}
}

void dropBlock() {	// ����� ����Ʈ��.
	if (CrushOn && CrushBlock(curBlock_X, curBlock_Y + 1, curBlockRotation) == TRUE) CrushOn = 0;	// ���� ��������� CrushOn ��ȣ�� ����.
	if (CrushOn && CrushBlock(curBlock_X, curBlock_Y + 1, curBlockRotation) == FALSE) {				// ���� ������� �ʰ�, CrushOn�� ���� ������
		for (int i = 0; i < GAMEBOARD_Y; i++) {
			for (int j = 0; j < GAMEBOARD_X; j++) {
				if (gameboard[i][j] == ACTIVE_BLOCK) gameboard[i][j] = INACTIVE_BLOCK;				// ���� ����� ������,
				curBlockColor = curBlock;															// ���� ����� ������ �����ϰ�,
			}
		}
		CrushOn = 0;			// CrushOn ��
		checkLine();
		NewBlockOn = 1;																				// ���ο� ��ϻ��� ��ȣ�� Ŵ
		return;
	}

	if (CrushBlock(curBlock_X, curBlock_Y + 1, curBlockRotation) == TRUE) moveBlock(DOWN);			// ���� ��������� �� ĭ �̵�
	if (CrushBlock(curBlock_X, curBlock_Y + 1, curBlockRotation) == FALSE) CrushOn++;				// ���� ������� ������ CrushOn ��
}

int gameOver() {
	int i;
	char* Text[4] = { "[1] �� ����", "[2] ��ŷ", "[3] ���θ޴�", "[4] ��������" };

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
			gotoxy(x1, y + 0); printf("  �����"); //���ӿ��� �޼��� 
			gotoxy(x1, y + 1); printf("��");
			gotoxy(x1, y + 2); printf("��    ���");
			gotoxy(x1, y + 3); printf("��      ��");
			gotoxy(x1, y + 4); printf("  �����");
			gotoxy(x1 + 12, y + 0); printf(" �����");
			gotoxy(x1 + 12, y + 1); printf("��      ��");
			gotoxy(x1 + 12, y + 2); printf("������");
			gotoxy(x1 + 12, y + 3); printf("��      ��");
			gotoxy(x1 + 12, y + 4); printf("��      ��");
			gotoxy(x1 + 24, y + 0); printf("���  ���");
			gotoxy(x1 + 24, y + 1); printf("��  ��  ��");
			gotoxy(x1 + 24, y + 2); printf("��  ��  ��");
			gotoxy(x1 + 24, y + 3); printf("��      ��");
			gotoxy(x1 + 24, y + 4); printf("��      ��");
			gotoxy(x1 + 36, y + 0); printf("�����");
			gotoxy(x1 + 36, y + 1); printf("��");
			gotoxy(x1 + 36, y + 2); printf("����");
			gotoxy(x1 + 36, y + 3); printf("��");
			gotoxy(x1 + 36, y + 4); printf("������");

			gotoxy(x2, y + 6); printf("   ���");
			gotoxy(x2, y + 7); printf(" ��    ��");
			gotoxy(x2, y + 8); printf("��      ��");
			gotoxy(x2, y + 9); printf(" ��    ��");
			gotoxy(x2, y + 10); printf("   ���");
			gotoxy(x2 + 11, y + 6); printf("��      ��");
			gotoxy(x2 + 11, y + 7); printf(" ��    ��");
			gotoxy(x2 + 11, y + 8); printf("  ��  ��");
			gotoxy(x2 + 11, y + 9); printf("   ���");
			gotoxy(x2 + 11, y + 10); printf("    ��");
			gotoxy(x2 + 23, y + 6); printf("�����");
			gotoxy(x2 + 23, y + 7); printf("��");
			gotoxy(x2 + 23, y + 8); printf("����");
			gotoxy(x2 + 23, y + 9); printf("��");
			gotoxy(x2 + 23, y + 10); printf("�����");
			gotoxy(x2 + 33, y + 6); printf("����");
			gotoxy(x2 + 33, y + 7); printf("��   ��");
			gotoxy(x2 + 33, y + 8); printf("����");
			gotoxy(x2 + 33, y + 9); printf("��   ��");
			gotoxy(x2 + 33, y + 10); printf("��    ��");

			gotoxy(x2 + 4, y + 13); printf("������������������������������������������������������������������");
			gotoxy(x2 + 4, y + 14); printf("��       YOUR SCORE : %6d     ��", score);
			gotoxy(x2 + 4, y + 15); printf("������������������������������������������������������������������");
			SetColor(BLACK, WHITE);
			gotoxy(X, Y); printf("[1] �� ����");
			SetColor(WHITE, BLACK);
			gotoxy(X, Y + 2); printf("[2] ��ŷ");
			gotoxy(X, Y + 4); printf("[3] ���θ޴�");
			gotoxy(X, Y + 6); printf("[4] ��������");

			last_score = score;

			while (1) {							// ���ѷ����� ���� ����Ű �Է� ������ Ȯ��
				int key = _getch();

				switch (key) {
				case UP:
					getCurPath(bgm[7]);
					PlaySound(path2cpy, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
					if (Y > 20) {				// UP�� ���� ���
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
				case ENTER:						// ENTER -> ����
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
	char* pauseText[3] = { "[1] ����ϱ�","[2] �� ����","[3] ���θ޴�" };
	int textY = 13;
	int Y = 13;

	gotoxy(x, y + 0); printf("��������������������������������������������������");
	gotoxy(x, y + 1); printf("��        �Ͻ�����       ��");
	gotoxy(x, y + 2); printf("��������������������������������������������������");
	gotoxy(x, y + 3); printf("��                       ��");
	gotoxy(x, y + 4); printf("��                       ��");
	gotoxy(x, y + 5); printf("��                       ��");
	gotoxy(x, y + 6); printf("��                       ��");
	gotoxy(x, y + 7); printf("��                       ��");
	gotoxy(x, y + 8); printf("��                       ��");
	gotoxy(x, y + 9); printf("��                       ��");
	gotoxy(x, y + 10); printf("��������������������������������������������������");

	SetColor(BLACK, WHITE);
	gotoxy(x + 6, Y); printf("[1] ����ϱ�");
	SetColor(WHITE, BLACK);
	gotoxy(x + 6, Y + 2); printf("[2] �� ����");
	gotoxy(x + 6, Y + 4); printf("[3] ���θ޴�");

	while (1) {							// ���ѷ����� ���� ����Ű �Է� ������ Ȯ��
		int key = _getch();

		switch (key) {
		case UP:
			if (Y > 13) {				// UP�� ���� ���
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
		case ENTER:						// ENTER -> ����
			if (Y - 11 == 2) {
				for (int i = 3; i < GAMEBOARD_Y - 1; i++) {			// ������ �� �ٽ� �׸�
					gotoxy(2 * (GAMEBOARD_X + GAMEBOARD_X_ADJ - 1), i);
					SetColor(WHITE, WHITE);
					printf("��");
					SetColor(WHITE, BLACK);
				}
				for (int i = 9; i < GAMEBOARD_Y - 5; i++) {			// PAUSE â�� ����
					gotoxy(2 * (GAMEBOARD_X + GAMEBOARD_X_ADJ), i);
					printf("                       ");

				}
				GameInfo();

				for (int i = 1; i < 3; i++) {		// ���� ��� ����
					for (int j = 0; j < 4; j++) {
						if (Blocks[nextBlock][0][i][j] == 1) {
							switch (nextBlock + 1) {	// ��� Ÿ�Կ� ���� �� ����
							case 1:
								SetColor(BLUE, BLUE);
								gotoxy(STATUS_X + 4 + (2 * j), i + 10);
								printf("��");
								SetColor(WHITE, BLACK);
								break;
							case 2:
								SetColor(CYAN, CYAN);
								gotoxy(STATUS_X + 4 + (2 * j), i + 10);
								printf("��");
								SetColor(WHITE, BLACK);
								break;
							case 3:
								SetColor(BROWN, BROWN);
								gotoxy(STATUS_X + 4 + (2 * j), i + 10);
								printf("��");
								SetColor(WHITE, BLACK);
								break;
							case 4:
								SetColor(YELLOW, YELLOW);
								gotoxy(STATUS_X + 4 + (2 * j), i + 10);
								printf("��");
								SetColor(WHITE, BLACK);
								break;
							case 5:
								SetColor(GREEN, GREEN);
								gotoxy(STATUS_X + 4 + (2 * j), i + 10);
								printf("��");
								SetColor(WHITE, BLACK);
								break;
							case 6:
								SetColor(RED, RED);
								gotoxy(STATUS_X + 4 + (2 * j), i + 10);
								printf("��");
								SetColor(WHITE, BLACK);
								break;
							case 7:
								SetColor(LIGHTPURPLE, LIGHTPURPLE);
								gotoxy(STATUS_X + 4 + (2 * j), i + 10);
								printf("��");
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
			for (int i = 3; i < GAMEBOARD_Y - 1; i++) {			// ������ �� �ٽ� �׸�
				gotoxy(2 * (GAMEBOARD_X + GAMEBOARD_X_ADJ - 1), i);
				SetColor(WHITE, WHITE);
				printf("��");
				SetColor(WHITE, BLACK);
			}
			for (int i = 9; i < GAMEBOARD_Y - 5; i++) {			// PAUSE â�� ����
				gotoxy(2 * (GAMEBOARD_X + GAMEBOARD_X_ADJ), i);
				printf("                       ");
				GameInfo();

				for (int i = 1; i < 3; i++) {		// ���� ��� ����
					for (int j = 0; j < 4; j++) {
						if (Blocks[nextBlock][0][i][j] == 1) {
							switch (nextBlock + 1) {	// ��� Ÿ�Կ� ���� �� ����
							case 1:
								SetColor(BLUE, BLUE);
								gotoxy(STATUS_X + 4 + (2 * j), i + 10);
								printf("��");
								SetColor(WHITE, BLACK);
								break;
							case 2:
								SetColor(CYAN, CYAN);
								gotoxy(STATUS_X + 4 + (2 * j), i + 10);
								printf("��");
								SetColor(WHITE, BLACK);
								break;
							case 3:
								SetColor(BROWN, BROWN);
								gotoxy(STATUS_X + 4 + (2 * j), i + 10);
								printf("��");
								SetColor(WHITE, BLACK);
								break;
							case 4:
								SetColor(YELLOW, YELLOW);
								gotoxy(STATUS_X + 4 + (2 * j), i + 10);
								printf("��");
								SetColor(WHITE, BLACK);
								break;
							case 5:
								SetColor(GREEN, GREEN);
								gotoxy(STATUS_X + 4 + (2 * j), i + 10);
								printf("��");
								SetColor(WHITE, BLACK);
								break;
							case 6:
								SetColor(RED, RED);
								gotoxy(STATUS_X + 4 + (2 * j), i + 10);
								printf("��");
								SetColor(WHITE, BLACK);
								break;
							case 7:
								SetColor(LIGHTPURPLE, LIGHTPURPLE);
								gotoxy(STATUS_X + 4 + (2 * j), i + 10);
								printf("��");
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

	char* Number[10] = { "��","��","��","��","��","��","��","��","��","10" };
	const x = 5;
	int i;

	system("cls");
	gotoxy(3, 1); printf("����������������������������������");
	gotoxy(3, 2); printf("�� R A N K I N G ��");
	gotoxy(3, 3); printf("����������������������������������");

	gotoxy(3, 5); printf("������������������������������������������������������������������������������������������������������������������������������������������������");
	gotoxy(3, 6); printf("No       ID              SCORE              LEVEL           LINES");
	gotoxy(3, 7); printf("������������������������������������������������������������������������");

	for (int i = 1; i < 11; i++) {
		gotoxy(3, 6 + 2 * i); printf("%s", Number[i - 1]);

	}

	gotoxy(3, 27); printf("������������������������������������������������������������������������������������������������������������������������������������������������");
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

	gotoxy(x, y - 1); printf("��������������������������������������������������");
	gotoxy(x, y + 0); printf("��                       ��");
	gotoxy(x, y + 1); printf("��       ���� �����     ��");
	gotoxy(x, y + 2); printf("��    �����Ͻðڽ��ϱ�?  ��");
	gotoxy(x, y + 3); printf("��                       ��");
	gotoxy(x, y + 4); printf("��                       ��");
	gotoxy(x, y + 5); printf("��                       ��");
	gotoxy(x, y + 6); printf("��������������������������������������������������");

	SetColor(BLACK, WHITE);
	gotoxy(X, Y); printf("[1] YES");
	SetColor(WHITE, BLACK);
	gotoxy(X + 9, Y); printf("[2] NO");

	while (1) {							// ���ѷ����� ���� ����Ű �Է� ������ Ȯ��
		int key = _getch();

		switch (key) {
		case LEFT:
			getCurPath(bgm[7]);
			PlaySound(path2cpy, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
			if (X > 32) {				// LEFT�� ���� ���
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
		case ENTER:						// ENTER -> ����
			getCurPath(bgm[6]);
			PlaySound(path2cpy, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
			if (X - textX == 0) {
				gotoxy(x, y - 1); printf("��������������������������������������������������");
				gotoxy(x, y + 0); printf("��                       ��");
				gotoxy(x, y + 1); printf("��   �̸��� �Է��ϼ���   ��");
				gotoxy(x, y + 2); printf("��    ( �ִ� 10 �ڸ� )   ��");
				gotoxy(x, y + 3); printf("��                       ��");
				gotoxy(x, y + 4); printf("��  ID :                 ��");
				gotoxy(x, y + 5); printf("��                       ��");
				gotoxy(x, y + 6); printf("��������������������������������������������������");

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
		printf("popen() ����");
		return;
	}

	fgets(path2cpy, 100, fp);

	strncpy_s(path2cpy, sizeof(path2cpy), path2cpy, strlen(path2cpy) - 1);

	if (bgm != NULL) {
		strcat(path2cpy, bgm);
	}
}