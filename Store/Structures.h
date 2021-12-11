#pragma once
#include <iostream>
#include "interface.h"
#include "Queue.h"

struct WaitingClient {
	int index; // The client's index in the vector with all clients
	int time;  // The last minute when the client could be in the store
	
	WaitingClient (int i = 0, int t = 0) : index(i), time(t) {};
};

struct Worker {
	int departureTime;    // The time when a worker is sent for restocking
	ResourceType product; // The product type he is going for
	Worker(int dT = INT_MAX, ResourceType pr = ResourceType::banana) : departureTime(dT), product(pr) {};
};

struct Delivery {
	int expectedBananas = 0;		  // The whole amount of bananas expected all workers to bring when they get back
	int expectedSchweppes = 0;		  // The whole amount of schweppes expected all workers to bring when they get back
	int availableWorkers = 0;		  // The count of workers that can be sent for restocking in any moment
	int nextDeliveryTime = INT_MAX;	  // The time when the next worker arrives with resources 
	Queue<Worker> workersList;		  // Deliveries are proceeded by the time they come (FIFO)
};
