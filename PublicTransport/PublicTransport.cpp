#include "PublicTransport.h"

string PublicTransport::file_name = "Map.txt";

PublicTransport::PublicTransport()
{
	bs_count = 0;
	lines_count = 0;
}

PublicTransport::PublicTransport(ifstream& file)
{
	load_data(file);
}

int PublicTransport::find_bus_stop(string name) const
{
	return bs_index.find(name) != bs_index.end() ? bs_index.find(name)->second : -1;
}

bool PublicTransport::set_busses_times_middle_stops(pair<int, TransportLine>& p_tl, list<int>::iterator& start_stop_it, bool cyclic)
{
	list<int>::iterator end = prev(p_tl.second.bus_stops.end());
	if (cyclic) // If applying the method for a cyclic line the setting should end 1 bus stop earlier than for a ordinary line
		end = prev(end);

	for (list<int>::iterator stops_it = start_stop_it; stops_it != end; ++stops_it)
	{
		list<Bus>::iterator this_stop_bus = find_busses(p_tl.first, *stops_it); // First buss at this stop on this line
		list<Bus>::iterator next_stop_bus = find_busses(p_tl.first, *next(stops_it)); // First buss at the next stop on this line
		list<Bus>::iterator invalid1 = find_busses_end(p_tl.first, *stops_it); // nullptr for this_stop_bus
		list<Bus>::iterator invalid2 = find_busses_end(p_tl.first, *next(stops_it)); // nullptr for next_stop_bus
		
		int next_bs_num = *next(stops_it);
		while (this_stop_bus != invalid1)
		{
			if (next_stop_bus == invalid2)
				return false;

			if (this_stop_bus->destination != next_bs_num)
				return false;

			if (this_stop_bus->time_begin >= next_stop_bus->time_begin)
				return false;

			this_stop_bus->time_end = next_stop_bus->time_begin;

			++this_stop_bus;
			++next_stop_bus;
		}

		// Both bus stops must have equal number of busses from any line going through them
		if (next_stop_bus != invalid2)
			return false;
	}

	// Check the last stop in a non-cyclic line
	if (!cyclic)
	{
		int last_bs = *prev(p_tl.second.bus_stops.end());
		const list<Bus>& bus_list = bus_stops.at(last_bs).busses.find(p_tl.first)->second;
		for (const Bus& b : bus_list)
		{
			if (b.destination != -1)
				return false;
		}
	}

	return true;
}

bool PublicTransport::set_busses_times_CyclicLine(pair<int, TransportLine>& p_tl)
{
	list<int>::iterator stops_it = p_tl.second.bus_stops.begin(); // The first bus stop in the line
	list<Bus>::iterator this_stop_bus = find_busses(p_tl.first, *stops_it); // Busses from the first bus stop
	list<Bus>::iterator invalid1 = find_busses_end(p_tl.first, *stops_it ); // nullptr for this_stop_bus

	++stops_it;
	list<Bus>::iterator next_stop_bus = find_busses(p_tl.first, *stops_it); // Busses from the next bus stop
	list<Bus>::iterator invalid2 = find_busses_end(p_tl.first, *stops_it); // nullptr for next_stop_bus

	// Check if the connection between the first 2 stops is valid and set the time when a buss arrives at it's destination from the current stop 
	int startCount = 0;
	int finishCount = 0;
	while (this_stop_bus != invalid1)
	{
		if (next_stop_bus == invalid2)
			break;

		if (this_stop_bus->destination == -1) // This is the end stop on the current bus
		{
			++finishCount;
			if (finishCount > startCount)
				return false;

			++this_stop_bus;
			continue;
		}

		if (this_stop_bus->destination != *stops_it)
			return false;

		if (this_stop_bus->time_begin >= next_stop_bus->time_begin)
			return false;

		this_stop_bus->time_end = next_stop_bus->time_begin;

		++startCount;
		++this_stop_bus;
		++next_stop_bus;
	}

	if (next_stop_bus != invalid2) // The first bus stop in a cyclic line has 2 times more busses from that line compared to the other stops
		return false;

	// All not already checked busses from the first stop must end their course at that bus stop
	while (this_stop_bus != invalid1)
	{
		if (this_stop_bus->destination != -1)
			return false;

		++finishCount;
		++this_stop_bus;
	}

	// The count of the busses going out from the first stop must be equal to the busses going in 
	if (startCount != finishCount)
		return false;

	// Apply the method for non-cyclic lines for the other bus stops (except the last 2)
	if (!set_busses_times_middle_stops(p_tl, stops_it, true))
		return false;

	// Check the connection between the last 2 bus stops
	stops_it = prev(prev(p_tl.second.bus_stops.end()));
	this_stop_bus = find_busses(p_tl.first, *stops_it); // The pre-last bus stop in the line
	next_stop_bus = find_busses(p_tl.first, *p_tl.second.bus_stops.begin()); // First and last bus stop in the line
	invalid2 = find_busses_end(p_tl.first, *stops_it);
	int first_stop_num = *p_tl.second.bus_stops.begin();

	while (this_stop_bus != invalid2)
	{
		while (next_stop_bus->destination != -1)
			++next_stop_bus;

		if (this_stop_bus->destination != first_stop_num)
			return false;

		if (this_stop_bus->time_begin >= next_stop_bus->time_begin)
			return false;

		this_stop_bus->time_end = next_stop_bus->time_begin;
		++this_stop_bus;
		++next_stop_bus;
	}

	// Other validations for the last 2 bus stops are not needed, because if there is any missmatch between them, it would be caught earier

	return true;
}

bool PublicTransport::set_busses_times()
{
	for (pair<int, TransportLine> p_tl : lines)
	{
		if (*p_tl.second.bus_stops.begin() == *prev(p_tl.second.bus_stops.end())) // Check if the line is cyclic
		{
			if (!set_busses_times_CyclicLine(p_tl))
				return false;
		}
		else
		{
			list<int>::iterator start_stop_it = p_tl.second.bus_stops.begin();
			if (!set_busses_times_middle_stops(p_tl, start_stop_it, false)) // The method sets and validates all bus stops in a non-cyclic line
				return false;
		}
	}

	return true;
}

list<Bus>::iterator PublicTransport::find_busses(int line, int bs)
{	
	return bus_stops.at(bs).busses.find(line)->second.begin();;
}

list<Bus>::iterator PublicTransport::find_busses_end(int line, int bs)
{
	return bus_stops.at(bs).busses.find(line)->second.end();;
}

bool PublicTransport::add_bus_stops_to_line(int stops_count, vector<string>& bs_names)
{
	cout << 1 << ". ";
	cin >> bs_names[0];

	for (int i = 1; i < stops_count; ++i)
	{
		cout << i + 1 << ". ";
		cin >> bs_names[i];
		for (int j = 0; j < i; j++)
		{
			if (bs_names[i] == bs_names[j])
			{
				if (i == stops_count - 1 && j == 0)
					continue;
				cout << "Invalid line! The line cannot contain inner cycles!" << endl;
				return false;
			}
		}
	}

	return true;
}

void PublicTransport::create_first_course(int newline, const vector<string>& bs_names)
{
	list<Bus> bs_list;
	int index1 = find_bus_stop(bs_names[0]);
	lines.find(newline)->second.bus_stops.push_back(index1);
	bus_stops.at(index1).busses.insert(make_pair(newline, bs_list));

	int index2 = find_bus_stop(bs_names[1]);
	lines.find(newline)->second.bus_stops.push_back(index2);
	bus_stops.at(index2).busses.insert(make_pair(newline, bs_list));

	cout << "\nCreating bus line " << newline << endl << endl;
	int prev_stop_time1, time1, time2, bs_count = bs_names.size();

	cout << "Creating relation " << bs_names[0] << " --> " << bs_names[1] << endl;
	cout << "The bus arrives at " << bs_names[0] << " at:  ";
	cin >> time1;
	while (time1 < 0 || time1 > INF / 2 - 1)
	{
		cout << "The bus cannot be at bus stop " << bs_names[0] << " earlier than 0" << "' and later than " << INF / 2 - 1 << " !\nEnter again... ";
		cin >> time1;
	}
	prev_stop_time1 = time1;
	bus_stops.at(index1).busses.find(newline)->second.push_back(Bus(newline, index2, time1));
	bus_stops.at(index1).busses_count++;

	cout << "The bus traveling from " << bs_names[0] << " at " << time1 << "' arrives at bus stop " << bs_names[1] << " at:  ";
	cin >> time2;
	while (time2 <= prev_stop_time1 || time2 > INF - bs_count + 1)
	{
		cout << "The bus cannot be at bus stop " << bs_names[1] << " earlier than " << prev_stop_time1 + 1 << "' and later than " << INF - bs_count +1 << "' !\nEnter again... ";
		cin >> time2;
	}

	bus_stops.at(index1).busses.at(newline).front().time_end = time2;
	int nextStop = bs_names[1] == bs_names.back() ? -1 : find_bus_stop(bs_names[2]);
	bus_stops.at(index2).busses.at(newline).push_back(Bus(newline, nextStop, time2));
	bus_stops.at(index2).busses_count++;
}

bool PublicTransport::update_shortest_time(unordered_map<int, Path>& dist, priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>>& pqueue, const Bus& bus, int current_stop) const
{
	int v = bus.destination;
	int time = bus.time_end;

	if (dist.at(v).time > time || (dist.at(v).time == time && dist.at(v).cntStops > dist.at(current_stop).cntStops + 1))
	{
		dist.at(v).time = time;
		dist.at(v).from = current_stop;
		dist.at(v).bus = bus.number;
		dist.at(v).cntStops = dist.at(current_stop).cntStops + 1;
		pqueue.push(make_pair(dist.at(v).time, v));
		return true;
	}

	return false;
}

int** PublicTransport::create_adj_matrix()
{
	int** matrix = new (nothrow) int* [bs_count];
	if (!matrix)
	{
		delete[] matrix;
		cout << "Not enough memory to perform the action!\n";
		return nullptr;
	}
	for (int i = 0; i < bs_count; ++i)
	{
		matrix[i] = new (nothrow) int[bs_count](); // Initial value 0
		if (!matrix[i])
		{
			for (int j = 0; j < i; ++j)
				delete[] matrix[j];

			delete[] matrix;
			cout << "Not enough memory to perform the action!\n";
			return nullptr;
		}
	}

	return matrix;
}

void PublicTransport::print_path(int bs1, int bs2, const unordered_map<int, Path>& dist) const
{
	stack<Path> full_path; // Where it is coming from, the time when it arrives and the bus number
	stack<int> bus_stop;  // Current stop
	while (bs1 != dist.at(bs2).from)
	{
		const Path& p = dist.at(bs2);
		full_path.push(Path(p.time, p.from, p.bus, p.cntStops));
		bus_stop.push(bs2);
		bs2 = p.from;
	}
	const Path& p = dist.at(bs2);
	full_path.push(Path(p.time, p.from, p.bus, p.cntStops));
	bus_stop.push(bs2);

	while (!full_path.empty())
	{
		cout << "Travel to bus stop " << bus_stops.at(bus_stop.top()).name << " with bus: " << full_path.top().bus << ". You will arrive at: " << full_path.top().time << endl;
		full_path.pop();
		bus_stop.pop();
	}
}

void PublicTransport::find_fastest_path() const
{	
	string name1, name2;
	int start_time, bs1, bs2;

	cout << "Enter name of a bus stop:\nStart --> ";
	cin >> name1;

	bs1 = find_bus_stop(name1);
	if (bs1 == -1)
	{
		cout << "The bus stop doesn't exist!" << endl;
		return;
	}
	cout << "End --> ";
	cin >> name2;

	bs2 = find_bus_stop(name2);
	if (bs2 == -1)
	{
		cout << "The bus stop doesn't exist!" << endl;
		return;
	}

	if (bs1 == bs2)
	{
		cout << "You are already there!" << endl;
		return;
	}

	cout << "Time begin --> ";
	cin >> start_time;
	if (start_time < 0 || start_time >= INF / 2)
	{
		cout << "Invalid time!" << endl;
		return;
	}

	unordered_map<int, Path> dist(bs_count);
	for (const pair<int, BusStop>& p_bs : bus_stops)
		dist.insert(make_pair(p_bs.first, Path()));

	// pair<int, int> -> first = time for reaching a vertex; second = the vertex
	priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pqueue;

	dist.at(bs1).time = start_time;
	dist.at(bs1).from = bs1;
	dist.at(bs1).visited = true;

	const BusStop& busStop = bus_stops.find(bs1)->second;
	for (const pair<int, list<Bus>>& p_courses : busStop.busses)
	{
		for (const Bus& b : p_courses.second)
		{
			if (b.destination == -1)
				continue;

			if (b.time_begin >= start_time)
				if (update_shortest_time(dist, pqueue, b, bs1)) // As the list is sorted, if the time is updated with a buss from 1 line, the next busses from this line cannot update it again
					break;
		}
	}

	while (!pqueue.empty())
	{
		int u = pqueue.top().second;
		pqueue.pop();

		if (dist.at(u).visited) 
			continue;

		dist.at(u).visited = true;
		const BusStop& busStop = bus_stops.find(u)->second;
		for (const pair<int, list<Bus>>& p_courses : busStop.busses)
		{
			for (const Bus& b : p_courses.second)
			{
				if (b.destination == -1)
					continue;

				if (b.time_begin >= dist.at(u).time)
					if (update_shortest_time(dist, pqueue, b, u))
						break;
			}
		}
	}

	if (dist.at(bs2).time == INF)
		cout << "\nThe bus stop cannot be reached!" << endl;
	else
	{
		cout << "\nStart at stop: " << bus_stops.find(bs1)->second.name << ",  at time: " << start_time << endl;
		print_path(bs1, bs2, dist);
	}
}

void PublicTransport::print_bus_stops_schedule() const
{
	if (bs_count < 1)
	{
		cout << "There aren't any bus stops on the map!" << endl;
		return;
	}

	cout << "Schedule:" << endl;
	for (const pair<int, BusStop>& p_busStop : bus_stops)
	{
		int number = p_busStop.first;
		const BusStop& busStop = p_busStop.second;

		cout << "\nBus stop \" " << busStop.name << " \" (" << number << "): " << endl;
		if (busStop.busses_count == 0)
			cout << "There aren't any lines going through this bus stop!" << endl;
		else
		{
			for (const pair<int, list<Bus>>& p : busStop.busses)
			{
				for (const Bus& b : p.second)
				{
					cout << "Bus " << b.number << " arrives at " << b.time_begin;
					if (b.destination == -1)
						cout << " (last stop)";
					cout << endl;
				}
			}
		}
	}

	cout << endl;
}

void PublicTransport::print_courses() const
{
	if (lines_count < 1)
	{
		cout << "No any public transport lines have been found!" << endl;
		return;
	}

	cout << "Do you want to see all courses or only for a selected line?" << endl;
	cout << "Enter 1 to see all courses or enter 2 to select a line\n--> ";
	string str;
	int option = -1;
	do {
		cin >> str;
		try {
			option = stoi(str, nullptr, 10);
		}
		catch (...) {
			option = -1;
		}

		switch (option)
		{
			case 1:
			{
				for (const pair<int, BusStop>& p_busStop : bus_stops)
				{
					int number = p_busStop.first;
					const BusStop& busStop = p_busStop.second;

					cout << "\nBus stop \" " << busStop.name << " \" (" << number << "): " << endl;
					for (const pair<int, list<Bus>>& p_courses : busStop.busses)
					{
						for (const Bus& b : p_courses.second)
						{
							if (b.destination != -1)
								cout << "Bus " << b.number << " arrives at " << b.time_begin << "' and travels to bus stop \" " << bus_stops.find(b.destination)->second.name << " \" for " << b.time_end - b.time_begin << " minutes." << endl;
						}
					}
				}
				break;
			}

			case 2:
			{
				int line;
				cout << "Enter line: ";
				cin >> str;
				try {
					line = stoi(str, nullptr, 10);
				}
				catch (...) {
					cout << "Invalid line!" << endl;
					return;
				}

				if (lines.find(line) == lines.end())
				{
					cout << "The line doesn't exist!" << endl;
					return;
				}

				for (const pair<int, BusStop>& p_busStop : bus_stops)
				{
					int number = p_busStop.first;
					const BusStop& busStop = p_busStop.second;

					if (busStop.busses.find(line) != busStop.busses.end())
					{
						list<Bus> l_courses = busStop.busses.find(line)->second;
						for (const Bus& b : l_courses)
						{
							if (b.destination != -1)
								cout << "Bus " << line << " travels from bus stop \" " << busStop.name << " \" at " << b.time_begin << " to bus stop \" " << bus_stops.find(b.destination)->second.name << " \" for " << b.time_end - b.time_begin << " minutes." << endl;
						}
					}
				}
				break;
			}
			default:
			{
				cout << "\nInvalid option! Enter again!\n--> ";
				option = -1;
				break;
			}
		}
	} while (option == -1);

	cout << endl;
}

void PublicTransport::print_line() const
{
	if (lines_count < 1)
	{
		cout << "No any public transport lines have been found!" << endl;
		return;
	}

	cout << "Do you want to see all lines or just a selected line?" << endl;
	cout << "Enter 1 to see all lines or enter 2 to select a line\n--> ";
	string str;
	int option = -1;
	do {
			
		cin >> str;
		try {
			option = stoi(str, nullptr, 10);
		}
		catch (...) {
			option = -1;
		}
		switch (option)
		{
			case 1:
			{
				for (const pair<int, TransportLine>& p : lines) 
				{
					cout << "\nTransport line " << p.first << ":" << endl;
					for (list<int>::const_iterator bs = p.second.bus_stops.begin(); bs != prev(p.second.bus_stops.end()); ++bs)
					{
						cout << bus_stops.at(*bs).name << " --> ";
					}
					cout << bus_stops.at(*prev(p.second.bus_stops.end())).name << endl;
				}
				break;
			}
			case 2:
			{
				int line = -1;
				cout << "Enter line: ";
				cin >> str;
				try
				{
					line = stoi(str, nullptr, 10);
				}
				catch (...)
				{
					cout << "Invalid line!" << endl;
					return;
				}

				if (lines.find(line) == lines.end())
				{
					cout << "The line doesn't exist!" << endl << endl;
					return;
				}

				const TransportLine& tl = lines.find(line)->second;

				cout << "\nTransport line " << line << ":" << endl;
				for (list<int>::const_iterator bs = tl.bus_stops.begin(); bs != prev(tl.bus_stops.end()); ++bs)
				{
					cout << bus_stops.at(*bs).name << " --> ";
				}
				cout << bus_stops.at(*prev(tl.bus_stops.end())).name << endl;

				break;
			}
			default:
			{
				cout << "\nInvalid option! Enter again!\n--> ";
				option = -1;
				break;
			}
		}
	} while (option == -1);

	cout << endl;
}

void PublicTransport::type_to_continue() const
{	
	string str;
	cout << "\nType anything to go back to the main menu... ";
	cin >> str;
	cout << endl;
}

void PublicTransport::users_option(int option)
{		
	switch (option)
	{
		case 1:
		{
			cout << "\n\t\tFinding the fastest path\n";
			if (bs_count < 1)
			{
				cout << "Not enough bus stops / lines added to the map! Please add more before doing this action!" << endl;
				type_to_continue();
				break;
			}
			else
				find_fastest_path();

			type_to_continue();
			break;
		}
		case 2:
		{
			cout << "\n\t\tAdd a new line to the map\n";
			if (add_new_line())
				cout << "\nA new line has been successfully added!" << endl;
			else
				cout << "Creating a new line has failed! No changes have been made!" << endl;

			type_to_continue();
			break;
		}
		case 3:
		{
			int line;
			string str;
			cout << "\n\t\tAdd a new course\n";
			cout << "Enter the line where you want to add a new course --> ";
			cin >> str;
			try {
				line = stoi(str);
			}
			catch (...)
			{
				cout << "Invalid line!" << endl;
				type_to_continue();
				break;
			}
			if (add_new_course(line))
				cout << "\nA new course for line " << line << " has been added!" << endl;
			else
				cout << "No changes have been made!" << endl;

			type_to_continue();
			break;
		}
		case 4:
		{
			int line;
			string name1, name2, str;
			cout << "\n\t\tExtend an existing line\n";
			cout << "Enter line --> ";
			cin >> str;
			try {
				line = stoi(str);
			}
			catch (...)
			{
				cout << "Invalid line!" << endl;
				type_to_continue();
				break;
			}
			if (lines.find(line) == lines.end())
			{
				cout << "The line doesn't exist!" << endl;
				type_to_continue();
				break;
			}

			cout << "Enter a bus stop --> ";
			cin >> name1;
			cout << "Enter a bus stop --> ";
			cin >> name2;

			if (extend_line(line, name1, name2))
				cout << "\nLine " << line << " has been successfully extended!" << endl;
			else
				cout << "The line has not been extended! No any changes have been made!" << endl;

			type_to_continue();
			break;
		}
		case 5:
		{
			string s;
			int line;
			cout << "\n\t\tRemove a line from the map" << endl;
			cout << "Enter the line which you want to remove /invalid symbol to go back/ --> ";
			cin >> s;
			try {
				line = stoi(s, nullptr, 10);
			}
			catch (...)
			{
				cout << "No changes..." << endl << endl;
				break;
			}

			if (remove_line(line))
				cout << "\nLine " << line << " has been removed! Please check the bus stops schedule!" << endl;
			else
				cout << "No changes have been made!" << endl;

			type_to_continue();
			break;
		}
		case 6:
		{
			string s;
			int line;
			cout << "\n\t\tRemove a course from an existing line" << endl;
			cout << "Enter line /Enter invalid symbol to go back/ --> ";
			cin >> s;
			try {
				line = stoi(s, nullptr, 10);
			}
			catch (...)
			{
				cout << "No changes..." << endl << endl;
				break;
			}

			if (!remove_course(line))
				cout << "No changes have been made!" << endl;

			type_to_continue();
			break;
		}
		case 7:
		{
			cout << "\n\t\tRemove a bus stop from the line" << endl;
			remove_bus_stop();
			type_to_continue();
			break;
		}
		case 8:
		{
			cout << "\n\t\tCheck public transport lines" << endl;
			print_line();
			type_to_continue();
			break;
		}
		case 9:
		{
			cout << "\n\t\tCheck courses schedule" << endl;
			print_courses();
			type_to_continue();
			break;
		}
		case 10:
		{
			cout << "\n\t\tCheck bus stops schedule" << endl;
			print_bus_stops_schedule();
			type_to_continue();
			break;
		}
		case 11:
		{
			cout << "\n\t\tReduce expenses - analysis\n";
			
			int** matrix = create_adj_matrix();
			if (matrix)
			{
				try {
					reduce_transport_lines(matrix);
				}
				catch (exception& e) {
					cout << "Error! " << e.what() << "\n";

					for (int j = 0; j < bs_count; ++j)
						delete[] matrix[j];
					delete[] matrix;
				}
			}

			type_to_continue();
			break;
		}
		case 12:
		{
			string new_file;
			cout << "\n\t\tLoad data from other text file" << endl;
			cout << "Enter file's name --> ";
			cin >> new_file;
			ifstream nf(new_file);
			if (!nf.is_open())
			{
				cout << "The text file has not been opened!\nNo changes have been made!" << endl;
				type_to_continue();
				break;
			}
			clear();
			load_data(nf);
			nf.close();

			type_to_continue();
			break;
		}
		case 13:
		{
			string choice;
			cout << "\n\t\tSave data" << endl;
			cout << "Enter 1 if you want to save data in another text file or anything other to save it in the default text file\n/0 to go back/\nEnter --> ";
			cin >> choice;

			if (choice == "1")
			{
				string new_file;
				cout << "Enter file's name --> ";
				cin >> new_file;
				ofstream nf(new_file);
				if (!nf.is_open())
				{
					cout << "The file could not be opened! Data is not saved!" << endl;
					type_to_continue();
					break;
				}
				else
					save_data(nf);
				nf.close();

				type_to_continue();
				break;
			}
			else if (choice == "0")
				break;

			ofstream def_file(file_name);
			save_data(def_file);
			def_file.close();

			type_to_continue();
			break;
		}
		case 0:
		{
			cout << "\nExiting the program..." << endl;
			break;
		}
		default:
		{
			cout << "\nInvalid option! Try again!\n\n";
			break;
		}
	}

	cout << endl;
}

void PublicTransport::menu()
{
	int option;
	string input;
	do
	{
		cout << "\t\tMain menu\n\n";
		cout << "1.  Find the fastest path" << endl;
		cout << "2.  Add a new line to the map" << endl;
		cout << "3.  Add a new course to an existing line" << endl;
		cout << "4.  Extend an existing line" << endl;
		cout << "5.  Remove a line from the map" << endl;
		cout << "6.  Remove a course from an existing line" << endl;
		cout << "7.  Remove a bus stop from the map" << endl;
		cout << "8.  Check public transport lines" << endl;
		cout << "9.  Check courses schedule" << endl;
		cout << "10. Check bus stops schedule" << endl;
		cout << "11. Reduce transport expenses - analysis" << endl;
		cout << "12. Load data from other text file" << endl;
		cout << "13. Save data" << endl;
		cout << "0.  Exit" << endl << endl;
		cout << "Enter  -->  ";
		
		cin >> input;

		try {
			option = stoi(input);
		}
		catch (...) {
			option = -1;
		}

		users_option(option);

	} while (option != 0);
}

void PublicTransport::set_course_time(queue<int>& time_begin, int line, int bs1, int bs2, int count, list<Bus>::iterator& bus, const string& name1, const string& name2, bool new_start)
{
	int time, prev_bus_time;

	list<Bus>::iterator prev_bus_it;
	if (!new_start)
	{
		prev_bus_time = 0;
		prev_bus_it = find_busses(line, bs1);
	}
	else
		prev_bus_time = -1;

	// Sets the courses time between 2 bus stops
	for (int i = 0; i < count; ++i)
	{
		cout << endl << "Creating relation " << name1 << " --> " << name2 << endl;
		int prev_stop_time = time_begin.front();

		if (!new_start)
		{
			cout << "Bus " << line << ", travelling from " << name1 << " at: " << prev_stop_time << " - reaches " << name2 << " at: ";
			cin >> time;
			while (time <= max(prev_stop_time, prev_bus_time) || time > INF - 1)
			{
				cout << "Invalid time! The bus must arrive at that bus stop in the interval [" << max(prev_stop_time + 1, prev_bus_time + 1) <<", " << INF - 1 <<"]" << endl;
				cout << "Enter again: ";
				cin >> time;
			}

			const list<Bus>::iterator& null = bus_stops.at(bs2).busses.at(line).end();
			while (bus != null && bus->time_begin <= time) // 'bus' is not null only if the current extension makes the line a cycle
				++bus;

			bus = bus_stops.at(bs2).busses.at(line).insert(bus, Bus(line, -1, time));
			bus_stops.at(bs2).busses_count++;
			prev_bus_it->destination = bs2;
			prev_bus_it->time_end = time;
			++prev_bus_it;
		}
		else
		{
			// prev_stop_time in this case is the time when the buss arrives to the next stop
			cout << "The bus arrives at " << name2 << ", at " << prev_stop_time << ".\nEnter the time when the bus arrives at " << name1 << "... ";
			cin >> time;
			while (time >= prev_stop_time || time <= prev_bus_time)
			{				
				if (prev_bus_time < 0)
					cout << "Invalid time! The bus must arrive at that bus stop not later than " << prev_stop_time - 1 << endl;
				else
					cout << "Invalid time! The bus must arrive at that bus stop between " << min(prev_stop_time - 1, prev_bus_time + 1) << " and " << max(prev_stop_time - 1, prev_bus_time + 1) << endl;

				cout << "Enter again: ";
				cin >> time;
			}

			bus = bus_stops.at(bs1).busses.find(line)->second.insert(bus, Bus(line, bs2, time, prev_stop_time));
			bus_stops.at(bs1).busses_count++;
		}

		time_begin.pop();
		prev_bus_time = time;
		++bus;
	}
}

int PublicTransport::create_new_bs(const string& name, int line)
{	
	int num;
	cout << "Creating a new bus stop (" << name << "). Enter it's number: ";
	while (true)
	{
		cin >> num;
		if (num < 0) {
			cout << "The number must be non-negative! Enter again...\n>";
		}
		else if (bus_stops.find(num) != bus_stops.end()) {
			cout << "That number is already taken! Enter again...\n> ";
		}
		else {
			break;
		}
	} 

	++bs_count;
	bus_stops.insert(make_pair(num, BusStop()));
	bus_stops.at(num).name = name;
	bus_stops.at(num).busses_count = 0;
	bus_stops.at(num).number = num;
	bus_stops.at(num).busses.insert(make_pair(line, list<Bus>()));
	bs_index.insert(make_pair(name, num));

	cout << "\nNew bus stop with name \" " << name << " \" and number " << num << " has been created!" << endl;
	return num;
}

void PublicTransport::create_bs_set_time(queue<int>& time_begin, int line, int bs1, int bs2, int count, bool new_start, const string& name1, const string& name2)
{	
	if (new_start)
	{
		bs1 = create_new_bs(name1, line);	
		lines.find(line)->second.stops_count++;
		list<Bus>::iterator bus = find_busses(line, bs1);
		set_course_time(time_begin, line, bs1, bs2, count, bus, name1, name2, new_start);
		lines.find(line)->second.bus_stops.push_front(bs1);
	}
	else
	{
		bs2 = create_new_bs(name2, line);
		lines.find(line)->second.stops_count++;
		list<Bus>::iterator bus = find_busses(line, bs2);
		set_course_time(time_begin, line, bs1, bs2, count, bus, name1, name2, new_start);
		lines.find(line)->second.bus_stops.push_back(bs2);
	}
}

void PublicTransport::add_stop_to_line(queue<int>& time_begin, int line, int bs1, int bs2, int count, const string& name1, const string& name2, bool new_start)
{
	list<Bus>::iterator bus;
	if (!new_start)
	{
		bus = find_busses(line, bs2);
		set_course_time(time_begin, line, bs1, bs2, count, bus, name1, name2, false);
		lines.find(line)->second.bus_stops.push_back(bs2);
	}
	else
	{
		bus = find_busses(line, bs1);
		set_course_time(time_begin, line, bs1, bs2, count, bus, name1, name2, true);
		lines.find(line)->second.bus_stops.push_front(bs2);
	}

	lines.find(line)->second.stops_count++;
}

bool PublicTransport::extend_line(int line, const string& name1, const string& name2)
{
	int bs1 = find_bus_stop(name1);
	int bs2 = find_bus_stop(name2);
	const list<int>::iterator& first = lines.find(line)->second.bus_stops.begin(); // The first stop from the line
	const list<int>::iterator& last = prev(lines.find(line)->second.bus_stops.end()); // The last stop from the line

	if (*first == *last)
	{
		cout << "The bus line begins and ends at the same bus stop therefore the line cannot be extended!" << endl;
		return false;
	}

	if (bs1 == *last) // The new bus stop will be the new end of the line
	{
		// Check if the line contains the bus stop that should be added
		if (bs2 != -1) {
			for (list<int>::iterator it = next(first); it != last; ++it) 
			{
				if (*it == bs2)
					return false;
			}
		}

		list<Bus>& bus_list = bus_stops.at(bs1).busses.find(line)->second; // Get the busses from that line at the last stop
		if (bus_list.back().time_begin == INF - 1)
		{
			cout << "A course from this line ends at " << INF - 1 << ". Therefore the line cannot be extended!" << endl;
			return false;
		}

		int count = 0;
		queue<int> time_begin; // Get the time of all busses from the line that arrive at the last stop
		for (const Bus& bus : bus_list)
		{
			++count;
			time_begin.push(bus.time_begin);	
		}

		if (bs2 != -1)
		{
			if (bus_stops.at(bs2).busses.find(line) == bus_stops.at(bs2).busses.end()) // False only if the current operation is making the line cyclic
			{
				list<Bus> bs_list;
				bus_stops.at(bs2).busses.insert(make_pair(line, bs_list));
			}
			add_stop_to_line(time_begin, line, bs1, bs2, count, name1, name2, false);
		}
		else
			create_bs_set_time(time_begin, line, bs1, bs2, count, false, name1, name2);
	}
	else
	{
		if (bs2 != *first)
			return false;

		// Check if the line contains the bus stop that should be added
		for (list<int>::iterator it = first; it != last; ++it)
		{
			if (*it == bs1)
				return false;
		}

		list<Bus>& bus_list = bus_stops.at(bs2).busses.find(line)->second;  // Get the busses from that line at the first stop
		if (bus_list.front().time_begin == 0)
		{
			cout << "There is a course for this line beginning at " << name2 << " at 0. Therefore the connection " << name1 << " --> " << name2 << " cannot be created!" << endl;
			return false;
		}

		int count = 0;
		queue<int> time_begin; // Get all departures time from the first stop
		for (const Bus& bus : bus_list)
		{
			++count;
			time_begin.push(bus.time_begin);			
		}


		if (bs1 != -1)
		{
			if (bus_stops.at(bs1).busses.find(line) == bus_stops.at(bs1).busses.end())
			{
				list<Bus> bs_list;
				bus_stops.at(bs1).busses.insert(make_pair(line, bs_list));
			}
			add_stop_to_line(time_begin, line, bs1, bs2, count, name1, name2, true);
		}
		else
			create_bs_set_time(time_begin, line, bs1, bs2, count, true, name1, name2);			
	}

	return true;
}

void PublicTransport::set_course_nonSpecial(int time1, int& time2, int stopper, int index, int line, int bs, list<Bus>::iterator& bus, const list<int>::iterator& it)
{
	for (int i = 0; i < index; ++i)
		++bus;

	int prev_bus_time = 0;
	if (index != 0)
		prev_bus_time = prev(bus)->time_begin;
	
	bool correct;
	const list<Bus>::iterator& null = find_busses_end(line, bs);
	do
	{
		correct = true;
		cin >> time2;
		if (time2 > INF - stopper)
		{
			cout << "Invalid time! Exceeds the limit! Must be <= " << INF - stopper << endl;
			cout << "Enter again... ";
			correct = false;
		}
		else if (bus != null && bus->number == line)
		{
			if (time2 < max(time1 + 1, prev_bus_time) || time2 > bus->time_begin)
			{
				cout << "Invalid time! The bus can arrive at bus stop " << bus_stops.at(*it).name << " in interval [" << max(time1 + 1, prev_bus_time) << ", " << min(bus->time_begin, INF - stopper) << "]" << endl;
				cout << "Enter again... ";
				correct = false;
			}
		}
		else if (time2 < max(time1 + 1, prev_bus_time))
		{
			cout << "Invalid time! The bus can't arrive at bus stop " << bus_stops.at(*it).name << " earlier than " << max(time1 + 1, prev_bus_time) << endl;
			cout << "Enter again... ";
			correct = false;
		}
		cout << endl;
	} while (!correct);
}

int PublicTransport::set_course_special(int time1, int index, int line, list<Bus>::iterator& bus, const list<int>::iterator& it)
{
	int prev_bus_time = 0, end_stop2 = 0, last_end_time = 2 * INF, time2;
	bool correct = true;
	const list<Bus>::iterator& null = bus_stops.at(lines.at(line).bus_stops.front()).busses.find(line)->second.end();
	for (int i = 0; i < index + 1 && bus != null; ++i)
	{
		if (bus->destination == -1)
		{
			++end_stop2;
			prev_bus_time = bus->time_begin;
			--i;
		}
		++bus;
	}

	while (index > end_stop2)
	{
		if (bus->destination == -1)
		{
			++end_stop2;
			prev_bus_time = bus->time_begin;
		}
		++bus;
	}

	while (bus != null)
	{
		if (bus->destination == -1)
		{
			last_end_time = bus->time_begin;
			++end_stop2;
			break;
		}
		++bus;
	}

	do
	{
		cin >> time2;
		correct = true;
		if (last_end_time != 2 * INF && (time2 < max(time1 + 1, prev_bus_time) || time2 > last_end_time))
		{
			cout << "Invalid time! The bus can arrive at bus stop " << bus_stops.at(*it).name << " in interval [" << max(time1 + 1, prev_bus_time) << ", " << last_end_time << "]" << endl;
			cout << "Enter again... ";
			correct = false;
		}
		else if (time2 < max(time1 + 1, prev_bus_time))
		{
			cout << "Invalid time! The bus can arrive at bus stop " << bus_stops.at(*it).name << " not earlier than " << max(time1 + 1, prev_bus_time) << endl;
			cout << "Enter again... ";
			correct = false;
		}
	} while (!correct);

	while (time2 < prev(bus)->time_begin)
		--bus;

	return time2;
}

bool PublicTransport::add_new_course(int line)
{
	if (lines.find(line) == lines.end())
	{
		cout << "The line doesn't exist!" << endl;
		return false;
	}

	TransportLine& tl = lines.find(line)->second;
	list<int>::iterator it = tl.bus_stops.begin();
	list<Bus>::iterator bus;
	const list<Bus>::iterator& null = find_busses_end(line, *it);
	int time1, time2, index = 0, bs_count = tl.stops_count;

	cout << "Creating a new course for line " << line << endl;
	//cout << "\nEnter time for relation " << bus_stops[*it].name << " --> " << bus_stops[*next(it)].name << endl;
	cout << "\nBus " << line << " begins the course from bus stop " << bus_stops.at(*it).name << " at... ";

	int stopper = bs_count - 1;
	bool correct;
	do // Choose when the course begins
	{	
		correct = true;
		cin >> time1;
		if (time1 >= INF / 2)
		{
			cout << "The course cannot start later than " << INF / 2 - 1 << endl;
			cout << "Enter again... ";
			correct = false;
		}
		else
		{
			for (bus = find_busses(line, *it); bus != null; ++bus)
			{
				if (bus->time_begin == time1 && bus->destination != -1)
				{
					cout << "\nThere is already a course for the line beginning at that time! Please select another time!" << endl;
					cout << "Enter again... ";
					index = 0;
					correct = false;
					break;
				}

				if (bus->time_begin < time1 && bus->destination != -1)
					++index;

				else if (bus->time_begin < time1 && bus->destination == -1) {
					continue;
				}
				else
					break;			
			}
		}

	} while (!correct);

	list<Bus>::iterator prev_bus = bus_stops.at(*it).busses.find(line)->second.insert(bus, Bus(line, *next(it), time1));
	bus_stops.at(*it).busses_count++;
	++it;
	// Add a course for the middle stops
	const list<int>& bs_list = tl.bus_stops;
	for (it; it != prev(bs_list.end()); ++it)
	{
		bus = find_busses(line, *it);

		cout << "\nRelation " << bus_stops[*prev(it)].name << " --> " << bus_stops[*it].name << endl;
		cout << "The bus traveling from bus stop " << bus_stops.at(*prev(it)).name << " at " << time1 << "'. arrives at " << bus_stops.at(*it).name << " at ";

		set_course_nonSpecial(time1, time2, stopper, index, line, *it, bus, it);
		--stopper;

		prev_bus->time_end = time2;
		prev_bus = bus_stops.at(*it).busses.find(line)->second.insert(bus, Bus(line, *next(it), time2));
		bus_stops[*it].busses_count++;
		time1 = time2;
	}

	// Add to the last stop:
	bus = find_busses(line, *it);

	cout << "\nRelation " << bus_stops.at(*prev(it)).name << " --> " << bus_stops.at(*it).name << endl;
	cout << "The bus traveling from bus stop " << bus_stops.at(*prev(it)).name << " at " << time1 << "'. arrives at " << bus_stops.at(*it).name << " at ";

	if (*prev(bs_list.end()) == *bs_list.begin()) // Check if the line forms a cycle	
		time2 = set_course_special(time1, index, line, bus, it);
	else
		set_course_nonSpecial(time1, time2, 1, index, line, *it, bus, it);

	prev_bus->time_end = time2;
	bus_stops.at(*it).busses.find(line)->second.insert(bus, Bus(line, -1, time2));
	bus_stops.at(*it).busses_count++;

	return true;
}

bool PublicTransport::add_new_line()
{
	int stops_count, courses, newstops = 0, newline;
	string str;
	cout << "\nNew line number: ";
	cin >> str;
	try {
		newline = stoi(str);
	}
	catch (...)
	{
		cout << "Invalid line!" << endl;
		return false;
	}

	if (lines.find(newline) != lines.end())
	{
		cout << "The line already exists!" << endl;
		return false;
	}

	if (newline < 1)
	{
		cout << "The line number must be a positive number!" << endl;
		return false;
	}

	cout << "\nHow many bus stops do you want to add in the new bus line... ";
	cin >> str;
	try {
		stops_count = stoi(str);
	}
	catch (...)
	{
		cout << "Invalid symbol!" << endl;
		return false;
	}

	cout << "\nHow many courses do you want the line to contain... ";
	cin >> str;
	try {
		courses = stoi(str);
	}
	catch (...)
	{
		cout << "Invalid symbol!" << endl;
		return false;
	}

	if (stops_count < 2 || courses < 1)
	{
		cout << "\nInvalid input! You need atleast 2 bus stops and 1 course to create a line!" << endl;
		return false;
	}

	cout << "\nEnter " << stops_count << " bus stops names in the order that busses should move." << endl;
	cout << "*Help -> You can add bus stops that aren't in the map yet by just typing a name of a bus stop which doesn't exist" << endl;

	vector<string> bs_names(stops_count);
	if (!add_bus_stops_to_line(stops_count, bs_names))
		return false;
	
	if (stops_count == 2 && bs_names[0] == bs_names[1])
	{
		cout << "Invalid line!\n";
		return false;
	}

	for (int i = 0; i < stops_count; ++i)
	{
		if (find_bus_stop(bs_names[i]) == -1)
			create_new_bs(bs_names[i], newline);
	}

	lines.insert(make_pair(newline,TransportLine(newline, 2)));	

	create_first_course(newline, bs_names);

	if (stops_count > 2)
	{
		list<Bus> bs_list;
		for (int j = 1; j < stops_count - 1; ++j)
		{
			int i = find_bus_stop(bs_names[j + 1]);

			if (bus_stops.at(i).busses.find(newline) == bus_stops.at(i).busses.end())
				bus_stops.at(i).busses.insert(make_pair(newline, bs_list));

			extend_line(newline, bs_names[j], bs_names[j + 1]);
		}
	}
		
	cout << endl;

	++lines_count;
	for (int i = 1; i < courses; ++i)
		add_new_course(newline);

	return true;
}

void PublicTransport::redirect_courses(int bs, int line)
{
	TransportLine& tl = lines.find(line)->second;
	list<int>::iterator prev_bs = tl.bus_stops.begin();

	while (*next(prev_bs) != bs)
		++prev_bs;

	list<Bus>::iterator bus_prev_bs = bus_stops.at(*prev_bs).busses.at(line).begin(); // Iterator for the busses from the previous bus stop
	list<Bus>& bs_list = bus_stops.at(bs).busses.at(line); // The busses from the bus stop that is going to be removed
	int next_bs = bs_list.begin()->destination;

	// In line A -> B -> C connect A to C and delete B
	while (!bs_list.empty())
	{
		while (bus_prev_bs->destination == -1) // In case the line is cyclic and this is a bus from bus stop A, skip the busses that end there 
			++bus_prev_bs;

		bus_prev_bs->destination = next_bs;
		bus_prev_bs->time_end = bs_list.front().time_end;

		bus_stops.at(bs).busses_count--;
		bs_list.pop_front();
		++bus_prev_bs;
	}

	bus_stops.at(bs).busses.erase(line);
	tl.bus_stops.remove(bs);
	tl.stops_count--;

	cout << "Courses from line " << line << " have been redirected!" << endl;
}

void PublicTransport::remove_bus_stop()
{
	string name;
	cout << "Enter the name of the bus stop that you want to remove\n--> ";
	cin >> name;
	int bs = find_bus_stop(name);
	if (bs == -1)
	{
		cout << "The bus stop doesn't exist!" << endl;
		return;
	}

	delete_bs(bs);
	bs_count--;

	cout << "Bus stop " << bus_stops.at(bs).name << " has been removed!" << endl;
	bus_stops.erase(bs);
}

bool PublicTransport::remove_course(int line)
{
	if (lines.find(line) == lines.end())
	{
		cout << "The line doesn't exist!" << endl;
		return false;
	}

	int count = 0;
	int bs = *lines.find(line)->second.bus_stops.begin();
	list<Bus> l_courses = bus_stops.at(bs).busses.find(line)->second;
	string str;

	cout << "Courses from this line: " << endl;
	for (const Bus& c : l_courses)
	{
		if (c.destination == -1) // Skip the courses ending at this stop if the line is cyclic
			continue;

		++count;
		cout << count << ". Course beginning at " << c.time_begin << endl;		
	}

	if (count == 1)
	{
		cout << "\nThe line contains only 1 course. Removing it will delete the whole line.\nEnter 1 if you want to continue or anything other to go back" << endl;
		cout << "--> ";
		cin >> str;
		if (str == "1")
		{
			remove_line(line);
			cout << "Transport line " << line << " has been removed due to lack of courses in the line" << endl;
			return true;
		}

		return false;	

	} else if (count == 0) {
		cout << "There aren't any courses in this line!" << endl;
		return false;
	}

	int option = 0;
	if (count > 1)
	{
		cout << "\nWhich course do you want to remove?\n--> ";
		cin >> str;
		try {
			option = stoi(str);
		}
		catch (...) {
			cout << "Invalid course!" << endl;
			return false;
		}

		if (option < 1 || option > count)
		{
			cout << "Invalid course!" << endl;
			return false;
		}
	}

	delete_course(line, option);
	return true;
}

bool PublicTransport::remove_line(int line)
{
	if (lines.find(line) == lines.end())
	{
		cout << "The line doesn't exist!" << endl;
		return false;
	}

	TransportLine& tl = lines.find(line)->second;
	bool cycle = (*tl.bus_stops.begin() == *prev(tl.bus_stops.end()));
	bool deleted = false;
	for (const int& bs : tl.bus_stops)
	{		
		if (bs != tl.bus_stops.front() || !cycle || (cycle && !deleted))
		{
			bus_stops.at(bs).busses_count -= bus_stops.at(bs).busses.at(line).size();
			bus_stops.at(bs).busses.erase(line);
			deleted = true;			
		}
	}

	lines.erase(line);
	--lines_count;

	return true;
}

void PublicTransport::reduce_transport_lines(int** matrix) const
{
	int i = 0;
	unordered_map<int, int> matrixIndexes; // the key is a bus stop number, the value is an index in the matrix
	for (const pair<int, BusStop>& p_bs : bus_stops)
		matrixIndexes.insert(make_pair(p_bs.first, i++));
	
	i = 0;
	vector<pair<int, int>> lines_length(lines_count); // pair.first is the length of a line, pair.second is the number of the line
	for (const pair<int, TransportLine>& p_tl : lines)
	{
		lines_length[i++] = make_pair(p_tl.second.stops_count, p_tl.first);

		list<int>::const_iterator first_stop = p_tl.second.bus_stops.begin();
		list<int>::const_iterator last_stop = prev(p_tl.second.bus_stops.end());

		// Get the number of busses from this line
		int courses = bus_stops.at(*first_stop).busses.at(p_tl.first).size(); 

		if (*first_stop == *last_stop) // The first and last stop are equal in a cyclic line only 
			courses /= 2; // In cyclic lines the first stop has 2 times more busses than the other stops because the courses end here

		for (first_stop; first_stop != last_stop; ++first_stop)
			matrix[matrixIndexes.at(*first_stop)][matrixIndexes.at(*next(first_stop))] += courses;	
	}

	sort(lines_length.begin(), lines_length.end());

	list<int> canBeRemoved; // List of the numbers of the lines that can be removed
	for (i = 0; i < lines_count; ++i)
	{
		bool remove = true;
		const TransportLine& tl = lines.at(lines_length[i].second);
		list<int>::const_iterator first_stop = tl.bus_stops.begin();
		list<int>::const_iterator last_stop = prev(tl.bus_stops.end());

		int courses = bus_stops.at(*first_stop).busses.at(lines_length[i].second).size();
		if (*first_stop == *last_stop)
			courses /= 2;

		for (first_stop; first_stop != last_stop; ++first_stop)
		{
			// A line can be removed only if without it between each 2 bus stops (from that line) still exist atleast 2 courses
			if (matrix[matrixIndexes.at(*first_stop)][matrixIndexes.at(*next(first_stop))] - courses < 2) {
				remove = false;
				break;
			}

			matrix[matrixIndexes.at(*first_stop)][matrixIndexes.at(*next(first_stop))] -= courses;
		}

		if (remove)
			canBeRemoved.push_back(lines_length[i].second);
		else { // Recover the removed courses if the line cannot be removed
			for (list<int>::const_iterator it = tl.bus_stops.begin(); it != first_stop; ++it)
				matrix[matrixIndexes.at(*it)][matrixIndexes.at(*next(it))] += courses;
		}
	}

	for (int j = 0; j < bs_count; ++j)
		delete[] matrix[j];
	delete[] matrix;

	if (!canBeRemoved.empty())
	{
		cout << "Lines that can be removed: ";
		for (int i : canBeRemoved)
			cout << i << " ";

		cout << "\n";
	}
	else
		cout << "There aren't any lines that can be removed!\n";
}

void PublicTransport::load_data(ifstream& file)
{
	bs_count = -1;
	lines_count = -1;
	file >> bs_count >> lines_count;
	
	if (bs_count < 0 || lines_count < 0)
	{
		bs_count = 0;
		lines_count = 0;
		cout << "Reading Error! Default data has been loaded!" << endl;
		return;
	}

	bus_stops.reserve(bs_count);
	string name;
	Bus bus;
	TransportLine tl;	
	unordered_set<int> order;
	int num, first;

	for (int i = 0; i < lines_count; ++i)
	{
		tl.line_number = -1;
		tl.stops_count = -1;
		file >> tl.line_number >> tl.stops_count;
		if (tl.line_number < 1 || tl.stops_count < 2)
		{
			cout << "Reading Error! Default data has been loaded!" << endl;
			clear();
			return;
		}

		// Add the first stop to the line:
		num = -1;
		file >> num;
		if (num < 0)
		{
			cout << "Reading Error! Default data has been loaded!" << endl;
			clear();
			return;
		}
		first = num;
		order.insert(num);
		tl.bus_stops.push_back(num);

		// Add the rest stops to the line:
		for (int j = 1; j < tl.stops_count; ++j)
		{
			num = -1;
			file >> num;
			if (num < 0 || (order.find(num) != order.end() && j != tl.stops_count - 1) || (order.find(num) != order.end() && j == tl.stops_count - 1 && first != num))
			{
				cout << "Reading Error! Default data has been loaded!" << endl;
				clear();
				return;
			}
			tl.bus_stops.push_back(num);
			order.insert(num);
		}
		lines.insert(make_pair(tl.line_number, tl));
		tl.bus_stops.clear();
		order.clear();
	}

	//Bus stops information:
	int prevbus = 0, prevtime = 0;
	for (int i = 0; i < bs_count; ++i)
	{
		BusStop bs;
		bs.number = -1;
		file >> bs.number;
		if (file.eof())
			break;

		if (bs.number < 0)
		{
			cout << "Reading Error! Default data has been loaded!" << endl;
			clear();
			return;
		}

		bs.busses_count = -1;
		file >> bs.name >> bs.busses_count;
		if (bs.name.empty() || bs.busses_count < 0)
		{
			cout << "Reading Error! Default data has been loaded!" << endl;
			clear();
			return;
		}

		// Check if a bus stop with that name or number already exists
		if (bus_stops.find(bs.number) != bus_stops.end() || bus_stops.find(find_bus_stop(bs.name)) != bus_stops.end())
		{
			cout << "Reading Error! Default data has been loaded!" << endl;
			clear();
			return;
		}
		
		bs_index.insert(make_pair(bs.name, bs.number));

		prevbus = 0;
		prevtime = 0;
		for (int j = 0; j < bs.busses_count; ++j)
		{
			bus.number = -1;
			bus.time_begin = -1;
			file >> bus.number >> bus.time_begin >> bus.destination;
			if (bus.number < 1 || bus.time_begin < 0 || bus.time_begin > INF - 1 || bus.destination < -1)
			{
				cout << "Reading Error! Default data has been loaded!" << endl;
				clear();
				return;
			}
			if (bus.number == prevbus && bus.time_begin < prevtime)
			{
				cout << "Reading Error! Default data has been loaded!" << endl;
				clear();
				return;
			}

			if (bs.busses.find(bus.number) == bs.busses.end())
			{
				list<Bus> bs_list;
				bs_list.push_back(bus);
				bs.busses.insert(make_pair(bus.number, bs_list));
			}
			else
				bs.busses.find(bus.number)->second.push_back(bus);
		}

		bus_stops.insert(make_pair(bs.number, bs));
	}

	// Validate and set the end-time on each bus
	if (!set_busses_times())
	{
		cout << "Reading Error! Default data has been loaded!" << endl;
		clear();
		return;
	}

	cout << "Data has been successfully loaded!" << endl;
}

void PublicTransport::save_data(ofstream& file) const
{
	if (!file.is_open())
	{
		cout << "Unable to save data!" << endl;
		return;
	}

	file.clear();
	file << bs_count  << " " << lines_count << endl << endl;

	for (const pair<int, TransportLine>& p : lines)
	{
		const TransportLine& tl = p.second;
		file << tl.line_number << " " << tl.stops_count << endl;
		for (const int bs : tl.bus_stops)
			file << bs << " ";

		file << endl << endl;
	}

	for (const pair<int, BusStop>& p_bs : bus_stops)
	{	
		file << endl;
		const BusStop& bs = p_bs.second;
		if (bs.number != -1)
		{
			file << p_bs.first << endl;
			file << bs.name << " " << bs.busses_count  << endl;
			for (const pair<int, list<Bus>>& p : bs.busses)
			{
				for (const Bus& bus : p.second)
					file << bus.number << " " << bus.time_begin << " " << bus.destination << endl;
			}
		}
	}

	cout << "Data has been successfully saved!" << endl;
}

void PublicTransport::delete_bs(int bs)
{
	BusStop& toDel = bus_stops.at(bs);
	while (toDel.busses_count != 0)
	{
		int line = toDel.busses.begin()->second.begin()->number;
		TransportLine& tl = lines.find(line)->second;
		const list<int>::iterator& first_bs = tl.bus_stops.begin();
		const list<int>::iterator& last_bs = prev(tl.bus_stops.end());

		// Removing 1 bus stop from a line containing only 2 bus stops (or 3, but the first and last are the same) will delete the whole line
		if (tl.stops_count == 2 || (tl.stops_count == 3 && *first_bs == *last_bs))
		{
			remove_line(line);
			cout << "The whole line " << line << " has been removed because the bus stops remaining in the line were less than 1" << endl;
			continue;
		}

		if (*last_bs == bs) // The line ends on this bus stop 
		{
			if (bs == *first_bs) // If the bus stop is also the beginning of the transport line (True only in cyclic lines)
			{
				tl.bus_stops.pop_front();
				tl.stops_count--;
			}

			toDel.busses_count -= toDel.busses.at(line).size();
			toDel.busses.erase(line);
			tl.bus_stops.pop_back();
			tl.stops_count--;

			// Mark the current pre-last stop as last for the line
			int prev_bs = *prev(tl.bus_stops.end());
			list<Bus>& lBus_prev_bs = bus_stops.at(prev_bs).busses.at(line);
			for (Bus& b : lBus_prev_bs) {
				b.destination = -1; 
				b.time_end = -1;
			}
		} 
		else if (*first_bs == bs) // The line begins with that bus stop and the line is not cyclic
		{
			toDel.busses_count -= toDel.busses.at(line).size();
			toDel.busses.erase(line);

			tl.bus_stops.pop_front();
			--tl.stops_count;
		}
		else // The bus stop is a middle stop for the line (not first or last)
			redirect_courses(bs, line);	
	}

	toDel.busses.clear();
	toDel.number = -1;
	bs_index.erase(toDel.name);
}

void PublicTransport::delete_course(int line, int count)
{
	const list<int>::iterator& first = lines.find(line)->second.bus_stops.begin();
	const list<int>::iterator& null = lines.find(line)->second.bus_stops.end();

	for (list<int>::iterator it = first; it != null; ++it)
	{
		list<Bus>::iterator b = find_busses(line, *it);

		if (next(it) != null) // If it is not the last bus stop in the line
		{
			for (int i = 1; i < count; ++i) // Find the bus from the course that has to be deleted
			{
				while (b->destination == -1) // In case the line is cyclic - skip the busses that end the course at this stop
					++b;
				++b;
			}		

			while (b->destination == -1)
				++b;
		}
		else if (*prev(null) == *first) // If the line is cyclic and it is the last stop
		{
			int end_stop = 0;
			while (true)
			{
				if (b->destination == -1)
					++end_stop;
				if (count == end_stop)
					break;
				++b;
			}
		}
		else // If it is the last stop and the line is not cyclic
		{
			for (int i = 1; i < count; ++i)
				++b;
		}

		bus_stops.at(*it).busses_count--;
		bus_stops.at(*it).busses.at(line).erase(b);
	}

	cout << "\nCourse " << count << " has been removed from the line" << endl;	
}

void PublicTransport::clear()
{
	bs_count = 0;
	lines_count = 0;
	bus_stops.clear();
	bs_index.clear();
	lines.clear();
}
