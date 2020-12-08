#pragma warning(disable: 4996) // scanf, gets, fopen���� �Լ� ���� ����� ��� ����
#include <stdio.h> // �⺻ ��� ����
#include <string.h> // ���ڿ� �Լ� ȣ�� ��
#include <time.h> // time�Լ��� clock�Լ� ȣ�� ��
#pragma comment(lib, "winmm.lib") 
#define HAVE_STRUCT_TIMESPEC // windows.h�� pthread.h���� �ð� ����ü ������ ���� ������
#include <pthread.h> // ������ ���� ���� ��

#define SPEED 2 // �ܾ �����Ǵ� ���� (���� : sec). 

/*�߰� */
#define STRING_SIZE 80 // ������ ũ�⸦ ��´�
char buffer[STRING_SIZE];
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <termio.h>
#include <termios.h>
#include <unistd.h>

int getch(void) { //c�� getch() �Լ� ���
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

char* words[35] = { //�ܾ� ǥ��

	"�ѱ�",
	"word",
	"wolrd",
	"school",
	"korea",
	"study",
	"computer",
	"����Ʈ��",
	"book",
	"homework",
	"����Ʈ",
	"bread",
	"class",
	"java",
	"���α׷�",
	"coding",
	"system",
	"problem",
	"question",
	"pain",
	"��������",
	"cookie",
	"������",
	"write",
	"read",
	"memory",
	"����ӽ�",
	"studio",
	"include",
	"server",
	"Ŭ���̾�Ʈ",
	"quit",
	"dinner",
	"clean",
	"lab"
};

typedef struct { // �꼺��(�ܾ�) ����ü
	int x; // �ܾ��� x��ǥ
	char word[20]; // �ܾ� ���� ����
} rain;

rain rains[21]; // 20��° rains�� ������.(0~19��°������ ȭ�� ǥ��)
clock_t start, end; // ���� ����, ����(����) �ð�
double sec = 0.0; // �÷��� ���� �ð�
double heart = 7.0; // ü��
//char buffer[50]; // ����� �Է� ����

void help(); // ���� �Լ�
void pointRecode(); // ���� ���� �Լ�
void initrains(); // �ܾ� �迭 �ʱ�ȭ �Լ�
void gameStart(); // ���� �� �Լ�
void prnscreen(); // ȭ�� ��� �Լ�
void makerain(); // �� �ܾ� ���� �Լ�

static pthread_t p_thread; // ������ �̸�
static int thr_id; // �������� ���̵�
static int thr_exit = 1; // ������ ���� ���� ����

void* t_function(void* data); // ������ȭ �� ����� �Է� �Լ�
void start_thread(); // ������ ���� �Լ�
void end_thread(); // ������ ���� �Լ�



int main(void)
{
	int choice = 0;


	printf("\n\n\n"); // ù ȭ��
	printf("         ######     ###         ##       ##      #######        ######  ########   ###     #######    ########  ##                                      \n");
	printf("        ##    ##   ## ##        ###     ###      ##             ##         ##     ## ##    ##    ##      ##     ##                                      \n");
	printf("        ##        ##   ##       ## ## ## ##      ##             ##         ##    ##   ##   ##    ##      ##     ##                                      \n");
	printf("        ##       ##     ##      ##  ###  ##      #######        ######     ##   ##     ##  #######       ##     ##                                      \n");
	printf("        ##   ### #########      ##       ##      ##                 ##     ##   #########  ##   ##       ##     ##                                      \n");
	printf("        ##    ## ##     ##      ##       ##      ##                 ##     ##   ##     ##  ##    ##      ##                                             \n");
	printf("         ######  ##     ##      ##       ##      #######        ######     ##   ##     ##  ##     ##     ##     ##                                  \n");
	printf("\n\n                                                    [ �����Ϸ��� �ƹ�Ű�� �������� ]\n");
	getch();

	

	while (1)
	{
		system("clear"); // �ܼ�â �ʱ�ȭ
		printf("\n\n\n                            [ ���θ޴� ]\n\n\n\n\n"); // ���θ޴�
		printf("                            1. ���ӽ���\n\n");
		printf("                            2. ��Ϻ���\n\n");
		printf("                            3. �� �� ��\n\n");
		printf("                            4. ��������\n\n");
		printf("                       ����>");
		scanf("%d", &choice);

		switch (choice)
		{
		case 1:
			gameStart(); // ������ �� �Լ� ȣ�� (���� ����)
			break;
		case 2:
			pointRecode(); // ���� ��� ��� �Լ� ȣ��
			break;
		case 3:
			help(); // ���� ��� �Լ� ȣ��
			break;
		case 4:
			system("clear");
			return 0; // ���� ����
			break;
		default:
			break;
		}
	}
}

void help() // ���� ��� �Լ�
{
	system("clear"); // �ܼ�â �ʱ�ȭ
	printf("Ÿ�ڿ����� �Բ� ��̸� ���� �� �־��� �׶� �� ������ �� ����, '�꼺��'�� ������ �����Դϴ�.\n");
	printf("�ϴÿ��� ������ �ܾ���� ���� �Է��Ͽ� �����ּ���!\n");
	printf("�ϴ��� ü���� 0�� �Ǹ� ������ ����˴ϴ�.\n");
	printf("������ ����˴ϴ�. ���θ޴��� '2. ��Ϻ���'���� Ȯ�� �����մϴ�.\n\n");
	printf("�ƹ�Ű�� ������ ���� �޴��� �̵��մϴ�.");
	getch();
}

void pointRecode() // ���� ��� ����ϴ� �Լ�
{
	system("clear"); // �ܼ�â �ʱ�ȭ
	
	FILE* fp2 = NULL; // ���� ���� ������ �ҷ��� ���� ������
	double s;
	int cnt; // n��° ��ȣ
	printf("[ ���� ]\n\n\n");
	fp2 = fopen("score.txt", "r"); // ���� ���� �ҷ�����
	if (fp2 == NULL)
		printf("�ѹ��� ������ �Ͻ����� ���ų�,\n���� ������ �ҷ����µ� ������ �ֽ��ϴ�.\n");
	else
	{
		cnt = 1; // ������ ù��°����
		while (EOF != fscanf(fp2, "%lf", &s)) // ���������� �����鼭
		{
			printf("%d. %.2lf��\n", cnt, s); // ���� ���
			cnt++;
		}
	}
	
	printf("\n\n�ƹ�Ű�� ������ ���θ޴��� �̵��մϴ�.");
	getch();
}

void gameStart() // ������ ���� �Լ�
{
	FILE* fp = NULL; // ���� ���� ���� ������
	heart = 5.0; // ü�� 5.0���� �ʱ�ȭ
	system("clear"); // �ܼ�â �ʱ�ȭ
	initrains(); // �ܾ� �迭 �ʱ�ȭ
	start_thread(); // ����ڷκ��� �Է��� �޴� ������ ����
	start = clock(); // ���� ���� �ð� ���
	while (1)
	{
		sleep(SPEED); // ������ �ð���ŭ �ܾ� ���� ����
		makerain(); // ���ο� �ܾ� ����
		end = clock(); // �ش� �ܾ ������ �ð� ���
		sec = (double)(end - start) / CLOCKS_PER_SEC; // ������� ������ �ð� 
		prnscreen(); // ȭ�� ���
		if (heart <= 0) // ü���� 0 �����̸�
			break; // ���� ����
	}

	end_thread(); // �Է� ������ ����
	printf("\nGame Over!\n");
	fp = fopen("score.txt", "a"); // ���� ���� ���� ����
	if (fp == NULL) 
		printf("���� ��� ���� �ۼ� ����!\n\n");
	else
	{
		fprintf(fp, "%.2lf\n", sec); // ���� ���
		fclose(fp);
	}
	printf("�ƹ�Ű�� ������ ���θ޴��� �̵��մϴ�.\n");
	printf("���θ޴��� ��Ÿ���� ������ �ѹ� �� �Է����ּ���.");
	getch();
}

void prnscreen() // ȭ�� ��� �Լ�
{
	int i;
	system("clear"); // �ܼ�â �ʱ�ȭ
	for (i = 0; i < 20; i++)
	{
		printf("%*s%s\n", rains[i].x, "", rains[i].word); // x��ǥ�� ���߾� ���������� �ܾ� ���
	}
	printf("~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~"); // ������(rains[20].word)
	if (strcmp(rains[20].word, " ")) // �������� �ܾ ����������
		heart -= 1.0; // 1.0�� ü�� ����
	printf("\n[ ü�� ] %.1lf  [ �ð� ] %.2lf��\n\n", heart, sec); // ü�¿� �� �ð� ���
	printf("�Է�>"); // ������� �Էº�
}

void makerain() // ���ο� �ܾ�(��)�� �����ϴ� �Լ�
{
	int i;
	for (i = 20; i >= 0; i--)
	{
		strcpy(rains[i].word, rains[i - 1].word); // ���� �ܾ�� �� �پ� �а�
		rains[i].x = rains[i - 1].x;
		rains[i - 1].x = 0;
		strcpy(rains[i - 1].word, " "); // �� ���� ����ó��
	}
	rains[0].x = rand() % 53;
	srand(time(NULL));
	strcpy(rains[0].word, words[rand() % 35]); // ���ο� �ܾ �������� ��ġ
}

void initrains() // �ܾ� �迭 �ʱ�ȭ �Լ� (��� ��������)
{
	int i;
	for (i = 0; i < 21; i++)
	{
		rains[i].x = 0;
		strcpy(rains[i].word, " ");
	}
}

void* t_function(void* data) // ������ ó���� �ܾ� �Է� �Լ�
{
	while (!thr_exit) // �����尡 ������ ������ �Է��� ��� ����
	{
		int i;
		fgets(buffer, STRING_SIZE, stdin);
		for (i = 0; i < 20; i++)
		{
			if (strstr(rains[i].word, buffer)) // �Է� ���� �ܾ ��ġ�ϸ� �ش� �ܾ� ����
				strcpy(rains[i].word, " ");
		}
	}
}

void start_thread() // ������ ���� �Լ�
{
	thr_exit = 0;
	thr_id = pthread_create(&p_thread, NULL, t_function, NULL); // ������ ����
}

void end_thread() // ������ ���� �Լ�
{
	thr_exit = 1;
	pthread_cancel(p_thread); // ������ ���
}
