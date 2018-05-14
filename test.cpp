#include <stdio.h>
#include <time.h>
#include "E101.h"

int calcError () {
	int line = 0;
	int max = 0;
	int min = 999;
	int white;
	take_picture();
	display_picture(1,000000);
	unsigned char pix;
	int i = 0;
	int columns = 320;
	int rows = 240;
	for (i=0;i<columns;i=i+1) {
		pix = get_pixel(rows/2,i,3);
		if (pix > max) {
			max = pix;
		}
		if (pix<min) {
			min = pix;
		}
	}
	int threshold = (max-min)/2;
	i = 0;
	for (i=0;i<columns;i=i+1) {
		pix = get_pixel(rows/2,i,3);
		line = line + (i-160)*pix;
		if (pix > threshold) {
			white = 1;
		}
		else {
			white = 0;
		}
	}
	return line;	
}

void direction(unsigned char error, unsigned char prevError) {
	int vGo = 100;
	unsigned char Kp = 0.005;
	unsigned char Kd = 0.05;
	unsigned char diff = error/0.5;
	unsigned char dv = (error*Kp) + (diff*Kd);
	int vLeft = -1*(vGo+dv);
	int vRight = vGo-dv;
	set_motor(1,vRight);
	set_motor(2,vLeft);
	sleep1(4,0);
	
	

	
}

int main(){
	init();
	unsigned char error;
	unsigned char prevError = 0;
	int x = 0;
	for (x=0;x<4;x=x+1) {
		error = calcError();
		direction(error,prevError);
		prevError = error;
	
	}
	set_motor(1,0);
	set_motor(2,0);

return 0;}
   
