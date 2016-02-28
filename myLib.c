#include "myLib.h"


unsigned short *videoBuffer = (unsigned short *)0x6000000;

void setPixel(int r, int c, unsigned short color)
{
	videoBuffer[OFFSET(r, c, 240)] = color;
}
/*
void setPixel4(int r, int c, char index)
{
    int pixelNumber = OFFSET(r, c, 240);
    int shortNumber = pixelNumber/2;
    unsigned short theShort = videoBuffer[shortNumber];
    if (col & 1)
    {
        vb[shortNumber] = (theShort & 0x00FF) | (index << 8);
    } else
    {
        vb[shortNumber] = (theShort & 0xFF00) | (index);
    }
}

void setPixel(int r, int c, char index)
{
    if(c & 1)
        vb[OFFSET(r, c, 240) / 2] = (vb[OFFSET(r, c, 240) / 2] & 0x00FF) | (index << 8);
    else
    {

    }
}*/

void DMANow(int channel, volatile const void* source, volatile const void* destination, unsigned int control)
{
    DMA[channel].src = source;
    DMA[channel].dst = destination; //videoBuffer[OFFSET(row+r, col, 240)]
    DMA[channel].cnt = control | DMA_ON;    
}

void drawRect(int row, int col, int height, int width, unsigned short color)
{
    volatile unsigned short c = color;
    for(int i = 0; i <= height; i++)
    {
        DMANow(3, &c, &videoBuffer[OFFSET(row + i, col, 240)], width | DMA_SOURCE_FIXED );
    }       
}

void fillScreen(unsigned short color)
{
    volatile unsigned short c = color;
    DMANow(3, &c, &videoBuffer[OFFSET(0, 0, 240)], 38400 | DMA_SOURCE_FIXED );
}

void drawCircle(int row, int col, int size, unsigned short color)
{
    //volatile unsigned short c = color;
    int radius = size / 2;
    for (int y = -radius; y <= radius; y++)
    {
        for (int x = -radius; x <= radius; x++)
        {
            if (x*x+y*y <= radius*radius - 1)
            {
                setPixel(row+x + radius, col+y + radius, color);
                //DMANow(3, &c, &videoBuffer[OFFSET(row+x+radius, col+y+radius, 240)], ??)
            }
        }
    }
}

void drawbgImage(const unsigned short * image)
{
     DMANow(3, image, &videoBuffer[OFFSET(0, 0, 240)], 38400);
}

void delay(int n)
{
	volatile int x = 0;
	for(int i=0; i<n*10000; i++)
	{
		x++;
	}
}

void waitForVblank()
{
	while(SCANLINECOUNTER > 160);
	while(SCANLINECOUNTER < 160);
}