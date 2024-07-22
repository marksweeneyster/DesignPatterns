#include <iostream>
#include <memory>

/**
 * Good comment from the Refactoring Guru:
 *
 *   "A lot of modern programming languages have functional type support that
 *    lets you implement different versions of an algorithm inside a set of
 *    anonymous functions. [aka lambdas]
 *    Then you could use these functions exactly as you’d have used the
 *    strategy objects, but without bloating your code with
 *    extra classes and interfaces."
 *
 * and:
 *    "Strategy usually describes different ways of doing the same thing,
 *    letting you swap these algorithms within a single context class."
 */

class Strategy {
public:
  virtual ~Strategy()                = default;
  virtual void algorithm_interface() = 0;
};
using StratPtr = std::unique_ptr<Strategy>;

class Context {
public:
  explicit Context(StratPtr&& strat) : strategy(std::move(strat)) {}

  void do_something() { strategy->algorithm_interface(); }

  // Being able to change the strategy would not be possible if
  // context were templated on the concrete strategy types.
  // Point for dynamic dispatch.
  void set_strategy(StratPtr&& strat) { strategy = std::move(strat); }

private:
  StratPtr strategy;
};

// I'm intentionally not using "override" on the virtual methods below to
// remind (and convince) myself that it's not required.

class ConcreteStrategyFu : public Strategy {
public:
  void algorithm_interface() { std::cout << "Fu Strategy\n"; }
};

class ConcreteStrategyWu : public Strategy {
public:
  void algorithm_interface() { std::cout << "You have beef with me?!?\n"; }
};

void client_code() {
  Context context(std::make_unique<ConcreteStrategyFu>());
  context.do_something();

  StratPtr strat_ptr = std::make_unique<ConcreteStrategyWu>();
  // I like how the universal reference (&&) forces users to "move" the object.
  // This makes it quite clear what is happening.
  context.set_strategy(std::move(strat_ptr));
  context.do_something();

  strat_ptr = std::make_unique<ConcreteStrategyFu>();
  context.set_strategy(std::move(strat_ptr));
  context.do_something();
}

int main() { client_code(); }