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

		HealthState onHeal(unsigned int param)
		{
			if (state_ == HealthState::PlayerAlive) {
				std::cout << std::format("PlayerAlive -> Heal points {}\n", param);

				currentHealth_ += param;
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

	using PossibleEvent = std::variant<event::HitByMonster, event::Heal, event::Restart>;

	HealthState onEvent(const state::PlayerAlive& alive, const event::HitByMonster& monster) {
		std::cout << std::format("PlayerAlive -> HitByMonster force {}\n", monster.forcePoints_);
		if (alive.health_ > monster.forcePoints_)
		{
			return state::PlayerAlive{alive.health_ - monster.forcePoints_, alive.remainingLives_};
		}

		if (alive.remainingLives_ > 0)
			return state::PlayerDead{ alive.remainingLives_ - 1 };

		return state::GameOver{};
	}

	HealthState onEvent(const state::PlayerAlive& alive, const event::Heal& healingBonus) {
		std::cout << std::format("PlayerAlive -> Heal points {}\n", healingBonus.points_);

		return state::PlayerAlive{alive.health_ + healingBonus.points_, alive.remainingLives_};;
	}

	HealthState onEvent(const state::PlayerDead& dead, const event::Restart& restart) {
		std::cout << std::format("PlayerDead -> restart\n");

		return state::PlayerAlive{ restart.startHealth_, dead.remainingLives_ };
	}

	HealthState onEvent(const state::GameOver& over, const event::Restart& restart) {
		std::cout << std::format("GameOver -> restart\n");

		std::cout << "Game Over, please restart the whole game!\n";

		return over;
	}

	HealthState onEvent(const auto&, const auto&) {
		throw std::logic_error{ "Unsupported state transition" };
	}

	class GameStateMachine {
	public:
		void startGame(unsigned int health, unsigned int lives) {
			state_ = state::PlayerAlive{ health, lives };
		}

		void processEvent(const PossibleEvent& event) {
			state_ = std::visit(detail::overload{
				[](const auto& state, const auto& evt) {
				  return onEvent(state, evt);
				}
				},
				state_, event);
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

	std::cout << std::format("sizeof(HealthState):   {}\n", sizeof(HealthState));
    std::cout << std::format("sizeof(PossibleEvent): {}\n", sizeof(PossibleEvent));

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
	GameHealtfFSMTest();
	//OldGameHealtfFSMTest();
}