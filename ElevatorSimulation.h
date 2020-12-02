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

	ElevatorSimulation(ifstream& file); // ���� �������� ����� �� ������� ����

	int Send_Elevator(const Requests& req); // ����� 1(2) �� �� ������� 1��(2��) �������� � 0, ��� ���� �������� ��������
	void Set_Order(const Requests& req, list<Requests>& accepted_requests, Elevator& elevator); // �������� �������� �� �������� �� ����, ���� �� front() �� � ������� ������, ����� ������ �� �� �������
	void Set_direction(const Requests& req, const Requests& next_req, Elevator& elevator); // ������ ������ �� �������� �� ���������	
	void Accept_requests(); // ������ �� ������ ������ �� �������

	bool GetIn_elevator(Elevator& elevator, list<Requests>& accepted_req); // ������� �� ������� � ��������
	bool GoOff_from_elevator(Elevator& elevator, list<Requests>& accepted_req); // ������� �� ������� �� ��������
	bool Time_Optimization(); // ���������� ����������. �������� ������, ������ � ��������
	void Move_Elevator(Elevator& elevator, list<Requests>& accepted_req, char c); // ���������� �� ���������

	void Run(); // ��������� ����������

private:

	 Office_building office_building;
	 list<Requests> all_requests;
	 list<Requests> accepted_req_by1, accepted_req_by2;
	 // ��������� ������������� ������, � �� ����������� ������ �������� �� push_back(), insert(), erase()

	 Elevator elevator1, elevator2;
};

