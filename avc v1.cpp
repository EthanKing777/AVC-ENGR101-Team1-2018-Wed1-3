#include <stdio.h>
#include <sys/time.h>
#include "E101.h"

void connectServer () {
	char serverAddr[15] = {"130.195.6.196"};
	int port = 1024;
	char messege[24] = {"Please"};
	int a = 1;
	while (a!=0) {
		int a = connect_to_server (serverAddr, port);
	}
	int b = 1;
	while (b!=0) {
		int b = send_to_server(messege);
	}
	receive_from_server(messege);
}
int calcthreshold () {
	unsigned char pix;
	int i = 0;
	int max = 0;
	int min = 999;
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
	return threshold;
}



int calcError (int threshold) {
	int line = 0;
	int max = 0;
	int min = 999;
	int white[320]; 
	int np = 0;
	take_picture();
	unsigned char pix;
	int i = 0;
	int columns = 320;
	int rows = 240;
	i = 0;
	for (i=0;i<columns;i=i+1) {
		pix = get_pixel(rows/2,i,3);
		if (pix > threshold) {
			white[i] = 1;
			np = np + 1;
		}
		else {
			white[i] = 0;
		}
	}
	
	for (i=0;i<columns;i=i+1) { 
		line = line + (i-160)*white[i];
	}
	if (np>0) {
		line = line/np;
	}
	return line;	
}

void direction(unsigned char error, unsigned char prevError, unsigned char elasped, unsigned char prevtime) {
	int vGo = 50;
	unsigned char Kp = -0.005;
	unsigned char Kd = -0.05;
	int vLeft = 0;
	int vRight = 0;
	*if (error == 0){/this is where the comment was
		int vLeft = -100;
		int vRight = -100;
	} else {
		unsigned char diff = (error-prevError)/(elasped-prevtime);
		unsigned char dv = (error*Kp) + (diff*Kd);
		int vLeft = -1*(vGo+dv);
		int vRight = vGo-dv;
	}*
	unsigned char diff = (error-prevError)/(elasped-prevtime);
	printf("%d\n",diff);
	unsigned char dv = (error*Kp) + (diff*Kd);
	vLeft = -1*(vGo+dv);
	vRight = vGo-dv;
	set_motor(1,vRight);
	set_motor(2,vLeft);
	sleep1(0,500000);
	
	

	
}

int main(){
	init();
	/*int quad = 0;
	if (quad == 0){
		connectServer();
		quad = 1;
	}
	if (quad == 1) {
		quad = 2;
	}
	if (quad == 2) {
		quad = 3;
	}
	if (quad == 3) {
		quad = 4;
	}*/
	struct timeval t1;
	struct timeval t2;
	gettimeofday(&t1,0);
	unsigned char error;
	unsigned char prevError = 0;
	long prevtime = 0;
	long elasped = 0;
	int x = 0;
	int threshold = calcthreshold();
	for (x=0;x<60;x=x+1) {
		gettimeofday(&t2,0);
		elasped = (t2.tv_sec - t1.tv_sec)*1000000 + (t2.tv_usec - t1.tv_usec); 
		gettimeofday(&t1,0);
		error = calcError(threshold);
		direction(error,prevError,elasped,prevtime);
		prevError = error;
		prevtime = elasped;
		
	
	}
	set_motor(1,0);
	set_motor(2,0);

return 0;}
