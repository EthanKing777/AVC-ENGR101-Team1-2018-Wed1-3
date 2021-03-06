#include <stdio.h>
#include <sys/time.h>
#include "E101.h"

void connectServer () {
	char serverAddr[15] = {"130.195.6.196"}; /* the ip address of the gate */
	int port = 1024; /*port num of gate*/
	char messege[24] = {"Please"};
	int a = 1;
	while (a!=0) { /* gate should return 0 when connection is approved, run until it returns 0(not finished)*/
		a = connect_to_server(serverAddr, port);
	}
	int b = 1;
	while (b!=0) {
		b = send_to_server(messege);
	}
	receive_from_server(messege);
}
int calcthreshold () { /* calculates average to figure out what is white and what is black  */ 
	unsigned char pix;
	int i = 0;
	int max = 0;
	int min = 999;
	int columns = 320; /*not 100% sure these values are the right way around will print camera image in lab to check */
	int rows = 240;
	take_picture();
	/*takes the white intensity of the 
	 * middle row of pixels in the images*/
	for (i=0;i<columns;i=i+1) {
		pix = get_pixel(rows/2,i,3); 
		if (pix > max) {
			max = pix;
		}
		if (pix < min) {
			min = pix;
		}
	}
	printf("min %d\nmax %d\n", min, max);
	
	int threshold = (max-min)/2;
	return threshold;
}



int calcError (int threshold) {
	int line = 0; //error value
	int max = 0;
	int min = 999;
	int white[320]; //binary error array
	int np = 0; //Number of white pixels
	take_picture();
	unsigned char pix;
	int columns = 320;
	int rows = 240;
	for (int i=0;i<columns;i=i+1) {
		pix = get_pixel(rows/2,i,3);
		if (pix > threshold) {
			white[i] = 1;
			np = np + 1;
		}
		else {
			white[i] = 0;
		}
	}
	
	for (int i=0;i<columns;i=i+1) { 
		line = line + (i-160)*white[i];
	}
	if (np>0) {
		line = line/np;
	}
	/*error check*/
	else{
		printf("np = 0/n");
	}
	/*Print out white array*/
	for(int j = 0; j < columns; j++) {
		printf("%d ", white[j]);
	}
	return line;	
}

void direction(unsigned char error, unsigned char prevError, unsigned char elasped, unsigned char prevtime) {
	int vGo = 50; /* speed when bot is moving forward */
	unsigned char Kp = -0.005; /* amount bot reacts to a change in the error, is proportional to amount is turns by */
	unsigned char Kd = -0.05;
	int vLeft = 0;
	int vRight = 0;
	if (error == 0){
		int vLeft = -100; /* is this when the camera cannot sense the line it moves backwards? one motor is upside down so*/ 		
		int vRight = -100;/* both motors should not be set to the same value to move forwards or backwards or has that been fixed*/
	} else {
		unsigned char diff = (error-prevError)/(elasped-prevtime);
		unsigned char dv = (error*Kp) + (diff*Kd);
		int vLeft = -1*(vGo+dv);
		int vRight = vGo-dv;
	}
	unsigned char diff = (error-prevError)/(elasped-prevtime);
	printf("%d\n",diff); 
	
	unsigned char dv = (error*Kp) + (diff*Kd); /*should this be just dv = error*Kp ? */
	vLeft = -1*(vGo+dv);
	vRight = vGo-dv;
	set_motor(1,vRight);
	set_motor(2,vLeft);
	sleep1(0,500000);
	
	

	
}

int main(){
	init();
	connectServer();
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
	int threshold = calcthreshold(); /* gets threshold value which is whitest pixel value + darket pixel value / 2 */
	for (x=0;x<20;x=x+1) {
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
