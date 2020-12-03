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

	ElevatorSimulation(ifstream& file); // ×åòå âõîäíèòå äàííè îò òåêñòîâ ôàéë

	int Send_Elevator(const Requests& req); // Âðúùà 1(2) çà äà èçïðàòè 1âè(2ðè) àñàíñüîð è 0, àêî íÿìà ñâîáîäåí àñàíñüîð
	void Set_Order(const Requests& req, list<Requests>& accepted_requests, Elevator& elevator); // Ïîäðåæäà çàÿâêèòå íà êà÷èëèòå ñå õîðà, òàêà ÷å front() äà å ïúðâàòà çàÿâêà, êîÿòî òðÿáâà äà ñå èçïúëíè
	void Set_direction(const Requests& req, const Requests& next_req, Elevator& elevator); // Çàäàâà ïîñîêà íà äâèæåíèå íà àñàíñüîðà	
	void Accept_requests(); // Îïèòâà äà ïðèåìå çàÿâêè îò åòàæèòå

	bool GetIn_elevator(Elevator& elevator, list<Requests>& accepted_req); // Âëèçàíå íà ïúòíèöè â àñàíñüîð
	bool GoOff_from_elevator(Elevator& elevator, list<Requests>& accepted_req); // Ñëèçàíå íà ïúòíèöè îò àñàíñüîð
	bool Time_Optimization(); // Îïòèìèçèðà ïðîãðàìàòà. Ïðîïóñêà ñòúïêè, êîãàòî å âúçìîæíî
	void Move_Elevator(Elevator& elevator, list<Requests>& accepted_req, char c); // Äâèæåíèåòî íà àñàíñüîðà

	void Run(); // Èçïúëíÿâà ïðîãðàìàòà

private:

	 Office_building office_building;
	 list<Requests> all_requests;
	 list<Requests> accepted_req_by1, accepted_req_by2;
	 // Èçïîëçâàì äâîéíîñâúðçàí ñïèñúê, à íå åäíîñâúðçàí çàðàäè óïîòðåáà íà push_back(), insert(), erase()

	 Elevator elevator1, elevator2;
};

