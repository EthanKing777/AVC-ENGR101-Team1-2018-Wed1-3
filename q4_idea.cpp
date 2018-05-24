/*int getSensor(int sensor){
	x = read_analog(sensor);
	return x;
}

void quad4TurnRight(){
	set_motor(1, 0);
	set_motor(2, 0);
	sleep1(0, 500000);
	set_motor(1, -40);
	set_motor(2, 0);
	sleep1(0,333333);
	fprintf(file, "\nturn Right   \n");
}

void quad4TurnLeft(){
	set_motor(1, 0);
	set_motor(2, 0);
	sleep1(0, 500000);
	set_motor(1,0);
	set_motor(2, 40);
	sleep1(0,333333);
	fprintf(file, "\nturn Left\n");
}

void quad4Forwards(){
	set_motor(1, 0);
	set_motor(2,0);
	sleep1(0, 500000);
	set_motor(1,-40);
	set_motor(2,40);
	sleep1(0, 300000);
	}
	
void quad4Backwards(){
	set_motor(1, 0);
	set_motor(2,0);
	sleep1(0, 500000);
	set_motor(1,40);
	set_motor(2,-40);
	sleep1(0, 300000);
	}


void quad4() {
	int Left = getSensor(0);
	int Right = getSensor(1);
	int Front = getSensor(2);
	if(front < //some value(get close to a wall somewhat)){
		if(right > left){
			quad4TurnRight();
		}else if (Left > Right){ 
			quad4TurnLeft();
		}else if (front < //some value(too close to wall)){
			quad4Backwards();
			}
	}else{
		quad4Forwards();/
			}
		}*/
