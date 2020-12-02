#pragma once
#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <list>
#include <stdexcept>
#include "Structs.h"
using namespace std;

class ElevatorSimulation
{ 
public:
	static int curr_time;

	ElevatorSimulation(ifstream& file); // Чете входните данни от текстов файл

	int Send_Elevator(const Requests& req); // Връща 1(2) за да изпрати 1ви(2ри) асансьор и 0, ако няма свободен асансьор
	void Set_Order(const Requests& req, list<Requests>& accepted_requests, Elevator& elevator); // Подрежда заявките на качилите се хора, така че front() да е първата заявка, която трябва да се изпълни
	void Set_direction(const Requests& req, const Requests& next_req, Elevator& elevator); // Задава посока на движение на асансьора	
	void Accept_requests(); // Опитва да приеме заявки от етажите

	bool GetIn_elevator(Elevator& elevator, list<Requests>& accepted_req); // Влизане на пътници в асансьор
	bool GoOff_from_elevator(Elevator& elevator, list<Requests>& accepted_req); // Слизане на пътници от асансьор
	bool Time_Optimization(); // Оптимизира програмата. Пропуска стъпки, когато е възможно
	void Move_Elevator(Elevator& elevator, list<Requests>& accepted_req, char c); // Движението на асансьора

	void Run(); // Изпълнява програмата

private:

	 Office_building office_building;
	 list<Requests> all_requests;
	 list<Requests> accepted_req_by1, accepted_req_by2;
	 // Използвам двойносвързан списък, а не едносвързан заради употреба на push_back(), insert(), erase()

	 Elevator elevator1, elevator2;
};

