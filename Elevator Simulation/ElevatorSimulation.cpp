#include <stdexcept>
#include "ElevatorSimulation.h"
using namespace std;

int ElevatorSimulation::curr_time = 0;

ElevatorSimulation::ElevatorSimulation(ifstream& file)
{
	file >> office_building.floors;
	if (office_building.floors <= 1)
		throw logic_error("Invalid input! Building floors should be atleast 2!");

	file >> office_building.elevator_size1;
	file >> office_building.elevator_size2;
	if (office_building.elevator_size1 < 1 || office_building.elevator_size2 < 1)
		throw logic_error("Invalid input! Elevator size should be atleast 1!");

	file >> office_building.requests;

	string action;
	Requests temp_req;

	for (int i = 0; i < office_building.requests; i++)
	{
		file >> action;
		if (action == "call")
			file >> temp_req.dir;

		file >> temp_req.floor;
		if (temp_req.floor <= 0 || temp_req.floor > office_building.floors)
			throw logic_error("Invalid input! The floor should be in the interval [1, F]");

		file >> temp_req.time;
		if (temp_req.time < 0)
			throw logic_error("Invalid input! Time cannot be a negative number!");

		all_requests.push_back(temp_req);
	}

	elevator1.size = office_building.elevator_size1;
	elevator2.size = office_building.elevator_size2;
}

void ElevatorSimulation::Set_Order(const Requests& req, list<Requests>& accepted_requests, Elevator& elevator)
{
	list<Requests>::iterator it = accepted_requests.begin();
	bool inserted = false;
	if (elevator.next_dir == "up" || elevator.next_dir == "UP")
	{
		elevator.dir = elevator.next_dir;
		for (it; it != accepted_requests.end(); ++it)
		{
			if ((*it).floor > req.floor)
			{
				accepted_requests.insert(it, req);
				inserted = true;
				break;
			}
		}
		if (!inserted)
		{
			accepted_requests.push_back(req);
			elevator.direction_floor = req.floor;
		}
	}
	else if (elevator.next_dir == "down" || elevator.next_dir == "DOWN")
	{
		elevator.dir = elevator.next_dir;
		for (it; it != accepted_requests.end(); ++it)
		{
			if ((*it).floor < req.floor)
			{
				accepted_requests.insert(it, req);
				inserted = true;
				break;
			}
		}
		if (!inserted)
		{
			accepted_requests.push_back(req);
			elevator.direction_floor = req.floor;
		}
	}
	else
	{
		accepted_requests.push_front(req);
		elevator.dir = req.dir;
		elevator.next_dir = elevator.dir;
		elevator.direction_floor = req.floor;
	}

}

void ElevatorSimulation::Set_direction(const Requests& req, const Requests& next_req, Elevator& elevator)
{
	if (!elevator.isMoving())
	{
		if (req.floor - elevator.curr_floor > 0)
			elevator.dir = "UP";
		else
			elevator.dir = "DOWN";

		if (req.floor > next_req.floor)
			elevator.next_dir = "DOWN";
		else
			elevator.next_dir = "UP";

		elevator.direction_floor = req.floor;
	}
	else if ((req.dir == "UP" && req.floor > elevator.direction_floor) ||
		(req.dir == "DOWN" && req.floor < elevator.direction_floor))
		elevator.direction_floor = req.floor;
}

int ElevatorSimulation::Send_Elevator(const Requests& req)
{
	if (req.floor == elevator1.curr_floor && (!elevator1.isMoving() || (elevator1.movement % 5 == 0 && elevator1.next_dir == req.dir && elevator1.dir == req.dir)))
		return 1;
	if (req.floor == elevator2.curr_floor && (!elevator2.isMoving() || (elevator2.movement % 5 == 0 && elevator2.next_dir == req.dir && elevator2.dir == req.dir)))
		return 2;

	bool elevator1_available = false;
	bool elevator2_available = false;

	if (!elevator1.isMoving())
		elevator1_available = true;

	else if (elevator1.next_dir == req.dir && elevator1.dir == req.dir &&
		(((req.dir == "up" || req.dir == "UP") && elevator1.curr_floor < req.floor) ||
			((req.dir == "down" || req.dir == "DOWN") && elevator1.curr_floor > req.floor)))
		elevator1_available = true;
	else if (elevator1.next_dir == req.dir && elevator1.direction_floor == req.floor &&
		(((elevator1.dir == "up" || elevator1.dir == "UP") && elevator1.curr_floor < req.floor) ||
			((elevator1.dir == "down" || elevator1.dir == "DOWN") && elevator1.curr_floor > req.floor)))
		elevator1_available = true;

	if (!elevator2.isMoving())
		elevator2_available = true;

	else if (elevator2.next_dir == req.dir && elevator2.dir == req.dir &&
		(((req.dir == "up" || req.dir == "UP") && elevator2.curr_floor < req.floor) ||
			((req.dir == "down" || req.dir == "DOWN") && elevator2.curr_floor > req.floor)))
		elevator2_available = true;
	else if (elevator2.next_dir == req.dir && elevator2.direction_floor == req.floor &&
		(((elevator2.dir == "up" || elevator2.dir == "UP") && elevator2.curr_floor < req.floor) ||
			((elevator2.dir == "down" || elevator2.dir == "DOWN") && elevator2.curr_floor > req.floor)))
		elevator2_available = true;

	if (elevator1_available && !elevator2_available)
		return 1;
	else if (!elevator1_available && elevator2_available)
		return 2;
	else if (elevator1_available && elevator2_available)
	{
		if (abs(elevator1.curr_floor - req.floor) <= abs(elevator2.curr_floor - req.floor))
			return 1;
		else
			return 2;
	}

	return 0;
}

void ElevatorSimulation::Accept_requests()
{
	list<Requests>::iterator it = all_requests.begin();
	list<Requests>::iterator temp;
	int elev = 0;
	
	while (!all_requests.empty() && it != all_requests.end() && (*it).time <= curr_time)
	{
		if (!(*it).isElevatorSent)
		{
			elev = Send_Elevator((*it));
			if (elev == 1)
			{
				(*it).isElevatorSent = true;
				temp = it;
				++temp;
				if ((*it).floor != elevator1.curr_floor)
					Set_direction((*it), (*temp), elevator1);			
				else 
				{
					elevator1.dir = (*it).dir;
					elevator1.next_dir = elevator1.dir;
					if (elevator1.dir == "UP" && elevator1.direction_floor < (*temp).floor)
						elevator1.direction_floor = (*temp).floor;
					else if (elevator1.dir == "DOWN" && elevator1.direction_floor > (*temp).floor)
						elevator1.direction_floor = (*temp).floor;
				}
			}
			else if (elev == 2)
			{
				(*it).isElevatorSent = true;
					temp = it;
					++temp;
				if ((*it).floor != elevator2.curr_floor)				
					Set_direction((*it), (*temp), elevator2);				
				else
				{
					elevator2.dir = (*it).dir;
					elevator2.next_dir = elevator2.dir;
					if (elevator2.dir == "UP" && elevator2.direction_floor < (*temp).floor)
						elevator2.direction_floor = (*temp).floor;
					else if (elevator2.dir == "DOWN" && elevator2.direction_floor > (*temp).floor)
						elevator2.direction_floor = (*temp).floor;
				}
			}
		}

		++it;
		++it;
	}
}

bool ElevatorSimulation::GetIn_elevator(Elevator& elevator, list<Requests>& accepted_req)
{
	list<Requests>::iterator it = all_requests.begin();

	bool getin = false; // Ïðîâåðÿâà äàëè ñå å êà÷èë íÿêîé íà òîçè åòàæ
	while (it != all_requests.end() && curr_time >= (*it).time)
	{
		if (elevator.curr_floor == (*it).floor)
		{
			if (elevator.isFull() && elevator.next_dir == (*it).dir)
			{
				(*it).isElevatorSent = false;
				++it;
				++it;
				getin = true;
			}
			else if (elevator.next_dir != elevator.dir && elevator.next_dir == (*it).dir && elevator.direction_floor != (*it).floor)
			{
				++it;
				++it;
			}
			else if (!elevator.isFull() && (!elevator.isMoving() || elevator.next_dir == (*it).dir))
			{
				it = all_requests.erase(it);
				Set_Order((*it), accepted_req, elevator);
				it = all_requests.erase(it);
				++elevator.people;
				getin = true;
			}
			else
			{
				++it;
				++it;
			}
		}
		else
		{
			++it;
			++it;
		}
	}

	if (getin)
		return true;

	return false;
}

bool ElevatorSimulation::GoOff_from_elevator(Elevator& elevator, list<Requests>& accepted_req)
{
	bool off = false; // Àêî å èñòèíà ùå èçïðèíòè, ÷å àñàíñüîðúò å ñïðÿë íà òîçè åòàæ
	while (!accepted_req.empty() && elevator.curr_floor == accepted_req.front().floor)
	{
		--elevator.people;
		accepted_req.pop_front();
		off = true;
	}

	if (off)
		return true;

	return false;
}

void ElevatorSimulation::Move_Elevator(Elevator& elevator, list<Requests>& accepted_req, char c)
{
	++elevator.movement;

	if (elevator.movement % 5 == 0)
	{
		++curr_time;
		if (elevator.dir == "up" || elevator.dir == "UP")
			++elevator.curr_floor;
		else
			--elevator.curr_floor;

		bool printed_on_screen = false;
		if (!accepted_req.empty())
		{
			if (GoOff_from_elevator(elevator, accepted_req))
			{
				cout << c << " " << curr_time << " " << elevator.curr_floor << " " << elevator.dir << endl;
				printed_on_screen = true;
			}
		}

		string temp_dir;
		if (!printed_on_screen)
			temp_dir = elevator.dir;

		if (GetIn_elevator(elevator, accepted_req))
		{
			if (!printed_on_screen)
				cout << c << " " << curr_time << " " << elevator.curr_floor << " " << temp_dir << endl;
		}
		else if (elevator.direction_floor == elevator.curr_floor)
		{
			elevator.dir = "";
			elevator.next_dir = "";
			elevator.direction_floor = 0;
			if (!printed_on_screen)
				cout << c << " " << curr_time << " " << elevator.curr_floor << " " << temp_dir << "  Empty turn! " << endl;
		}
		
		--curr_time;
	}
}

bool ElevatorSimulation::Time_Optimization()
{
	if (!elevator1.isMoving() && !elevator2.isMoving() && !all_requests.empty() && curr_time < all_requests.front().time)
	{
		curr_time = all_requests.front().time; // Skips time of inactivity
		return true;
	}
	else if (elevator1.isMoving() && elevator2.isMoving()) // Skips up to 4 checking steps
	{
		while (((elevator1.movement + 1) % 5 != 0 && (elevator2.movement + 1) % 5 != 0))
		{
			++elevator1.movement;
			++elevator2.movement;
			++curr_time;
		}
		++curr_time;
		return true;
	}

	return false;
}

void ElevatorSimulation::Run()
{
	while (!accepted_req_by1.empty() || !accepted_req_by2.empty() || !all_requests.empty())
	{
		Accept_requests();

		if (elevator1.movement % 5 == 0)
			GetIn_elevator(elevator1, accepted_req_by1);

		if (elevator1.isMoving())
			Move_Elevator(elevator1, accepted_req_by1, 'L');

		if (elevator2.movement % 5 == 0)
			GetIn_elevator(elevator2, accepted_req_by2);

		if (elevator2.isMoving())
			Move_Elevator(elevator2, accepted_req_by2, 'S');


		if (!Time_Optimization())
			++curr_time;

	}
}
