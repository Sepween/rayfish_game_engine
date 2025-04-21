#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "lua.hpp"
#include "LuaBridge/LuaBridge.h"

using namespace std;

class EventBus {
public:
	static EventBus& getInstance(){
		static EventBus instance;
		return instance;
	}

	static void Publish(const string& eventType, luabridge::LuaRef eventObject);
	static void Subscribe(const string& eventType, luabridge::LuaRef component, luabridge::LuaRef function);
	static void Unsubscribe(const string& eventType, luabridge::LuaRef component, luabridge::LuaRef function);

	void processSubscriptions();

private:
	struct Subscriber {
		luabridge::LuaRef component;
		luabridge::LuaRef function;

		Subscriber(luabridge::LuaRef componentIn, luabridge::LuaRef functionIn) :
			component(componentIn), function(functionIn) {}

		bool operator==(const Subscriber& other) const {
			return component == other.component && 
				function == other.function;
		}
	};

	struct SubscriptionRequest {
		string eventType;
		Subscriber subscriber;
		bool isSubscribe;

		SubscriptionRequest(const string& event, luabridge::LuaRef componentIn,
			luabridge::LuaRef functionIn, bool sub) :
			eventType(event),
			subscriber(componentIn, functionIn),
			isSubscribe(sub) {}
	};

	unordered_map<string, vector<Subscriber>> subscribers;
	vector<SubscriptionRequest> pendingRequests;

	EventBus() = default;
	EventBus(const EventBus&) = delete;
	EventBus& operator=(const EventBus&) = delete;
};