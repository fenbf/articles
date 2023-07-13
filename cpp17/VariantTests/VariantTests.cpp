#include <benchmark/benchmark.h>

#include <iostream>
#include <stdexcept>
#include <variant>
#include <vector>

namespace helper {
	template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
	template<class... Ts> overload(Ts...) -> overload<Ts...>; // no need in C++20, MSVC?
}

namespace state {
	struct Idle { };
	struct AmountEntered { int amount{ 0 }; int availableChange{ 0 }; };
	struct ItemSelected { std::string item; int availableChange{ 0 }; };
	struct ChangeDispensed { int change{ 0 }; };
	struct Dummy { int a; int b; };
	struct Dummy2 { int a; double b; };
	struct Dummy3 { int a; float b; };
	struct Dummy4 { int a; char b; };
}

struct VendingState : std::variant<state::Idle, state::AmountEntered, state::ItemSelected, state::ChangeDispensed, state::Dummy, state::Dummy2, state::Dummy3, state::Dummy4>
{
	using variant::variant;
};

namespace event {
	struct EnterAmount { int amount{ 0 }; };
	struct SelectItem { std::string item; };
	struct DispenseChange { };
	struct Reset { };
	struct Dummy { int a; int b; };
	struct Dummy2 { int a; double b; };
	struct Dummy3 { int a; float b; };
	struct Dummy4 { int a; char b; };
}

using PossibleEvent = std::variant<event::EnterAmount, event::SelectItem, event::DispenseChange, event::Reset, event::Dummy, event::Dummy2, event::Dummy3, event::Dummy4>;

struct Item { std::string name; unsigned quantity{ 0 }; int price{ 0 }; };

class VendingMachine {

public:
	void Reset() {
		state_ = state::Idle{ };
		registry_ = {
		  Item {"Coke", 5, 50},
		  Item {"Pepsi", 3, 45},
		  Item {"Water", 4, 35},
		  Item {"Snack", 5, 25}
		};
	}

	void processEvent(const PossibleEvent& event) {
		state_ = std::visit(helper::overload{
			[this](const auto& state, const auto& evt) {
				return onEvent(state, evt);
			}
			}, state_, event);
	}

	void processEventGet(const PossibleEvent& event) {
		if (auto enterAmountEvent = std::get_if<event::EnterAmount>(&event)) {
			if (auto idleState = std::get_if<state::Idle>(&state_)) {
				state_ = onEvent(*idleState, *enterAmountEvent);
			}
			else if (auto amountEnteredState = std::get_if<state::AmountEntered>(&state_)) {
				state_ = onEvent(*amountEnteredState, *enterAmountEvent);
			}
			else {
				throw std::logic_error{ "Unsupported event transition" };
			}
		}
		else if (auto selectItemEvent = std::get_if<event::SelectItem>(&event)) {
			if (auto amountEnteredState = std::get_if<state::AmountEntered>(&state_)) {
				state_ = onEvent(*amountEnteredState, *selectItemEvent);
			}
			else {
				throw std::logic_error{ "Unsupported event transition" };
			}
		}
		else if (auto dispenseChangeEvent = std::get_if<event::DispenseChange>(&event)) {
			if (auto itemSelectedState = std::get_if<state::ItemSelected>(&state_)) {
				state_ = onEvent(*itemSelectedState, *dispenseChangeEvent);
			}
			else if (auto amoundEntered = std::get_if<state::AmountEntered>(&state_)) {
				state_ = onEvent(*amoundEntered, *dispenseChangeEvent);
			}
			else {
				throw std::logic_error{ "Unsupported event transition" };
			}
		}
		else if (auto resetEvent = std::get_if<event::Reset>(&event)) {
			if (auto changeDispensedState = std::get_if<state::ChangeDispensed>(&state_)) {
				state_ = onEvent(*changeDispensedState, *resetEvent);
			}
			else {
				throw std::logic_error{ "Unsupported event transition" };
			}
		}
		else {
			throw std::logic_error{ "Unsupported event type" };
		}
	}

	void processEventGetInline(const PossibleEvent& event) {
		if (auto enterAmountEvent = std::get_if<event::EnterAmount>(&event)) {
			if (auto idleState = std::get_if<state::Idle>(&state_)) {
				state_ = state::AmountEntered{ enterAmountEvent->amount, enterAmountEvent->amount };
			}
			else if (auto amountEnteredState = std::get_if<state::AmountEntered>(&state_)) {
				state_ = state::AmountEntered{ amountEnteredState->amount + enterAmountEvent->amount, amountEnteredState->availableChange + enterAmountEvent->amount };
			}
			else {
				throw std::logic_error{ "Unsupported event transition" };
			}
		}
		else if (auto selectItemEvent = std::get_if<event::SelectItem>(&event)) {
			if (auto amountEnteredState = std::get_if<state::AmountEntered>(&state_)) {
				auto it = std::ranges::find(registry_, selectItemEvent->item, &Item::name);
				if (it != registry_.end() && it->quantity > 0 && it->price <= amountEnteredState->amount)
				{
					--(it->quantity);
					state_ = state::ItemSelected{ selectItemEvent->item, amountEnteredState->availableChange - it->price };
				}
				else {
					state_ = *amountEnteredState;
				}
			}
			else {
				throw std::logic_error{ "Unsupported event transition" };
			}
		}
		else if (auto dispenseChangeEvent = std::get_if<event::DispenseChange>(&event)) {
			if (auto itemSelectedState = std::get_if<state::ItemSelected>(&state_)) {
				state_ = state::ChangeDispensed{ itemSelectedState->availableChange };
			}
			else if (auto amoundEntered = std::get_if<state::AmountEntered>(&state_)) {
				state_ = state::Idle{ };
			}
			else {
				throw std::logic_error{ "Unsupported event transition" };
			}
		}
		else if (auto resetEvent = std::get_if<event::Reset>(&event)) {
			if (auto changeDispensedState = std::get_if<state::ChangeDispensed>(&state_)) {
				state_ = state::Idle{ };
			}
			else {
				throw std::logic_error{ "Unsupported event transition" };
			}
		}
		else {
			throw std::logic_error{ "Unsupported event type" };
		}
	}

	void processEventHoldAlternative(const PossibleEvent& event) {
		if (std::holds_alternative<event::EnterAmount>(event)) {
			auto& enterAmountEvent = std::get<event::EnterAmount>(event);
			if (std::holds_alternative<state::Idle>(state_)) {
				auto& idleState = std::get<state::Idle>(state_);
				state_ = state::AmountEntered{ enterAmountEvent.amount, enterAmountEvent.amount };
			}
			else if (std::holds_alternative<state::AmountEntered>(state_)) {
				auto& amountEnteredState = std::get<state::AmountEntered>(state_);
				state_ = state::AmountEntered{ amountEnteredState.amount + enterAmountEvent.amount, amountEnteredState.availableChange + enterAmountEvent.amount };
			}
			else {
				throw std::logic_error{ "Unsupported event transition" };
			}
		}
		else if (std::holds_alternative<event::SelectItem>(event)) {
			auto& selectItemEvent = std::get<event::SelectItem>(event);
			if (std::holds_alternative<state::AmountEntered>(state_)) {
				auto& amountEnteredState = std::get<state::AmountEntered>(state_);
				auto it = std::ranges::find(registry_, selectItemEvent.item, &Item::name);
				if (it != registry_.end() && it->quantity > 0 && it->price <= amountEnteredState.amount)
				{
					--(it->quantity);
					state_ = state::ItemSelected{ selectItemEvent.item, amountEnteredState.availableChange - it->price };
				}
				else {
					state_ = amountEnteredState;
				}
			}
			else {
				throw std::logic_error{ "Unsupported event transition" };
			}
		}
		else if (std::holds_alternative<event::DispenseChange>(event)) {
			if (std::holds_alternative<state::ItemSelected>(state_)) {
				auto& itemSelectedState = std::get<state::ItemSelected>(state_);
				state_ = state::ChangeDispensed{ itemSelectedState.availableChange };
			}
			else if (std::holds_alternative<state::AmountEntered>(state_)) {
				state_ = state::Idle{ };
			}
			else {
				throw std::logic_error{ "Unsupported event transition" };
			}
		}
		else if (std::holds_alternative<event::Reset>(event)) {
			if (std::holds_alternative<state::ChangeDispensed>(state_)) {
				state_ = state::Idle{ };
			}
			else {
				throw std::logic_error{ "Unsupported event transition" };
			}
		}
		else {
			throw std::logic_error{ "Unsupported event type" };
		}
	}


	VendingState onEvent(const state::Idle& idle, const event::EnterAmount& amount) {
		//std::cout << std::format("Idle -> EnterAmount: {}\n", amount.amount);
		return state::AmountEntered{ amount.amount, amount.amount };
	}

	VendingState onEvent(const state::AmountEntered& current, const event::EnterAmount& amount) {
		//std::cout << std::format("AmountEntered {} -> EnterAmount: {}\n", current.amount, amount.amount);
		return state::AmountEntered{ current.amount + amount.amount, current.availableChange + amount.amount };
	}

	VendingState onEvent(const state::AmountEntered& amountEntered, const event::SelectItem& item) {
		//std::cout << std::format("AmountEntered {} -> SelectItem: {}\n", amountEntered.amount, item.item);

		auto it = std::ranges::find(registry_, item.item, &Item::name);
		if (it != registry_.end() && it->quantity > 0 && it->price <= amountEntered.amount)
		{
			--(it->quantity);
			return state::ItemSelected{ item.item, amountEntered.availableChange - it->price };
		}
		return amountEntered;
	}

	VendingState onEvent(const state::ItemSelected& itemSelected, const event::DispenseChange& change) {
		//std::cout << std::format("ItemSelected -> DispenseChange {}\n", itemSelected.availableChange);
		return state::ChangeDispensed{ itemSelected.availableChange };
	}

	VendingState onEvent(const state::ChangeDispensed& changeDispensed, const event::Reset& reset) {
		//std::cout << "ChangeDispensed -> Reset\n";
		return state::Idle{ };
	}

	VendingState onEvent(const state::AmountEntered& emountEntered, const event::DispenseChange& change) {
		//std::cout << "AmountEntered -> Dispense\n";
		return state::Idle{ };
	}

	VendingState onEvent(const state::AmountEntered& emountEntered, const event::Reset& reset) {
		//std::cout << "AmountEntered -> Reset\n";
		return state::Idle{ };
	}

	VendingState onEvent(const auto&, const auto&) {
		throw std::logic_error{ "Unsupported event transition" };
	}

private:

	std::vector<Item> registry_{
		Item {"Coke", 5, 50},
		Item {"Pepsi", 3, 45},
		Item {"Water", 4, 35},
		Item {"Snack", 5, 25}
	};

	VendingState state_;
};

enum class EnumState {
	Idle,
	AmountEntered,
	ItemSelected,
	ChangeDispensed
};

enum class EnumEvent {
	EnterAmount,
	SelectItem,
	DispenseChange,
	Reset
};

class EnumVendingMachine {
public:
	void Reset() {
		state_ = EnumState::Idle;
		registry_ = {
			Item {"Coke", 5, 50},
			Item {"Pepsi", 3, 45},
			Item {"Water", 4, 35},
			Item {"Snack", 5, 25}
		};
		availableChange_ = 0;
	}

	void processEvent(EnumEvent event, const std::string& item = "", int amount = 0) {
		switch (state_) {
		case EnumState::Idle: {
			if (event == EnumEvent::EnterAmount) {
				availableChange_ += amount;
				state_ = EnumState::AmountEntered;
			}
			else {
				throw std::logic_error{ "Unsupported event transition" };
			}
			break;
		}
		case EnumState::AmountEntered: {
			switch (event) {
			case EnumEvent::EnterAmount: {
				availableChange_ += amount;
				break;
			}
			case EnumEvent::SelectItem: {
				auto it = std::find_if(registry_.begin(), registry_.end(),
					[&item](const Item& i) { return i.name == item; });
				if (it != registry_.end() && it->quantity > 0 && it->price <= availableChange_) {
					--(it->quantity);
					availableChange_ -= it->price;
					state_ = EnumState::ItemSelected;
				}
				else {
					throw std::logic_error{ "Item not available or insufficient amount entered" };
				}
				break;
			}
			case EnumEvent::DispenseChange: {
				state_ = EnumState::Idle;
				break;
			}
			default: {
				throw std::logic_error{ "Unsupported event transition" };
			}
			}
			break;
		}
		case EnumState::ItemSelected: {
			if (event == EnumEvent::DispenseChange) {
				state_ = EnumState::ChangeDispensed;
			}
			else {
				throw std::logic_error{ "Unsupported event transition" };
			}
			break;
		}
		case EnumState::ChangeDispensed: {
			if (event == EnumEvent::Reset) {
				state_ = EnumState::Idle;
			}
			else {
				throw std::logic_error{ "Unsupported event transition" };
			}
			break;
		}
		}
	}

private:
	std::vector<Item> registry_{
		Item {"Coke", 5, 50},
		Item {"Pepsi", 3, 45},
		Item {"Water", 4, 35},
		Item {"Snack", 5, 25}
	};

	EnumState state_{ EnumState::Idle };
	int availableChange_{ 0 };
};

constexpr int RUNS = 10000;

static void VisitVersion(benchmark::State& state) {
	VendingMachine vm;
	// Code inside this loop is measured repeatedly
	for (auto _ : state) {
		vm.Reset();
		for (int i = 0; i < RUNS; ++i)
		{
			vm.processEventGet(event::EnterAmount { 30 });
			vm.processEventGet(event::EnterAmount { 30 });
			vm.processEventGet(event::DispenseChange {});
			vm.processEventGet(event::EnterAmount { 30 });
			vm.processEventGet(event::DispenseChange {});
			vm.processEventGet(event::EnterAmount { 30 });
			vm.processEventGet(event::EnterAmount { 30 });
		}
		vm.processEvent(event::SelectItem { "Coke" });
		vm.processEvent(event::DispenseChange {});
		vm.processEvent(event::Reset { });
	}
}
// Register the function as a benchmark
BENCHMARK(VisitVersion);

static void GetIfVersion(benchmark::State& state) {
	VendingMachine vm;
	// Code inside this loop is measured repeatedly
	for (auto _ : state) {
		vm.Reset();
		for (int i = 0; i < RUNS; ++i)
		{
			vm.processEventGet(event::EnterAmount { 30 });
			vm.processEventGet(event::DispenseChange {});
			vm.processEventGet(event::EnterAmount { 30 });
			vm.processEventGet(event::EnterAmount { 30 });
			vm.processEventGet(event::DispenseChange {});
			vm.processEventGet(event::EnterAmount { 30 });
			vm.processEventGet(event::EnterAmount { 30 });
		}
		vm.processEventGet(event::SelectItem { "Coke" });	
		vm.processEventGet(event::DispenseChange {});
		vm.processEventGet(event::Reset { });
	}
}
// Register the function as a benchmark
BENCHMARK(GetIfVersion);

static void GetIfInlineVersion(benchmark::State& state) {
	VendingMachine vm;
	// Code inside this loop is measured repeatedly
	for (auto _ : state) {
		vm.Reset();
		for (int i = 0; i < RUNS; ++i)
		{
			vm.processEventGetInline(event::EnterAmount { 30 });
			vm.processEventGetInline(event::DispenseChange {});
			vm.processEventGetInline(event::EnterAmount { 30 });
			vm.processEventGetInline(event::EnterAmount { 30 });
			vm.processEventGetInline(event::DispenseChange {});
			vm.processEventGetInline(event::EnterAmount { 30 });
			vm.processEventGetInline(event::EnterAmount { 30 });
		}
		vm.processEventGetInline(event::SelectItem { "Coke" });
		vm.processEventGetInline(event::DispenseChange {});
		vm.processEventGetInline(event::Reset { });
	}
}
// Register the function as a benchmark
BENCHMARK(GetIfInlineVersion);

static void GetIfHoldsAlternativeVersion(benchmark::State& state) {
	VendingMachine vm;
	// Code inside this loop is measured repeatedly
	for (auto _ : state) {
		vm.Reset();
		for (int i = 0; i < RUNS; ++i)
		{
			vm.processEventHoldAlternative(event::EnterAmount { 30 });
			vm.processEventHoldAlternative(event::DispenseChange {});
			vm.processEventHoldAlternative(event::EnterAmount { 30 });
			vm.processEventHoldAlternative(event::EnterAmount { 30 });
			vm.processEventHoldAlternative(event::DispenseChange {});
			vm.processEventHoldAlternative(event::EnterAmount { 30 });
			vm.processEventHoldAlternative(event::EnterAmount { 30 });
		}
		vm.processEventHoldAlternative(event::SelectItem { "Coke" });
		vm.processEventHoldAlternative(event::DispenseChange {});
		vm.processEventHoldAlternative(event::Reset { });
	}
}
// Register the function as a benchmark
BENCHMARK(GetIfHoldsAlternativeVersion);

static void EnumVersion(benchmark::State& state) {
	EnumVendingMachine vm;
	// Code inside this loop is measured repeatedly
	for (auto _ : state) {
		vm.Reset();
		for (int i = 0; i < RUNS; ++i)
		{
			vm.processEvent(EnumEvent::EnterAmount, "", 30);
			vm.processEvent(EnumEvent::DispenseChange);
			vm.processEvent(EnumEvent::EnterAmount, "", 30);
			vm.processEvent(EnumEvent::EnterAmount, "", 30);
			vm.processEvent(EnumEvent::DispenseChange);
			vm.processEvent(EnumEvent::EnterAmount, "", 30);
			vm.processEvent(EnumEvent::EnterAmount, "", 30);
		}
		vm.processEvent(EnumEvent::SelectItem, "Coke");
		vm.processEvent(EnumEvent::DispenseChange);
		vm.processEvent(EnumEvent::Reset);
	}
}
// Register the function as a benchmark
BENCHMARK(EnumVersion);


BENCHMARK_MAIN();