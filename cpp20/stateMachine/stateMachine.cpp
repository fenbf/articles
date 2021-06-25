import std.core;

/*#include <chrono>
#include <iostream>
#include <variant>
#include <unordered_map>
#include <vector>*/

namespace detail {
	template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
	//template<class... Ts> overload(Ts...)->overload<Ts...>; // no need in C++20, MSVC?
}

namespace shopping {

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
			//Order(std::string id, std::string cust, std::chrono::seconds t) : id_(std::move(id)), customer_(std::move(cust)), time_(t) { }
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

	ShopState onEvent(state::EmptyBasket empty, event::AddItem newItem, OrderSystem&) {
		std::cout << std::format("EmptyBasket -> AddItem \"{}\", count {}\n", newItem.name_, newItem.count_);
		if (newItem.count_ > 0) // plus check if the item name is in the inventory... future...
		{
			state::ActiveBasket active;
			active.items_[newItem.name_] = newItem.count_;
			return active;
		}
		return state::EmptyBasket{};
	}

	ShopState onEvent(state::ActiveBasket active, event::AddItem newItem, OrderSystem&) {
		std::cout << std::format("ActiveBasket -> AddItem \"{}\", count {}\n", newItem.name_, newItem.count_);
		if (newItem.count_ > 0) // plus check if the item name is in the inventory... future...
			active.items_[newItem.name_] += newItem.count_;

		return active;
	}

	ShopState onEvent(state::ActiveBasket active, event::RemoveItem remItem, OrderSystem&) {
		std::cout << std::format("ActiveBasket -> RemoveItem \"{}\", count {}\n", remItem.name_, remItem.count_);
		if (remItem.count_ > 0) {
			if (active.items_[remItem.name_] > remItem.count_)
				active.items_[remItem.name_] -= remItem.count_;
			else
				active.items_.erase(remItem.name_);
		}

		if (active.items_.empty())
			return state::EmptyBasket{};

		return active;
	}

	ShopState onEvent(state::ActiveBasket active, event::StartOrder startOrder, OrderSystem& orderSys) {
		std::cout << std::format("ActiveBasket -> StartOrder\n");
		orderSys.placeOrder(active.items_);

		return state::ReadyToOrder{  };
	}

	ShopState onEvent(state::ReadyToOrder ready, event::CompleteOrder complete, OrderSystem& orderSys) {
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

		template <typename Event>
		void processEvent(Event&& event) {
			state_ = std::visit(detail::overload{
				[&](const auto& state) {
				  return onEvent(state, std::forward<Event>(event), system_);
				}
				},
				state_);
		}

		std::string reportCurrentState() {
			return std::visit(
				detail::overload{ [](const state::EmptyBasket& state) -> std::string {
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
}

void ShoppingCartFSMTest() {
	using namespace shopping;

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

namespace oldGame {
	enum class HealthState { PlayerAlive, PlayerDead, GameOver };
	enum class Event { HitByMonster, Heal, Restart };

	class GameStateMachine {
	public:
		void startGame(unsigned int health, unsigned int lives) {
			state_ = HealthState::PlayerAlive;
			currentHealth_ = health;
			remainingLives_ = lives;
		}

		void processEvent(Event evt, unsigned int param) {
			switch (evt)
			{
			case Event::HitByMonster:
				state_ = onHitByMonster(param);
				break;
			case Event::Heal:
				state_ = onHeal(param);
				break;
			case Event::Restart:
				state_ = onRestart(param);
				break;
			default:
				throw std::logic_error{ "Unsupported state transition" };
				break;
			}
		}

		void reportCurrentState() {
			switch (state_)
			{
			case HealthState::PlayerAlive:
				std::cout << std::format("PlayerAlive {} remaining lives {}\n", currentHealth_, remainingLives_);
				break;
			case HealthState::PlayerDead:
				std::cout << std::format("PlayerDead, remaining lives {}\n", remainingLives_);
				break;
			case HealthState::GameOver:
				std::cout << std::format("GameOver\n");
				break;
			default:
				break;
			}
		}

	private:
		HealthState onHitByMonster(unsigned int param) {
			if (state_ == HealthState::PlayerAlive) {
				std::cout << std::format("PlayerAlive -> HitByMonster force {}\n", param);
				if (currentHealth_ > param) {
					currentHealth_ -= param;
					return state_;
				}

				if (remainingLives_ > 0)
				{
					--remainingLives_;
					return HealthState::PlayerDead;
				}

				return HealthState::GameOver;
			}

			throw std::logic_error{ "Unsupported state transition" };
		}
HealthState onHeal(unsigned int param) {
	if (state_ == HealthState::PlayerAlive) {
		std::cout << std::format("PlayerAlive -> Heal points {}\n", param);

		currentHealth_+= param;
		return state_;
	}
	throw std::logic_error{ "Unsupported state transition" };
}
HealthState onRestart(unsigned int param) {
	if (state_ == HealthState::PlayerDead) {
		std::cout << std::format("PlayerDead -> restart\n");
		currentHealth_ = param;
		return HealthState::PlayerAlive;
	}
	throw std::logic_error{ "Unsupported state transition" };
}

	private:
		HealthState state_;
		unsigned int currentHealth_{ 0 };
		unsigned int remainingLives_{ 0 };
	};
}

void OldGameHealtfFSMTest() {
	using namespace oldGame;

GameStateMachine game;
game.startGame(100, 1);

try {
	game.processEvent(Event::HitByMonster, 30);
	game.reportCurrentState();
	game.processEvent(Event::HitByMonster, 30);
	game.reportCurrentState();
	game.processEvent(Event::HitByMonster, 30);
	game.reportCurrentState();
	game.processEvent(Event::HitByMonster, 30);
	game.reportCurrentState();
	game.processEvent(Event::Restart, 100);
	game.reportCurrentState();
	game.processEvent(Event::HitByMonster, 60);
	game.reportCurrentState();
	game.processEvent(Event::HitByMonster, 50);
	game.reportCurrentState();
	game.processEvent(Event::Restart, 100);
	game.reportCurrentState();

}
catch (std::exception& ex) {
	std::cout << "Exception! " << ex.what() << '\n';
}
}

namespace game {

namespace state {
	struct PlayerAlive { unsigned int health_{ 0 }; unsigned int remainingLives_{ 0 }; };

	struct PlayerDead { unsigned int remainingLives_{ 0 }; };

	struct GameOver { };
}

using HealthState = std::variant<state::PlayerAlive, state::PlayerDead, state::GameOver>;

namespace event {
	struct HitByMonster { unsigned int forcePoints_{ 0 }; };

	struct Heal { unsigned int points_{ 0 }; };

	struct Restart { unsigned int startHealth_{ 0 }; };
}

	HealthState onEvent(state::PlayerAlive alive, const event::HitByMonster& monster) {
		std::cout << std::format("PlayerAlive -> HitByMonster force {}\n", monster.forcePoints_);
		if (alive.health_ > monster.forcePoints_)
		{
			alive.health_ -= monster.forcePoints_;
			return alive;
		}

		if (alive.remainingLives_ > 0)
			return state::PlayerDead{ alive.remainingLives_ - 1 };

		return state::GameOver{};
	}

	HealthState onEvent(state::PlayerAlive alive, const event::Heal& healingBonus) {
		std::cout << std::format("PlayerAlive -> Heal points {}\n", healingBonus.points_);

		alive.health_ += healingBonus.points_;
		return alive;
	}

	HealthState onEvent(state::PlayerDead dead, const event::Restart& restart) {
		std::cout << std::format("PlayerDead -> restart\n");

		return state::PlayerAlive{ restart.startHealth_, dead.remainingLives_ };
	}

	HealthState onEvent(state::GameOver over, const event::Restart& restart) {
		std::cout << std::format("GameOver -> restart\n");

		std::cout << "Game Over, please restart the whole game!\n";

		return over;
	}

	HealthState onEvent(auto state, const auto&) {
		throw std::logic_error{ "Unsupported state transition" };
	}

	class GameStateMachine {
	public:
		void startGame(unsigned int health, unsigned int lives) {
			state_ = state::PlayerAlive{ health, lives };
		}

		template <typename Event>
		void processEvent(const Event& event) {
			state_ = std::visit(detail::overload{
				[&](const auto& state) {
				  return onEvent(state, event);
				}
				},
				state_);
		}

		void reportCurrentState() {
			std::visit(
				detail::overload{ [](const state::PlayerAlive& alive) {
						   std::cout << std::format("PlayerAlive {} remaining lives {}\n", alive.health_, alive.remainingLives_);
						 },
						 [](const state::PlayerDead& dead) {
						   std::cout << std::format("PlayerDead, remaining lives {}\n", dead.remainingLives_);
						 },
						 [](const state::GameOver& over) {
						   std::cout << std::format("GameOver\n");
						 }
				},
				state_);
		}

	private:
		HealthState state_;
	};
}

void GameHealtfFSMTest() {
	using namespace game;

	GameStateMachine game;
	game.startGame(100, 1);

	try {
		game.processEvent(event::HitByMonster {30});
		game.reportCurrentState();
		game.processEvent(event::HitByMonster {30});
		game.reportCurrentState();
		game.processEvent(event::HitByMonster {30});
		game.reportCurrentState();
		game.processEvent(event::HitByMonster {30});
		game.reportCurrentState();
		game.processEvent(event::Restart {100});
		game.reportCurrentState();
		game.processEvent(event::HitByMonster {60});
		game.reportCurrentState();
		game.processEvent(event::HitByMonster {50});
		game.reportCurrentState();
		game.processEvent(event::Restart {100});
		game.reportCurrentState();

	}
	catch (std::exception& ex) {
		std::cout << "Exception! " << ex.what() << '\n';
	}
}

int main() {
	//GameHealtfFSMTest();
	OldGameHealtfFSMTest();
	//ShoppingCartFSMTest();	
}