#include "EventBus.h"
#include "Actor.h"
#include "EngineUtils.h"
#include "Rigidbody.h"
#include <algorithm>

void EventBus::Publish(const string& eventType, luabridge::LuaRef eventObject) {
	EventBus& instance = getInstance();
	auto it = instance.subscribers.find(eventType);

	if (it != instance.subscribers.end()) {
		auto currentSubscribers = it->second;

		for (const auto& subscriber : currentSubscribers) {
			try {
				subscriber.function(subscriber.component, eventObject);
			}
			catch (const luabridge::LuaException& e) {
				string componentName = "";

				if (subscriber.component.isTable()) {
					Actor* actor = subscriber.component["actor"].cast<Actor*>();
					if (actor) {
						componentName = actor->name;
					}
				}
				else if (subscriber.component.isUserdata()) {
					if (subscriber.component.isInstance<Rigidbody>()) {
						Rigidbody* rb = subscriber.component.cast<Rigidbody*>();
						if (rb) {
							componentName = rb->GetActor()->name;
						}
					}
				}

				EngineUtils::ReportError(componentName, e);
			}
		}
	}
}

void EventBus::Subscribe(const string& eventType, luabridge::LuaRef component, luabridge::LuaRef function) {
	if (!component.isTable() && !component.isUserdata()) {
		cout << "Invalid component reference in Subscribe" << endl;
		exit(0);
	}

	if (!function.isFunction()) {
		cout << "Invalid function reference in Subscribe" << endl;
		exit(0);
	}

	getInstance().pendingRequests.emplace_back(eventType, component, function, true);
}

void EventBus::Unsubscribe(const string& eventType, luabridge::LuaRef component, luabridge::LuaRef function) {
	if (!component.isTable() && !component.isUserdata()) {
		cout << "Invalid component reference in Subscribe" << endl;
		exit(0);
	}

	if (!function.isFunction()) {
		cout << "Invalid function reference in Subscribe" << endl;
		exit(0);
	}

	getInstance().pendingRequests.emplace_back(eventType, component, function, false);
}

void EventBus::processSubscriptions() {
	for (const auto& request : pendingRequests) {
		if (request.isSubscribe) {
			subscribers[request.eventType].push_back(request.subscriber);
		}
		else {
			auto& subs = subscribers[request.eventType];
			auto it = find_if(subs.begin(), subs.end(),
				[&request](const Subscriber& sub) {
					return sub == request.subscriber;
				});

			if (it != subs.end()) {
				subs.erase(it);
			}
		}
	}

	pendingRequests.clear();
}