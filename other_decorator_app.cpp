#include <format>
#include <iostream>
#include <type_traits>
#include <vector>

// Taken from "Hands on Design Patterns", CH 16, example 07 (Fedor Pikus, Packt)

template<typename Callable>
class DebugDecorator {
public:
  DebugDecorator(const Callable& c, const char* s) : c_(c), s_(s) {}
  template<typename... Args>
  auto operator()(Args&&... args) const {
    std::cout << std::format("Invoking {}\n", s_);
    auto res = c_(std::forward<Args>(args)...);
    std::cout << std::format("Result: {}\n", res);
    return res;
  }

private:
  const Callable& c_;
  const std::string s_;
};

template<typename Callable>
auto decorate_debug(const Callable& c, const char* s) {
  return DebugDecorator<Callable>(c, s);
}

struct R {
  int value() const { return rand(); }
};

int g(int i, int j) { return i - j; }

struct S {
  double operator()() const { return double(rand() + 1) / double(rand() + 1); }
};

double x = 0;
double& fx() { return x; }
const double& cfx() { return x; }

int main() {
  auto f1 = decorate_debug([](int i) { return i; }, "i->i");
  f1(5);
  std::cout << '\n';

  auto f2 = decorate_debug([](int i, int j) { return i + j; }, "i+j");
  f2(5, 3);
  std::cout << '\n';

  auto g1 = decorate_debug(g, "g()");
  g1(5, 2);
  std::cout << '\n';

  S s;
  auto s1 = decorate_debug(s, "rand/rand");
  s1();
  std::cout << '\n';
  s1();
  std::cout << '\n';

  R r;
  auto f0 = decorate_debug([&]() { return r.value(); }, "rand");
  f0();
  std::cout << '\n';
  f0();
  std::cout << '\n';

  auto fx1 = decorate_debug(fx, "fx()");
  fx()     = 5;
  std::cout << "x=" << x << std::endl;
  //fx1() = 6; cout << "x=" << x << endl; // Needs more work to preserve the &

  auto cfx1 = decorate_debug(cfx, "cfx()");
  cfx1();

  return 0;
}
