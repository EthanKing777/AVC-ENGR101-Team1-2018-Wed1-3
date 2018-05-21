#include <stdio.h>
#include <sys/time.h>
#include "E101.h"

FILE *file; //Log file initialisation
int np = 0;


void connectServer () {
	/* the ip address of the gate */
	char serverAddr[15] = {"130.195.6.196"}; 
	int port = 1024; /*port num of gate*/
	char message[24] = {"Please"};
	/* gate should return 0 when connection 
	 * is approved, run until it returns 0(not finished).
	 * When it returns 0 it will have modified
	 * the string called "message"*/
	int a = 1;
	while (a!=0) { 
		a = connect_to_server(serverAddr, port);
		sleep1(1,0); //to prevent flooding the server
	}
	int b = 1;
	while (b!=0) {
		b = send_to_server(message);
		sleep1(1,0); //to prevent flooding the server
	}
	receive_from_server(message);
	send_to_server(message);
	fprintf(file, "Gate Opened - Quad 1 start\n");
}

/* calculates average to figure out what is white and what is black */ 
int calcthreshold () { 
	unsigned char pix; //total light intensity of each pixel
	int max = 0; //Maximum light intensity 
	int min = 999; //Minimum light intensity
	int columns = 320; //No. of columns in image (x coord)
	int rows = 240; //No. of rows in image (y coord)
	take_picture();
	display_picture(1,0);
	/* For loop to read the all the values in the
	 * middle row of the stored image  and store 
	 * min and max values */
	for (int i=0;i<columns;i++) {
		pix = get_pixel(rows/2,i,3);
		if (pix > max) {
			max = pix;
		}
		if (pix<min) {
			min = pix;
		}
	}
	int threshold = (max-min)/2; // Threshold value
	/* Log max, min and threshold values*/
	fprintf(file, "Max: %d  Min: %d\nThreshold: %d\n", max, min, threshold);
	return threshold;
}


/* Method to calculate the error value from white line */
int calcError (int threshold) {
	int line = 0; // error values
	/* Binary array for the middle row of pixels in the image
	 * 1 = white, 0 = black */
	int white[320]; 
	np = 0; // No. of white pixels (moderates diagonal reading)
	take_picture();
	unsigned char pix; //as per calcthreshold()
	int columns = 320;
	int rows = 240;
	/* For loop to fill in array with values (1 or 0)
	 * Counts number of white pixels found (np) */
	for (int i=0; i<columns; i++) {
		pix = get_pixel(rows/2,i,3);
		if (pix > threshold) {
			white[i] = 1;
			np = np + 1;
		}
		else {
			white[i] = 0;
		}
		/* Logs all 320 white[] values can be commented 
		 * back in for testing purposes.
		fprintf(file, "Power to motors: %d\n", power);*/
	}
	/* Produces the error value based on the location of the white
	 * pixels in the array then normalises the error based on np*/
	for (int i=0; i<columns; i++) { 
		line = line + (i-160)*white[i];
		fprintf(file, "%d", white[i]);
	}
	fprintf(file, "\n\n");
	if (np>0) { //To prevent crashes
		line = line/np;
	} 
		
	else{
		fprintf(file, "np = 0");
	}
	return line;	
}

/* Function to calculate the amount of power to give to
 * the motors based on the error values and the PID controller*/
void direction(int error, int prevError, 
long elasped, long prevtime) {
	int vGo = 60; /* speed when bot is moving forward */
	float Kp = -0.5; /* amount bot reacts to a change in the error, is proportional to amount is turns by */
	float Kd = -0;
	float diff = 0;
	float dv = 0;
	int vLeft = 0; //Power to left motor
	int vRight = 0; //Power to right motor
	if (elasped == prevtime){
		prevtime = 0;
	}
		diff = (error-prevError)/(elasped-prevtime);
		dv = ((error*Kp) + (diff*Kd)); //should this be just dv = error*Kp ? 
		vLeft = vGo-dv;
		vRight = vGo+dv;
		set_motor(1,vRight);
		set_motor(2,-vLeft);


	/* Prints all the relevant stats to the log file */
	fprintf(file, "%d\t%ld\t%f\t%f\t%d\t%d\n", error, elasped, diff, dv, vLeft, vRight);
	sleep1(0,010000);	
}

int main(){
	init();
	//Initiate log file for reporting
	file = fopen("log_file.txt", "w");
	fprintf(file, "\nStarting run\n");
	connectServer(); //Open gate */
	/*int quad = 0;
	if (quad == 0){
		
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
	int error;
	int prevError = 0;
	long prevtime = 0;
	long elasped = 0;
	int x = 0;
	int threshold = calcthreshold(); /* gets threshold value which is whitest pixel value + darket pixel value / 2 */
	fprintf(file, "err\telapsed\t\tdiff\tdv\tvLeft\tvRight\n"); //Sets up columns of values
	for (x=0;x<2000;x=x+1) {
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
	fprintf(file, "Run finished\n\n"); //to differentiate between runs
return 0;}

