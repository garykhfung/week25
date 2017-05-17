#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#define WAIT 100000
#define YBoundary 29
#define XBoundary 80
#define Esc 27
#define monster "*"
#define NumberOfMonster 10 //must not be greater than XBoundary

void printPlayer();
void movePlayer(int input);
void createMonster();
void printMonster(int i);
void moveMonster();


int yMonster[NumberOfMonster];
int xMonster [NumberOfMonster];
int xMonsterSpeed [NumberOfMonster];

int ch;
int yPlayer = YBoundary-1;
int xPlayer = XBoundary/2;
int playing = TRUE;


int kbhit(void)
{
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
 
	if(ch != EOF)
	{
		ungetc(ch, stdin);
		return 1;
	}
	
	ch = 0;
	fflush(stdin);
	return 0;
}

int main()
{	
	initscr();			/* Start curses mode 		*/
	//raw();				/* Line buffering disabled	*/
	keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
	noecho();			/* Don't echo() while we do getch */
	curs_set(0);
	
	getch();
	createMonster();
	printPlayer();
	refresh();
	
	while ( playing == TRUE)
	{
		halfdelay(1);
		ch = getch();
		if(ch != ERR)
		{
			movePlayer(ch);
			usleep(WAIT);
		}
		moveMonster();
		refresh();	
	}
	endwin();			/* End curses mode*/

	return 0;
}

//player
void printPlayer()
{
	mvprintw(yPlayer,xPlayer,"+");
	return;
}

void movePlayer(int input)
{
	while(0 < yPlayer && yPlayer < YBoundary
			&& 0 < xPlayer && xPlayer < XBoundary)
	{	
		switch(input){
			case KEY_RIGHT:
				mvprintw(yPlayer,xPlayer," ");
				xPlayer++;
				break;
			case KEY_LEFT:
				mvprintw(yPlayer,xPlayer," ");
				xPlayer--;
				break;
			case KEY_UP:
				mvprintw(yPlayer,xPlayer," ");
				yPlayer--;
				break;
			case KEY_DOWN:
				mvprintw(yPlayer,xPlayer," ");
				yPlayer++;
				break;	
		}
		break;	
	}
	printPlayer();

}

//monster
void createMonster ()
{
	int i;
	
	for(i = 0; i < NumberOfMonster; i++)
	{
		yMonster[i] = 1;
		xMonster[i] = (NumberOfMonster - i);
		xMonsterSpeed[i] = 1;
		
		printMonster(i);
	}
	
}

void printMonster (int i)
{
	mvprintw(yMonster[i], xMonster[i], monster);
	return;
}

void moveMonster()
{
	int i;
	
	for(i = 0; i < NumberOfMonster; i++)
	{
		while(0 < yMonster[i] && yMonster[i] < YBoundary)
		{
			if(0 < xMonster[i] && xMonster[i] < XBoundary)
			{
				mvprintw(yMonster[i],xMonster[i]," ");
				xMonster[i] = xMonster[i] + xMonsterSpeed[i];
			}else{
				xMonsterSpeed[i] = -xMonsterSpeed[i];
				mvprintw(yMonster[i],xMonster[i]," ");
				xMonster[i] = xMonster[i] + xMonsterSpeed[i];
				yMonster[i]++;
			}
			printMonster(i);
			refresh();
			
			break;
		}
	}
}

