#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>

#define WAIT 100000
#define esc 27

#define monster "@"
#define player "+"
#define bullet "^"
#define mushroom ";"

#define NumberOfMonster 9 //must not be greater than XBoundary
#define SpeedOfBullet 3
#define DefaultNumberOfMushrooms 40
#define NumberOfMushroom DefaultNumberOfMushrooms + NumberOfMonster
#define MushroomLife 5

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
void mushroomUpdate(int j);

void collision();

int gameSetup();
void gameUpdate();
void scoreUpdate();
int gameEnd();

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
int monsterLeft = NumberOfMonster;
int monsterIndicator[NumberOfMonster];

int xBullet, yBullet, bulletLife;

int yMushroom[NumberOfMushroom];
int xMushroom[NumberOfMushroom];
int mushroomLife[NumberOfMushroom];

int score;

int i, j, k; //temp variables

//wait for input
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
  return 0;  
}  

//Main Function
int main()
{	
	if(gameSetup() == 0) playing = FALSE;
	
	mvprintw(YBoundary/2, (XBoundary-22)/2, "Press any key to start");
	getch();
	
	while (playing == TRUE)
		gameUpdate();
	
	
	while(gameEnd() != esc)
	{
		mvprintw(YBoundary/2,(XBoundary-24)/2,"GAMEOVER, YOUR SCORE IS %d", score);
		usleep(WAIT*30);
	}
	
	endwin();/* End curses mode*/
	
	printf("GAMEOVER, YOUR SCORE IS %d#n", score);
	return(0);

}


//player functioins
void playerUpdate()
{
	//print player
	attron(COLOR_PAIR(1));
	mvprintw(yPlayer,xPlayer,player);
	attroff(COLOR_PAIR(1));
	return;
}

void movePlayer(int input)
{
	//input value switch
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
void createMonster ()//monster setup
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

void monsterUpdate() //move monster
{
	for(i = 0; i < NumberOfMonster; i++)
	{
		while(monsterLife[i] == TRUE)
		{
			mvprintw(yMonster[i],xMonster[i]," ");
			xMonster[i] = xMonster[i] + xMonsterSpeed[i];
				
			printMonster(i);
			break;
		} 
	}
	if(monsterLeft == 0) playing = FALSE;
}

void printMonster (int i)
{
	mvprintw(yMonster[i], xMonster[i], monster);
	return;
}

void monsterDestroy(int i) //destroy monster if intercept with bullet
{
	monsterLife[i] = FALSE;
	yMushroom[DefaultNumberOfMushrooms + i] = yMonster[i];
	xMushroom[DefaultNumberOfMushrooms + i] = xMonster[i];
	mushroomLife[DefaultNumberOfMushrooms + i] = MushroomLife;
	//check how many monsters left
	monsterLeft = monsterLeft - 1; 
}



//bullet functions
void bulletCreate() //setup bullet while KEY_UP is pressed
{
	bulletLife = TRUE;
	yBullet = yPlayer;
	xBullet = xPlayer;
	return;
}

void bulletDestroy()// destroy bullet when out of bound || touched monster
{
	bulletLife = FALSE;
	mvprintw(yBullet, xBullet, " ");
	return;
}

void bulletUpdate(){ //move bullet
	while(bulletLife == TRUE)
	{
		if(yBullet != yPlayer)
			mvprintw(yBullet, xBullet," ");
		yBullet = yBullet - 1;
		
		if(yBullet == 0) 
			bulletDestroy();
		else
			mvprintw(yBullet, xBullet, bullet);
	
		break;
	}
}



//mushroom functions
void createMushroom()//mushroom setup
{
	time_t t;
	srand((unsigned) time(&t));
	
	for(j = 0; j < DefaultNumberOfMushrooms; j++)
	{
		mushroomLife[j] = MushroomLife; //hits that can sustain
		yMushroom[j] = (rand() %(YBoundary-2)) + 1;
		xMushroom[j] = rand() %XBoundary;
		mushroomUpdate(j);
	}
	for(j = DefaultNumberOfMushrooms; j < NumberOfMushroom; j++)
	{
		mushroomLife[j] = 0; //reserved for defeated monsters
		yMushroom[j] = -1;
		xMushroom[j] = -1;
	}
	
}

void mushroomUpdate(int j) //update life of mushroom
{
	while(mushroomLife[j] > 0)
	{
		attron(COLOR_PAIR(mushroomLife[j]));
		mvprintw(yMushroom[j], xMushroom[j], mushroom);
		attroff(COLOR_PAIR(mushroomLife[j]));
		break;
	}

}



//handle all collisions of all elements in the game (logic of the game)
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
					
					//monster mushroom  && monster x-boundary collision
					if(((xMonster[i] == (xMushroom[j]) || xMonster[i] == (xMushroom[j]))
						&& (yMonster[i] == yMushroom[j])) || (0 == xMonster[i] || xMonster[i] == XBoundary))
					{
						if(monsterIndicator[i] == 0)
						{
							xMonsterSpeed[i] = -xMonsterSpeed[i];
							mvprintw(yMonster[i],xMonster[i]," ");
							yMonster[i]++;
							monsterIndicator[i] = 1;
						}else{
							xMonster[i] = xMonster[i] + xMonsterSpeed[i];
							monsterIndicator[i] = 0;
						}
						break;
					}
					mushroomUpdate(j);
					break;
				}
				
			}
			
			//monster bullet collision
			if(xMonster[i] == xBullet && yMonster[i] == yBullet)
			{
				bulletDestroy();
				monsterDestroy(i);
				scoreUpdate(); //add score
			}
			
			
			//monster player collision
			if(xPlayer == xMonster[i] && yPlayer <= yMonster[i])
				playing = FALSE;
			
			break;
		}
		
	}
	
	for(j = 0; j < NumberOfMushroom; j++)
	{
		while(mushroomLife[j] != 0)
		{
			//bullet mushroom collision
			if(yBullet == (yMushroom[j] + 1) && xBullet == xMushroom[j])
			{
				bulletDestroy();
				mushroomLife[j] = mushroomLife[j] - 1;
			}
		break;
		}
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
	init_pair(4, COLOR_CYAN, COLOR_BLACK);
	init_pair(5, COLOR_YELLOW, COLOR_BLACK);
	
	//initialize player start position
	xPlayer = XBoundary/2;
	yPlayer = YBoundary-1;
	
	score = 0;
	mvprintw(0,XBoundary/2,"monsters left : %d", monsterLeft);
	mvprintw(0,0,"score : %d", score);
	
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
	
	monsterUpdate();
	
	//controls the number of loops of moving bullet compared to 1 time movement of monster
	for(k = 1; k < SpeedOfBullet; k++)
	{
		bulletUpdate();
		collision();
	}
	
		
	refresh();
	return;
}

void scoreUpdate()
{
	score++;
	mvprintw(0,0,"score : %d", score);
	mvprintw(0,XBoundary/2,"monsters left : %d", monsterLeft);
}

int gameEnd()
{
	ch = getch();

		
	return(ch);

}


