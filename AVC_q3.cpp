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
		
	/*else{
		fprintf(file, "np = 0");
	}*/
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
	fprintf(file, "%d\t%d\t%f\t%f\t%d\t%d\n", error, np, diff, dv, vLeft, vRight);
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

int sumArray(int Arr[] ){
	int T = 0;
	for(int i=0; i< 240; i++){ //240 = size of array
		T += Arr[i];
	}
	return T;
}


void turnLeft(){
	set_motor(1, 0);
	set_motor(2, 0);
	sleep1(0, 500000);
	set_motor(1,80);
	set_motor(2, 0);
	sleep1(0,333333);
	fprintf(file, "\nturn Left\n");
}

void turnRight(){
	set_motor(1, 0);
	set_motor(2, 0);
	sleep1(0, 500000);
	set_motor(1, 0);
	set_motor(2, -80);
	sleep1(0,333333);
	fprintf(file, "\nturn Right   \n");
}

void turnAround(){
	set_motor(1, 0);
	set_motor(2, 0);
	sleep1(0, 500000);	
	set_motor(1,-50);
	set_motor(2,-50);
	sleep1(0,600000);
	fprintf(file, "\nturn Around\n");
}

void turn(int threshold){
	int whiteVertLeft[240];
	int whiteVertRight[240]; 
	int rows = 240;
	for(int i=0;i<rows;i++) {
		int pix1 = get_pixel(i,rows/3,3);
		if (pix1 > threshold) {
			whiteVertLeft[i] = 1;
			
		}
		else {
			whiteVertLeft[i] = 0;
		}
		
		
		int pix2 = get_pixel(i,rows*(2/3),3);
		if (pix2 > threshold) {
			whiteVertRight[i] = 1;
			
		}
		else {
			whiteVertRight[i] = 0;
		}
	}
	if ((sumArray(whiteVertLeft)< 20) && (sumArray(whiteVertRight) > 20)){
		turnRight();
	} else if (sumArray(whiteVertLeft) > 20){
		turnLeft();
	} else {
		turnAround();
	}
}
	
	
void goStriaght(int error, int prevError, 
long elasped, long prevtime){
	if(!np< 20){
		set_motor(1, 0);
		set_motor(2,0);
		sleep1(0, 500000);
		set_motor(1,40);
		set_motor(2,-40);
		sleep1(0, 300000);
		for(int i = 0; i<500; i++){
		direction(error,prevError, elasped, prevtime);
		}
	}
	set_motor(1,0);
	set_motor(2,0);
}

int main(){
	init();
	//Initiate log file for reporting
	file = fopen("log_file.txt", "w");
	fprintf(file, "\nStarting run\n");
	int quad = 0;
	struct timeval t1;
	struct timeval t2;
	gettimeofday(&t1,0);
	int error;
	int prevError = 0;
	long prevtime = 0;
	long elasped = 0;
	int count = 0;
	int threshold = calcthreshold(); /* gets threshold value which is whitest pixel value + darket pixel value / 2 */
	fprintf(file, "err\tnp\t\tdiff\tdv\tvLeft\tvRight\n"); //Sets up columns of values
	/*while (true) {
		if (quad == 0){
			connectServer(); //Open gate 
			quad = 1;
		}
		if (quad == 1) {
			gettimeofday(&t2,0);
			elasped = (t2.tv_sec - t1.tv_sec)*1000000 + (t2.tv_usec - t1.tv_usec); 
			gettimeofday(&t1,0);
			error = calcError(threshold);
			direction(error,prevError,elasped,prevtime);
			prevError = error;
			prevtime = elasped;
			if (np==320) {
				quad = 2;
			}
		}
		if (quad == 2) {
			if (findRed() == 1) {
				quad = 3;
			} else {
				error = calcError(threshold);
				if (np==0 && count == 0) {
					set_motor(1,40);
					set_motor(2,0);
					sleep1(2,0);
					count = 1;
				} else if (np==0 && count == 1) {
					set_motor(1,0);
					set_motor(2,-40);
					sleep1(4,0);
					count = 0;
				} else {
					gettimeofday(&t2,0);
					elasped = (t2.tv_sec - t1.tv_sec)*1000000 + (t2.tv_usec - t1.tv_usec); 
					gettimeofday(&t1,0);
					direction(error,prevError,elasped,prevtime);
					prevError = error;
					prevtime = elasped;
				}
			}
		}
		if (quad == 3) {
			quad = 4;
		}
	}*/
	int i = 0;
	for(i=0;i<1000;i=i+1) {
			if (findRed() == 1) {
				quad = 3;
				printf("Stopped");
			} else {
				error = calcError(threshold);
				if((np>=20) && (np<200)) {
					gettimeofday(&t2,0);
					elasped = (t2.tv_sec - t1.tv_sec)*1000000 + (t2.tv_usec - t1.tv_usec); 
					gettimeofday(&t1,0);
					direction(error,prevError,elasped,prevtime);
					prevError = error;
					prevtime = elasped;
				}
				else {		
					turn(threshold);
				}
		}
	}
	
	set_motor(1,0);
	set_motor(2,0);
	fprintf(file, "Run finished\n\n"); //to differentiate between runs
return 0;
}

