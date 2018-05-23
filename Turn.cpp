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
	//save_picture("Thr");
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
	int vGo = 42; /* speed when bot is moving forward */
	float Kp = -0.27; /* amount bot reacts to a change in the error, is proportional to amount is turns by */
	float Kd = -0.2;
	float diff = 0;
	float dv = 0;
	int vLeft = 0; //Power to left motor
	int vRight = 0; //Power to right motor
	diff = (error-prevError)/elasped;
	dv = ((error*Kp) + (diff*Kd)); //should this be just dv = error*Kp ? 
	vLeft = vGo-dv;
	vRight = vGo+dv;
	set_motor(1,vRight);
	set_motor(2,-vLeft);


	/* Prints all the relevant stats to the log file */
	fprintf(file, "%d\t%ld\t%f\t%f\t%d\t%d\n", error, elasped, diff, dv, vLeft, vRight);
	sleep1(0,010000);	
}

int findRed() {
	take_picture();
	int pix = get_pixel(120,160,0);
	if (pix >= 200) {
		return 1;
	} else {
		return 0;
	}
	
}

int main(){
	init();
	//Initiate log file for reporting
	
	set_motor(1,0);
	set_motor(2,-150);
	sleep1(0, 333333);
	set_motor(1,0);
	set_motor(2,0);

return 0;}

