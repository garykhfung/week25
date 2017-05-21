#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>

#define WAIT 100000


#define monster "@"
#define player "+"
#define bullet "^"
#define mushroom ";"

#define NumberOfMonster 10 //must not be greater than XBoundary
#define DefaultNumberOfMushrooms 40
#define NumberOfMushroom DefaultNumberOfMushrooms + NumberOfMonster
#define MushroomLife 4

//game functions
void playerUpdate();
void movePlayer(int input);

void createMonster();
void monsterUpdate();
void printMonster(int i);
void monsterDestroy(int i);

void bulletCreate();
void bulletUpdate();
void bulletDestory();

void createMushroom();
void mushroomUpdate();

void collision();

int gameSetup();
void gameUpdate();

//game variables
int ch;
int XBoundary;
int YBoundary;

int yPlayer;
int xPlayer;
int playing = TRUE;

int yMonster[NumberOfMonster];
int xMonster[NumberOfMonster];
int xMonsterSpeed[NumberOfMonster];
int monsterLife[NumberOfMonster];	

int xBullet, yBullet, bulletLife;

int yMushroom[NumberOfMushroom];
int xMushroom[NumberOfMushroom];
int mushroomLife[NumberOfMushroom];

int i, j; //temp variables


//Main Function
int main()
{	
	if(gameSetup() == 0) playing = FALSE;
	
		getch();
	
	while (playing == TRUE)
		gameUpdate();

	endwin();			/* End curses mode*/

	return 0;
}


//player functioins
void playerUpdate()
{
	mvprintw(yPlayer,xPlayer,player);
	return;
}

void movePlayer(int input)
{

	switch(input){
		case KEY_RIGHT:
			while(xPlayer < XBoundary)
			{
				mvprintw(yPlayer,xPlayer," ");
				xPlayer++;
				break;
			}
			break;
			
		case KEY_LEFT:
			while(0 < xPlayer)
			{
				mvprintw(yPlayer,xPlayer," ");
				xPlayer--;
				break;
			}
			break;
			
		case KEY_UP:
			while(bulletLife == FALSE)
				bulletCreate();
			break;
	}
	playerUpdate();
	return;
}



//monster functions
void createMonster ()
{
	for(i = 0; i < NumberOfMonster; i++)
	{
		yMonster[i] = 1;
		xMonster[i] = (NumberOfMonster - i);
		xMonsterSpeed[i] = 1;
		monsterLife[i] = TRUE;
		
		printMonster(i);
	}
	
}

void monsterUpdate()
{
	mvprintw(yMonster[i],xMonster[i]," ");
	xMonster[i] = xMonster[i] + xMonsterSpeed[i];
			
	printMonster(i);
}

void printMonster (int i)
{
	mvprintw(yMonster[i], xMonster[i], monster);
	return;
}

void monsterDestroy(int i)
{
	monsterLife[i] = FALSE;
	yMushroom[DefaultNumberOfMushrooms + i] = yMonster[i];
	xMushroom[DefaultNumberOfMushrooms + i] = xMonster[i];
	mushroomLife[DefaultNumberOfMushrooms + i] = MushroomLife;
}



//bullet functions
void bulletCreate()
{
	bulletLife = TRUE;
	yBullet = yPlayer;
	xBullet = xPlayer;
	return;
}

void bulletDestroy()
{
	bulletLife = FALSE;
	mvprintw(yBullet, xBullet, " ");
	return;
}

void bulletUpdate(){
	while(bulletLife == TRUE)
	{
		if(yBullet != yPlayer)
			mvprintw(yBullet, xBullet," ");
		yBullet--;
		
		if(yBullet == 0) 
			bulletDestroy();
		else
			mvprintw(yBullet, xBullet, bullet);
	
		break;
	}
}



//mushroom functions
void createMushroom()
{
	time_t t;
	srand((unsigned) time(&t));
	
	for(j = 0; j < DefaultNumberOfMushrooms; j++)
	{
		mushroomLife[j] = MushroomLife; //hits that can sustain
		yMushroom[j] = (rand() %(YBoundary-2)) + 1;
		xMushroom[j] = rand() %XBoundary;
	}
	for(j = DefaultNumberOfMushrooms; j < NumberOfMushroom; j++)
	{
		mushroomLife[j] = 0; //reserved for defeated monsters
		yMushroom[j] = -1;
		xMushroom[j] = -1;
	}
	mushroomUpdate();
}

void mushroomUpdate()
{
	for(j = 0; j < NumberOfMushroom; j++)
	{
		while(mushroomLife[j] > 0)
		{
			//attron(COLOR_PAIR(mushroomLife[j]));
			mvprintw(yMushroom[j], xMushroom[j], mushroom);
			//attroff(COLOR_PAIR(mushroomLife[j]));
			break;
		}
	}
}



//handle all collisions of all elements in the game
void collision()
{
	
	for(i = 0; i < NumberOfMonster; i++)
	{
		while(monsterLife[i] == TRUE)
		{
	
			for(j = 0; j < NumberOfMushroom; j++)
			{
				while(mushroomLife[j] > 0)
				{
					//bullet mushroom collision
					if(yBullet == (yMushroom[j] + 1) && xBullet == xMushroom[j])
					{
						bulletDestroy();
						mushroomLife[j] = mushroomLife[j] - 1;
					}
					
					//monster mushroom  && monster boundary collision
					if(((xMonster[i] == (xMushroom[j] + 1) || xMonster[i] == (xMushroom[j] - 1))
						&& (yMonster[i] == yMushroom[j])) || (0 == xMonster[i] || xMonster[i] == XBoundary))
					{
						xMonsterSpeed[i] = -xMonsterSpeed[i];
						mvprintw(yMonster[i],xMonster[i]," ");
						xMonster[i] = xMonster[i] + xMonsterSpeed[i];
						yMonster[i]++;
						break;
					}
					break;
				}
				
			}
			
			//monster bullet collision
			if(xMonster[i] == xBullet && yMonster[i] == yBullet)
			{
				bulletDestroy();
				monsterDestroy(i);
				
			}
			
			
			//monster player collision
			if((xPlayer == xMonster[i] && yPlayer == yMonster[i]) || ( YBoundary < yMonster[i]))
				playing = FALSE;
			
			break;
		}
		monsterUpdate();
		mushroomUpdate();
	}
	return;
}


int gameSetup()
{
	initscr();			/* Start curses mode 		*/
	getmaxyx(stdscr,YBoundary,XBoundary); //get size of window
	//raw();				/* Line buffering disabled	*/
	keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
	noecho();			/* Don't echo() while we do getch */
	curs_set(0);
	start_color();
	
	/* Initialize all the colors */
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_BLUE, COLOR_BLACK);
	
	xPlayer = XBoundary/2;
	yPlayer = YBoundary-1;
	
	createMonster();
	playerUpdate();
	createMushroom();
	refresh();
	return(1);
}

void gameUpdate()
{
	halfdelay(1); //set delay for getch()
	ch = getch();
	if(ch != ERR) //ERR = no input
	{
		movePlayer(ch);
		usleep(WAIT); //timeout
	}
		
	collision();
	bulletUpdate();
		
	refresh();
	return;
}



