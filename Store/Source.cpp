#pragma once
#include <iostream>
#include <vector>
#include "../HomeWork1/Queue.h"
#include "../HomeWork1/implementation.h"

using std::cin;
using std::cout;
using std::endl;

// Same as the one from tests.cpp
struct StoreEvent {
	enum Type {
		WorkerSend, WorkerBack, ClientDepart
	};

	Type type;
	int minute;

	struct Worker {
		ResourceType resource;
	} worker;

	struct Client {
		int index;
		int banana;
		int schweppes;
	} client;
};

// Similar to the one from tests.cpp
struct TestStore : ActionHandler {
	Store* impl = nullptr;
	Queue<StoreEvent> log;

	TestStore()
		: impl(createStore()) {
		impl->setActionHandler(this);
	}

	~TestStore() {
		delete impl;
	}

	TestStore(const TestStore&) = delete;
	TestStore& operator=(const TestStore&) = delete;

	void init(int workerCount, int startBanana, int startSchweppes) {
		impl->init(workerCount, startBanana, startSchweppes);
	}

	void advanceTo(int minute) {
		impl->advanceTo(minute);
	}

	void execute()
	{
		advanceTo(INT_MAX);
		print();
	}

	void print()
	{
		StoreEvent ev;
		Queue<StoreEvent> helper;

		while (!log.empty())
		{
			ev = log.front();

			if (!helper.empty() && (ev.minute > helper.front().minute || ev.type == StoreEvent::ClientDepart))
			{
				while (!helper.empty())
				{
					cout << (helper.front().type == StoreEvent::WorkerBack ? "D " : "W ") << helper.front().minute << " Schweppes" << endl;
					helper.pop();
				}
			}

			if (ev.type != StoreEvent::ClientDepart)
			{
				if (ev.worker.resource == ResourceType::banana)								
					cout << (ev.type == StoreEvent::WorkerBack ? "D " : "W ") << ev.minute << " Banana" << endl;				
				else
					helper.push(ev);
			}
			else			
				cout << ev.client.index << " " << ev.minute << " " << ev.client.banana << " " << ev.client.schweppes << endl;
			
			log.pop();
		}

		while (!helper.empty())
		{
			cout << (ev.type == StoreEvent::WorkerBack ? "D " : "W ") << ev.minute << " Schweppes" << endl;
			helper.pop();
		}
	}

	void addClients(const std::vector<Client>& clients) {
		impl->addClients(clients.data(), clients.size());
	}

private:
	void onWorkerSend(int minute, ResourceType resource) override {
		StoreEvent ev;
		ev.type = StoreEvent::WorkerSend;
		ev.minute = minute;
		ev.worker.resource = resource;
		log.push(ev);
	}

	void onWorkerBack(int minute, ResourceType resource) override {
		StoreEvent ev;
		ev.type = StoreEvent::WorkerBack;
		ev.minute = minute;
		ev.worker.resource = resource;
		log.push(ev);
	}

	void onClientDepart(int index, int minute, int banana, int schweppes) override {
		StoreEvent ev;
		ev.type = StoreEvent::ClientDepart;
		ev.minute = minute;
		ev.client.banana = banana;
		ev.client.schweppes = schweppes;
		ev.client.index = index;
		log.push(ev);
	}
};

int main()
{
	TestStore store;

	int atime, wtime, banana, scheweppes, count;
	cin >> count;
	store.init(count, 0, 0);

	cin >> count;
	std::vector<Client> clients(count);

	for (int i = 0; i < count; ++i)
	{
		cin >> atime >> banana >> scheweppes >> wtime;
		clients[i] = { atime, banana, scheweppes, wtime };
	}
	store.addClients(clients);
	cout << endl;
	store.execute();

	return 0;
}