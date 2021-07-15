module oldGame;

import std.core;

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


void OldGameHealthFSMTest() {
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