#include <iostream>
#include <vector>
#include <exception>
#include "List.h"
#include "interface.h"
#include "Structures.h"

struct MyStore : Store {

	const int DELIVERY_AMOUNT = 100;
	const int DELIVERY_TIME = 60;

	ActionHandler* actionHandler = nullptr;
	
	void setActionHandler(ActionHandler* handler) override {
		actionHandler = handler;
	}

	void init(int workerCount = 0, int startBanana = 0, int startSchweppes = 0) override {
		
		if (workerCount < 0 || startBanana < 0 || startSchweppes < 0)
			throw new std::exception("Invalid Initialization");

		banana = startBanana;
		schweppes = startSchweppes;
		deliveries.availableWorkers = workerCount;
	}

	void addClients(const Client* clients, int count) override {
		
		clientsList.reserve(count);

		int prevClientTime = 0;
		for (int i = 0; i < count; ++i)
		{
			if (validClient(clients[i], prevClientTime))
			{
				clientsList.push_back(clients[i]);
				prevClientTime = clients[i].arriveMinute;
			}
			else
				std::cout << "Client " << i << " - Invalid data! Client hasn't been added!" << std::endl;
		}
	}

	void advanceTo(int minute) override {

		int minWaitigTime = INT_MAX;
		int currentTime = getNextActionTime(minWaitigTime);
		const int size = clientsList.size();
		bool newDelivery = false;

		while (currentTime <= minute && (nextClient < size || !deliveries.workersList.empty() || !waitingClients.empty()))
		{
			/* Events are proceeded by certain order */
			
			// First send a worker(s)
			if (nextClient < size && currentTime == clientsList[nextClient].arriveMinute && (banana < clientsList[nextClient].banana || schweppes < clientsList[nextClient].schweppes))					
				enqueueClient(currentTime, minWaitigTime);
		
			// Second get a worker(s) back
			if (currentTime == deliveries.nextDeliveryTime)
			{
				getWorkerBack(currentTime);
				newDelivery = true;
			}

			// Third - client departure (3 ways a client to leave the store, executed by priority)			
			// 1st way -> There is a new delivery so we check if already waiting clients can take the products they came for 
			if (newDelivery && !waitingClients.empty())
			{
				/* 
					The method doesn't allow clients that had come later to go before clients that are still waiting for resources,
						however the later came clients may require not more than the current amount of resources.
					Another logic is to iterate trough the whole list and pass all clients that require less or equal than the current amount
				*/

				dequeueWaitingClient(currentTime, minWaitigTime);
				newDelivery = false;
			}

			// 2nd way -> The waiting time of a client has passed. All waiting clients that must go now leave by the order they came in the store 
			if (currentTime == minWaitigTime)			
				dequeueLeavingClient(currentTime, minWaitigTime);
			
			// 3rd way -> A new client can immediately take what he wants and leave
			if (nextClient < size && (currentTime == clientsList[nextClient].arriveMinute) && (banana >= clientsList[nextClient].banana) && (schweppes >= clientsList[nextClient].schweppes))
			{
				clientDepart(nextClient, currentTime);
				++nextClient;
			}
			
			currentTime = getNextActionTime(minWaitigTime);
		}		
	}

	virtual int getBanana() const {
		return banana;
	}

	virtual int getSchweppes() const {
		return schweppes;
	}

	bool validClient(const Client& client, int prevClientTime)
	{
		if (client.arriveMinute < prevClientTime || client.arriveMinute < 0 || client.banana < 0 ||
			client.schweppes < 0 || client.maxWaitTime < 0 || INT_MAX - client.arriveMinute - DELIVERY_TIME <= client.maxWaitTime)
				return false;

		return true;
	}


private:

	// Receive all deliveries in the current time
	void getWorkerBack(int currentTime)
	{
		while (currentTime == deliveries.nextDeliveryTime && !deliveries.workersList.empty())
		{
			ResourceType rssType = deliveries.workersList.front().product;
			if (rssType == ResourceType::banana)
			{
				deliveries.expectedBananas -= DELIVERY_AMOUNT;
				banana += DELIVERY_AMOUNT;
			}
			else
			{
				deliveries.expectedSchweppes -= DELIVERY_AMOUNT;
				schweppes += DELIVERY_AMOUNT;
			}

			++deliveries.availableWorkers;
			deliveries.workersList.pop();

			if (deliveries.workersList.empty())
				deliveries.nextDeliveryTime = INT_MAX;
			else
				deliveries.nextDeliveryTime = deliveries.workersList.front().departureTime + 60;

			actionHandler->onWorkerBack(currentTime, rssType);
		}
	}

	// Send a worker to get 1 type of resource
	void sendWorker(const ResourceType& rssType, int currentTime)
	{
		--deliveries.availableWorkers;

		if (deliveries.workersList.empty())
			deliveries.nextDeliveryTime = currentTime + DELIVERY_TIME;

		deliveries.workersList.push(Worker(currentTime, rssType));
		actionHandler->onWorkerSend(currentTime, rssType);
	}

	// When there aren't enough resources for a client - enqueue him and send workers for resources 
	void enqueueClient(int currentTime, int& minWaitigTime)
	{
		WaitingClient wClient(nextClient, currentTime + clientsList[nextClient].maxWaitTime);

		while (deliveries.availableWorkers > 0 && (schweppes + deliveries.expectedSchweppes < clientsList[nextClient].schweppes 
													 || banana + deliveries.expectedBananas < clientsList[nextClient].banana))
		{ 
			// Send workers first for the resource we need more. In case we need equal amount of both -> go for banana
			if (clientsList[nextClient].schweppes - schweppes - deliveries.expectedSchweppes > clientsList[nextClient].banana - banana - deliveries.expectedBananas)
			{
				deliveries.expectedSchweppes += DELIVERY_AMOUNT;
				sendWorker(ResourceType::schweppes, currentTime);
			}
			else
			{
				deliveries.expectedBananas += DELIVERY_AMOUNT;
				sendWorker(ResourceType::banana, currentTime);
			}
		}

		waitingClients.push_back(wClient);
		if (minWaitigTime > currentTime + clientsList[nextClient].maxWaitTime)
			minWaitigTime = currentTime + clientsList[nextClient].maxWaitTime;

		++nextClient;
	}

	// Client leaves the store, taking as much as he can
	void clientDepart(int index, int currentTime)
	{
		int takeBanana    = std::min(clientsList[index].banana, banana);
		int takeSchweppes = std::min(clientsList[index].schweppes, schweppes);

		banana    -= takeBanana;
		schweppes -= takeSchweppes;

		actionHandler->onClientDepart(index, currentTime, takeBanana, takeSchweppes);
	}

	// When there is a new delivery serve the clients (in the order they came) that take less or equal resources than the amount we have.
	void dequeueWaitingClient(int currentTime, int& minWaitigTime)
	{
		bool clientPassed = false;
		Node<WaitingClient>* node = waitingClients.pre_head();

		while(node->next && clientsList[node->next->data.index].banana <= banana && clientsList[node->next->data.index].schweppes <= schweppes)
		{
			clientPassed = true;
			clientDepart(node->next->data.index, currentTime);
			waitingClients.erase_after(node);
		}

		if (clientPassed)
			minWaitigTime = findFirstLeavingClient();
	}

	// All clients with expired waiting time leave and take what they can 
	void dequeueLeavingClient(int currentTime, int& minWaitigTime)
	{
		Node<WaitingClient>* node = waitingClients.pre_head();
		while (node->next != nullptr)
		{
			WaitingClient waitingClient = node->next->data;
			if (waitingClient.time == currentTime)
			{
				clientDepart(waitingClient.index, currentTime);
				waitingClients.erase_after(node);
			}
			else
				node = waitingClients.next(node);
		}

		minWaitigTime = findFirstLeavingClient();		
	}

	// Find the time of the next leaving client
	int findFirstLeavingClient()
	{
		if (waitingClients.empty())
			return INT_MAX;

		Node<WaitingClient>* node = waitingClients.front_node();
		int minTime = node->data.time;
		node = waitingClients.next(node);
		while (node != nullptr)
		{
			if (minTime > node->data.time)
				minTime = node->data.time;

			node=waitingClients.next(node);
		}

		return minTime;
	}

	// Get the time of the next action
	int getNextActionTime(int minWaitigTime)
	{
		int currentTime;
		currentTime = std::min(deliveries.nextDeliveryTime, minWaitigTime);
		if (nextClient < clientsList.size() && currentTime > clientsList[nextClient].arriveMinute)
			currentTime = clientsList[nextClient].arriveMinute;

		return currentTime;
	}

#pragma region Fields

	int banana; // current amount of banana in the store
	int schweppes; // current amount of schweppes in the store
	int nextClient = 0; // index in vector<Client>

	Delivery deliveries; // Using queue for the workers as i need FIFO
	std::vector<Client> clientsList; // Using vector because i don't delete, insert in middle or reorder it. Traversing all elements 1 by 1
	List<WaitingClient> waitingClients; // Using List because i need to delete elements from the middle

#pragma endregion
};

Store* createStore() {
	return new MyStore();
}