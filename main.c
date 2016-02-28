 /**
 * Breakout game
 * created by Ziyin Zhang
 */

#include "myLib.h"
#include "text.h"
#include <stdlib.h>
#include <stdio.h>
#include "background.h"
#include "winscreen.h"
#include "losescreen.h"

// Prototypes
void start();
void game();
void pause();
void win();
void lose();

void initialize();
int endRound();
void update();
void draw();
int collisionCheckBrickBall(BRICK* o, BALL* b);

unsigned int buttons;
unsigned int oldButtons;

int state;
int seed;
char str[41];
int score = 0;
BRICK bricks[40];
PLAYER player;
BALL ball;

int main()
{
	REG_DISPCTL = MODE3 | BG2_ENABLE;
	drawbgImage(&backgroundBitmap);
	sprintf(str, "PYRAMID BREAKOUT");
	drawString(60, 75, str, YELLOW);
	sprintf(str, "PRESS START");
	drawString(130, 87, str, WHITE);
	state = STARTSCREEN;

	while (1) {
		oldButtons = buttons;
		buttons = BUTTONS;

        switch(state)
        {
        	case STARTSCREEN:
        		start();
        		break;
        	case GAMESCREEN:
        		game();
        		break;
        	case LOSESCREEN:
        		lose();
        		break;
        	case WINSCREEN:
        		win();
        		break;
        	case PAUSESCREEN:
        		pause();
        		break;
        }
	}
	return 0;
}


void initialize()
{
	score = 0;
	//initialize player
	player.width = 50;
	player.column = 95;
	player.oldColumn = player.column;
	player.speed = 2;
	player.lives = 5;

	//initialzie ball
	ball.row = 147;
	ball.col = 118;
	ball.oldrow = 80;
	ball.oldcol = 120;
	ball.speed = 1;
	ball.rd = ball.speed;
	ball.cd = ball.speed;
	ball.size = 8;

	unsigned short colors[5];
	for (int j = 0; j < 5; j++) {
		colors[j] = rand()%WHITE + 1;
	}

	//initialize bricks
	for (int i = 0; i < 40; i++) {
		bricks[i].row = (i / 8) * 15; //get #row of the brick, and multiply by the height
		bricks[i].col = (i % 8) * 30; //get #col of the brick, and multiply by the length
		
		bricks[i].row += 1;			//move the bricks down by one
		bricks[i].col += 1;			//get the bricks centered
		int numRow = i / 8;
		//bricks[i].color = rand()%WHITE + 1;
		
		switch(numRow) {
			case 0:
				bricks[i].color = colors[numRow]; //RED
				break;
			case 1:
				bricks[i].color = colors[numRow]; //YELLOW
				break;
			case 2:
				bricks[i].color = colors[numRow]; //GREEN
				break;
			case 3:
				bricks[i].color = colors[numRow]; //BLUE
				break;
			case 4:
				bricks[i].color = colors[numRow]; //MEGENTA
		}
		bricks[i].length = 28;
		bricks[i].height = 10;
		bricks[i].active = 1;
		drawRect(bricks[i].row, bricks[i].col, bricks[i].height, bricks[i].length, bricks[i].color);
	}
}

void start()
{
    seed++;
    if (BUTTON_PRESSED(BUTTON_START))
    {
    	state = GAMESCREEN;
    	fillScreen(BLACK);
    	srand(seed);
    	initialize();
    }
}

void game()
{
    update();
    waitForVblank();
    draw();
	if (BUTTON_PRESSED(BUTTON_START))
	{
		state = PAUSESCREEN;
		drawbgImage(&backgroundBitmap);
		sprintf(str, "PRESS START TO RESUME");
		drawString(130, 57, str, WHITE);
	}
	
	if (state == WINSCREEN){
		drawbgImage(&winscreenBitmap);
		char str[41];
		sprintf(str, "You broke out!!");
		drawString(130, 50, str, YELLOW);
		sprintf(str, "PRESS START TO PLAY AGAIN");
		drawString(150, 50, str, WHITE);		
	}

	if (state == LOSESCREEN){
		drawbgImage(&losescreenBitmap);
		char str[41];
		sprintf(str, "You failed to escape :(");
		drawString(50, 51, str, RED);
		sprintf(str, "Your final Score is: %d", score);
		drawString(139, 20, str, WHITE);
		sprintf(str, "PRESS START TO PLAY AGAIN");
		drawString(150, 50, str, WHITE);
	 }
}

void pause()
{
    if (BUTTON_PRESSED(BUTTON_START))
    {
    	state = GAMESCREEN;
    	fillScreen(BLACK);
    	for (int i = 0; i < 40; i++)
    	{
    		if (bricks[i].active)
    			drawRect(bricks[i].row, bricks[i].col, bricks[i].height, bricks[i].length, bricks[i].color);
    	}
    }
}

void win()
{
	if (BUTTON_PRESSED(BUTTON_START))
	{
		state = STARTSCREEN;
		drawbgImage(&backgroundBitmap);
		sprintf(str, "PYRAMID BREAKOUT");
		drawString(60, 75, str, YELLOW);
		sprintf(str, "PRESS START");
		drawString(130, 87, str, WHITE);
	}
}

void lose()
{
    if (BUTTON_PRESSED(BUTTON_START))
    {
    	state = STARTSCREEN;
    	drawbgImage(&backgroundBitmap);
    	sprintf(str, "PYRAMID BREAKOUT");
		drawString(60, 75, str, YELLOW);
		sprintf(str, "PRESS START");
		drawString(130, 87, str, WHITE);
    }
}

void update()
{

	ball.oldrow = ball.row;
	ball.oldcol = ball.col;
	player.oldColumn = player.column;

	//player control
	if(KEY_DOWN_NOW(BUTTON_LEFT) && player.column > 2)
	{
		player.column -= player.speed;
	}
	if(KEY_DOWN_NOW(BUTTON_RIGHT) && player.column + player.width < 238)
	{
		player.column += player.speed;
	}

	ball.row += ball.rd;
	ball.col += ball.cd;

	//check brick collision
	int win = 1;
	for (int i = 0; i < 40; i++)
	{
		if (bricks[i].active)
		{
			if(collisionCheckBrickBall(&bricks[i], &ball))
			{
				win = 0;
				break;
			}
			win = 0;
		}
	}
	if (win == 1)	//all bricks are inactive, then win
	{
		state = WINSCREEN;
	}

	//check for paddle collision
	if (ball.row > 147)
	{
		if (ball.col + ball.size / 2 > player.column && ball.col + ball.size / 2 < player.column + player.width)
		{
			ball.row = 147;
			ball.rd = -ball.rd;
		} 
	}

	// Check for wall collisions
	if(ball.row < 0) //touches top
	{
		ball.row = 0;
		ball.rd = -ball.rd;
	}
	if(ball.row>159) //touches bottom
	{
		ball.row = 159;
		ball.rd = -ball.rd;
		player.lives--;
		endRound();
	}
	if(ball.col < 0) //touches left
	{
		ball.col = 0;
		ball.cd = -ball.cd;
	}
	if(ball.col>240-ball.size) //touches right
	{
		ball.col = 239-ball.size*2;
		ball.cd = -ball.cd;
	}
}

void draw() {
	waitForVblank();
	//draw paddle and number of lives
	drawRect(155, player.oldColumn, 5, player.width, BLACK);
	drawRect(155, player.column, 5, player.width, WHITE);

	sprintf(str, "Score: %d", score);
	drawRect(130, 100 + 6 * 7, 8, 12, BLACK); //cover up the previous score
	drawString(130, 100, str, GREY);

	drawCircle(ball.oldrow, ball.oldcol, ball.size, BLACK);
	drawCircle(ball.row, ball.col, ball.size, YELLOW);
}

/*
	This function will run after player dies, it checks the current
	state of game (check if anyone looses) and randomlly genertate
	a direction of the next ball	
*/
int endRound() 
{
	drawRect(155, 0, 5, 240, BLACK);
	player.width = player.width - 10;
	player.column = (240 - player.width) / 2;
	if (player.lives == 0) {
		state = LOSESCREEN;
		return 0;
	}
	ball.row = 147;
	ball.col = 118;
	drawCircle(ball.row, ball.col, ball.size, YELLOW);

	drawRect(155, player.column, 5, player.width, WHITE);
	delay(20);//use let the ball sit for a while before restart the game

	//randomlly generates the direction of the ball
	ball.rd = -(rand()%2 + ball.speed);
	ball.cd = -(rand()%2 + ball.speed);

	if (rand()%2) {
		ball.cd = ball.cd;
	} else {
		ball.cd = -ball.cd;
	}
	return 0;
}

int collisionCheckBrickBall(BRICK* o, BALL* b)
{
	if(o->col <= b->col + b->size && o->col + o->length >= b->col && o->row + o->height >= b->row && o->row <= b->row + b->size)
	{
		if (b->oldrow + b->size <= o->row
            || b->oldrow >= o->row + o->height) //ball comes from top or bottom
        {
            b->rd = -(b->rd);
        }
        if (b->oldcol + b->size <= o->col
            || b->oldcol >= o->col + o->length) //ball comes from left or right
        {
            b->cd = -(b->cd);
        }
		o->active = 0;
		score++;
		drawRect(o->row, o->col, o->height, o->length, BLACK);
		return 1;
	}
	return 0;
}