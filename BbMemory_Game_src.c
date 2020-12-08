#pragma warning(disable: 4996) // scanf, gets, fopen등의 함수 보안 취약점 경고 무시
#include <stdio.h> // 기본 헤더 파일
#include <string.h> // 문자열 함수 호출 용
#include <time.h> // time함수와 clock함수 호출 용
#pragma comment(lib, "winmm.lib") 
#define HAVE_STRUCT_TIMESPEC // windows.h와 pthread.h간의 시간 구조체 재정의 오류 방지용
#include <pthread.h> // 스레드 동작 구성 용

#define SPEED 2 // 단어가 생성되는 간격 (단위 : sec). 

/*추가 */
#define STRING_SIZE 80 // 적당한 크기를 잡는다
char buffer[STRING_SIZE];
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <termio.h>
#include <termios.h>
#include <unistd.h>

int getch(void) { //c의 getch() 함수 대용
	int ch;
	struct termios buf, save;
	tcgetattr(0, &save);
	buf = save;
	buf.c_lflag &= ~(ICANON | ECHO);
	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;
	tcsetattr(0, TCSAFLUSH, &buf);
	ch = getchar();
	tcsetattr(0, TCSAFLUSH, &save);
	return ch;
}

char* words[35] = { //단어 표본

	"한국",
	"word",
	"wolrd",
	"school",
	"korea",
	"study",
	"computer",
	"스마트폰",
	"book",
	"homework",
	"디저트",
	"bread",
	"class",
	"java",
	"프로그램",
	"coding",
	"system",
	"problem",
	"question",
	"pain",
	"어지러움",
	"cookie",
	"리눅스",
	"write",
	"read",
	"memory",
	"가상머신",
	"studio",
	"include",
	"server",
	"클라이언트",
	"quit",
	"dinner",
	"clean",
	"lab"
};

typedef struct { // 산성비(단어) 구조체
	int x; // 단어의 x좌표
	char word[20]; // 단어 저장 공간
} rain;

rain rains[21]; // 20번째 rains는 판정선.(0~19번째까지만 화면 표시)
clock_t start, end; // 게임 시작, 진행(종료) 시간
double sec = 0.0; // 플레이 누적 시간
double heart = 7.0; // 체력
//char buffer[50]; // 사용자 입력 버퍼

void help(); // 도움말 함수
void pointRecode(); // 점수 보기 함수
void initrains(); // 단어 배열 초기화 함수
void gameStart(); // 게임 주 함수
void prnscreen(); // 화면 출력 함수
void makerain(); // 새 단어 생성 함수

static pthread_t p_thread; // 스레드 이름
static int thr_id; // 스레드의 아이디
static int thr_exit = 1; // 스레드 종료 여부 상태

void* t_function(void* data); // 스레드화 할 사용자 입력 함수
void start_thread(); // 스레드 시작 함수
void end_thread(); // 스레드 중지 함수



int main(void)
{
	int choice = 0;


	printf("\n\n\n"); // 첫 화면
	printf("         ######     ###         ##       ##      #######        ######  ########   ###     #######    ########  ##                                      \n");
	printf("        ##    ##   ## ##        ###     ###      ##             ##         ##     ## ##    ##    ##      ##     ##                                      \n");
	printf("        ##        ##   ##       ## ## ## ##      ##             ##         ##    ##   ##   ##    ##      ##     ##                                      \n");
	printf("        ##       ##     ##      ##  ###  ##      #######        ######     ##   ##     ##  #######       ##     ##                                      \n");
	printf("        ##   ### #########      ##       ##      ##                 ##     ##   #########  ##   ##       ##     ##                                      \n");
	printf("        ##    ## ##     ##      ##       ##      ##                 ##     ##   ##     ##  ##    ##      ##                                             \n");
	printf("         ######  ##     ##      ##       ##      #######        ######     ##   ##     ##  ##     ##     ##     ##                                  \n");
	printf("\n\n                                                    [ 시작하려면 아무키나 누르세요 ]\n");
	getch();

	

	while (1)
	{
		system("clear"); // 콘솔창 초기화
		printf("\n\n\n                            [ 메인메뉴 ]\n\n\n\n\n"); // 메인메뉴
		printf("                            1. 게임시작\n\n");
		printf("                            2. 기록보기\n\n");
		printf("                            3. 도 움 말\n\n");
		printf("                            4. 게임종료\n\n");
		printf("                       선택>");
		scanf("%d", &choice);

		switch (choice)
		{
		case 1:
			gameStart(); // 게임의 주 함수 호출 (게임 시작)
			break;
		case 2:
			pointRecode(); // 점수 기록 출력 함수 호출
			break;
		case 3:
			help(); // 도움말 출력 함수 호출
			break;
		case 4:
			system("clear");
			return 0; // 게임 종료
			break;
		default:
			break;
		}
	}
}

void help() // 도움말 출력 함수
{
	system("clear"); // 콘솔창 초기화
	printf("타자연습과 함께 재미를 느낄 수 있었던 그때 그 시절의 그 게임, '산성비'를 구현한 게임입니다.\n");
	printf("하늘에서 내리는 단어들을 빨리 입력하여 없애주세요!\n");
	printf("하단의 체력이 0이 되면 게임이 종료됩니다.\n");
	printf("점수가 저장됩니다. 메인메뉴의 '2. 기록보기'에서 확인 가능합니다.\n\n");
	printf("아무키나 누르면 메인 메뉴로 이동합니다.");
	getch();
}

void pointRecode() // 점수 기록 출력하는 함수
{
	system("clear"); // 콘솔창 초기화
	
	FILE* fp2 = NULL; // 점수 저장 파일을 불러올 파일 포인터
	double s;
	int cnt; // n번째 번호
	printf("[ 점수 ]\n\n\n");
	fp2 = fopen("score.txt", "r"); // 점수 파일 불러오기
	if (fp2 == NULL)
		printf("한번도 게임을 하신적이 없거나,\n점수 파일을 불러오는데 문제가 있습니다.\n");
	else
	{
		cnt = 1; // 파일의 첫번째부터
		while (EOF != fscanf(fp2, "%lf", &s)) // 마지막까지 읽으면서
		{
			printf("%d. %.2lf초\n", cnt, s); // 점수 출력
			cnt++;
		}
	}
	
	printf("\n\n아무키나 누르면 메인메뉴로 이동합니다.");
	getch();
}

void gameStart() // 실행할 게임 함수
{
	FILE* fp = NULL; // 점수 저장 파일 포인터
	heart = 5.0; // 체력 5.0으로 초기화
	system("clear"); // 콘솔창 초기화
	initrains(); // 단어 배열 초기화
	start_thread(); // 사용자로부터 입력을 받는 스레드 시작
	start = clock(); // 게임 시작 시간 기록
	while (1)
	{
		sleep(SPEED); // 지정한 시간만큼 단어 생성 지연
		makerain(); // 새로운 단어 생성
		end = clock(); // 해당 단어가 생성된 시간 기록
		sec = (double)(end - start) / CLOCKS_PER_SEC; // 현재까지 진행한 시간 
		prnscreen(); // 화면 출력
		if (heart <= 0) // 체력이 0 이하이면
			break; // 게임 오버
	}

	end_thread(); // 입력 스레드 중지
	printf("\nGame Over!\n");
	fp = fopen("score.txt", "a"); // 점수 저장 파일 열기
	if (fp == NULL) 
		printf("점수 기록 파일 작성 실패!\n\n");
	else
	{
		fprintf(fp, "%.2lf\n", sec); // 점수 기록
		fclose(fp);
	}
	printf("아무키나 누르면 메인메뉴로 이동합니다.\n");
	printf("메인메뉴가 나타나지 않으면 한번 더 입력해주세요.");
	getch();
}

void prnscreen() // 화면 출력 함수
{
	int i;
	system("clear"); // 콘솔창 초기화
	for (i = 0; i < 20; i++)
	{
		printf("%*s%s\n", rains[i].x, "", rains[i].word); // x좌표에 맞추어 가변적으로 단어 출력
	}
	printf("~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~"); // 판정선(rains[20].word)
	if (strcmp(rains[20].word, " ")) // 판정선에 단어가 남아있으면
		heart -= 1.0; // 1.0씩 체력 깎음
	printf("\n[ 체력 ] %.1lf  [ 시간 ] %.2lf초\n\n", heart, sec); // 체력와 총 시간 출력
	printf("입력>"); // 사용자의 입력부
}

void makerain() // 새로운 단어(행)을 생성하는 함수
{
	int i;
	for (i = 20; i >= 0; i--)
	{
		strcpy(rains[i].word, rains[i - 1].word); // 기존 단어는 한 줄씩 밀고
		rains[i].x = rains[i - 1].x;
		rains[i - 1].x = 0;
		strcpy(rains[i - 1].word, " "); // 뒷 줄은 공백처리
	}
	rains[0].x = rand() % 53;
	srand(time(NULL));
	strcpy(rains[0].word, words[rand() % 35]); // 새로운 단어를 무작위로 배치
}

void initrains() // 단어 배열 초기화 함수 (모두 공백으로)
{
	int i;
	for (i = 0; i < 21; i++)
	{
		rains[i].x = 0;
		strcpy(rains[i].word, " ");
	}
}

void* t_function(void* data) // 스레드 처리할 단어 입력 함수
{
	while (!thr_exit) // 스레드가 중지될 때까지 입력을 계속 받음
	{
		int i;
		fgets(buffer, STRING_SIZE, stdin);
		for (i = 0; i < 20; i++)
		{
			if (strstr(rains[i].word, buffer)) // 입력 값과 단어가 일치하면 해당 단어 제거
				strcpy(rains[i].word, " ");
		}
	}
}

void start_thread() // 스레드 시작 함수
{
	thr_exit = 0;
	thr_id = pthread_create(&p_thread, NULL, t_function, NULL); // 스레드 생성
}

void end_thread() // 스레드 중지 함수
{
	thr_exit = 1;
	pthread_cancel(p_thread); // 스레드 취소
}
