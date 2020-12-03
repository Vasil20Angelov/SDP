#pragma once
#include <iostream>
#include <string>
using namespace std;

struct Office_building 
{
	int floors;
	int elevator_size1;
	int elevator_size2;
	int requests;
};

struct Requests
{
	string dir;
	int floor;
	int time;
	bool isElevatorSent = false;
};

struct Elevator
{
	int curr_floor = 1;
	int movement = 0;
	int direction_floor = 0;
	int people = 0;
	int size = 0;
	string next_dir = "";
	string dir = "";

	const bool isFull() const { return people == size; };
	const bool isMoving() const { return (dir != ""); };
};
