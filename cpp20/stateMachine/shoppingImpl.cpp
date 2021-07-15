module shopping;

import std.core;
import helper;

using StoreItems = std::unordered_map<std::string, unsigned int>;

std::string computeOrderId(const StoreItems& items) {
	std::string str;
	for (const auto& [key, val] : items) {
		if (!key.empty()) {
			str += key.front();
			str += key[key.length() / 2];
			str += key.back();
		}
		str += std::to_string(val);
	}
	return str;
}

class OrderSystem {
public:
	void placeOrder(const StoreItems& items) {
		currentItemsToOrder_ = items;
	}
	bool completeOrder(const std::string& customerData) {
		if (!currentItemsToOrder_.empty() && !customerData.empty()) {
			auto now = floor<std::chrono::seconds>(std::chrono::system_clock::now());
			auto localtime = std::chrono::zoned_time<std::chrono::seconds>(std::chrono::current_zone(), now);
			completedOrders_.emplace_back(computeOrderId(currentItemsToOrder_), customerData, localtime);
			currentItemsToOrder_.clear();
			return true;
		}
		return false;
	}
	bool cancelCurrentOrder() {
		// additionally: add into canceled orders list... etc...
		currentItemsToOrder_.clear();
		return true;
	}

	void showCompletedOrders() const {
		for (const auto& [id, customer, sec] : completedOrders_)
			std::cout << std::format("completed: {} for {} at {}\n", id, customer, sec);
	}
private:
	StoreItems currentItemsToOrder_;
	
	struct Order {
		std::string id_;
		std::string customer_;
		std::chrono::zoned_seconds time_;
	};
	
	std::vector<Order> completedOrders_;
};


namespace state {
	struct EmptyBasket { };

	struct ActiveBasket {
		StoreItems items_;
	};

	struct ReadyToOrder { };

	struct Ordered { std::string orderName_; };
}

using ShopState = std::variant<state::EmptyBasket, state::ActiveBasket, state::ReadyToOrder, state::Ordered>;

namespace event {
	struct AddItem { std::string name_; unsigned int count_{ 0 }; };

	struct RemoveItem { std::string name_; unsigned int count_{ 0 }; };

	struct StartOrder {};

	struct CancelBasket {};

	struct CancelOrder {};

	struct CompleteOrder { std::string userData_; };
}

using PossibleEvent = std::variant<event::AddItem, event::RemoveItem, event::StartOrder, event::CancelBasket, event::CancelOrder, event::CompleteOrder>;

ShopState onEvent(const state::EmptyBasket& empty, const event::AddItem& newItem, OrderSystem&) {
	std::cout << std::format("EmptyBasket -> AddItem \"{}\", count {}\n", newItem.name_, newItem.count_);
	if (newItem.count_ > 0) // plus check if the item name is in the inventory... future...
	{
		state::ActiveBasket active;
		active.items_[newItem.name_] = newItem.count_;
		return active;
	}
	return state::EmptyBasket{};
}

ShopState onEvent(const state::ActiveBasket& active, const event::AddItem& newItem, OrderSystem&) {
	std::cout << std::format("ActiveBasket -> AddItem \"{}\", count {}\n", newItem.name_, newItem.count_);
	state::ActiveBasket newState = active;
	if (newItem.count_ > 0) { // plus check if the item name is in the inventory... future...		
		newState.items_[newItem.name_] += newItem.count_;
	}

	return newState;
}

ShopState onEvent(const state::ActiveBasket& active, const event::RemoveItem& remItem, OrderSystem&) {
	std::cout << std::format("ActiveBasket -> RemoveItem \"{}\", count {}\n", remItem.name_, remItem.count_);
	state::ActiveBasket newState = active;
	if (remItem.count_ > 0) {
		if (newState.items_[remItem.name_] > remItem.count_)
			newState.items_[remItem.name_] -= remItem.count_;
		else
			newState.items_.erase(remItem.name_);
	}

	if (active.items_.empty())
		return state::EmptyBasket{};

	return newState;
}

ShopState onEvent(const state::ActiveBasket& active, const event::StartOrder& startOrder, OrderSystem& orderSys) {
	std::cout << std::format("ActiveBasket -> StartOrder\n");
	orderSys.placeOrder(active.items_);

	return state::ReadyToOrder{  };
}

ShopState onEvent(const state::ReadyToOrder& ready, const event::CompleteOrder& complete, OrderSystem& orderSys) {
	std::cout << std::format("ReadyToOrder -> CompleteOrder for {}\n", complete.userData_);
	if (!orderSys.completeOrder(complete.userData_))
		return ready; // + throw some error? report?

	return state::Ordered{ };
}

ShopState onEvent(auto, auto, OrderSystem&) {
	throw std::logic_error{ "Unsupported state transition" };
}

class ShoppingStateMachine {
public:
	ShoppingStateMachine(OrderSystem& sys) : system_(sys)
	{}

	void processEvent(const PossibleEvent& event) {
		state_ = std::visit(helper::overload{
			[this](const auto& state, const auto evt) {
				return onEvent(state, evt, system_);
			}
			},
			state_, event);
	}

	std::string reportCurrentState() {
		return std::visit(
			helper::overload{ [](const state::EmptyBasket& state) -> std::string {
						return "Basket is empty";
						},
						[](const state::ActiveBasket& active) {
						std::string str = std::format("{} items in basket\n", active.items_.size());
						for (auto& [key, val] : active.items_)
							str += std::format("\t{} count: {}\n", key, val);
						return str;
						},
						[](const state::ReadyToOrder&) -> std::string {
						return "Ready to order";
						},
					[](const state::Ordered&) -> std::string {
						return "Ordered";
						}
			},
			state_);
	}

private:
	ShopState state_;
	OrderSystem& system_;
};

class Inventory {
public:
};

void ShoppingCartFSMTest() {
	OrderSystem orders;

	auto fsm = ShoppingStateMachine{ orders };
	std::cout << fsm.reportCurrentState() << '\n';
	try {
		fsm.processEvent(event::AddItem {"C++17 in Detail ebook", 3});
		std::cout << fsm.reportCurrentState() << '\n';
		fsm.processEvent(event::AddItem {"C++ Lambda Story ebook", 2});
		std::cout << fsm.reportCurrentState() << '\n';
		fsm.processEvent(event::AddItem {"CppStories subscription", 1});
		std::cout << fsm.reportCurrentState() << '\n';
		fsm.processEvent(event::RemoveItem {"C++17 in Detail ebook", 1});
		std::cout << fsm.reportCurrentState() << '\n';
		fsm.processEvent(event::StartOrder {});
		std::cout << fsm.reportCurrentState() << '\n';
		fsm.processEvent(event::CompleteOrder {"Bartek F."});
		std::cout << fsm.reportCurrentState() << '\n';

		orders.showCompletedOrders();

		// fsm.process_event(event::CompleteOrder {}); // unknon transition
	}
	catch (std::exception& ex) {
		std::cout << "Exception! " << ex.what() << '\n';
	}
}