#include <format>
#include <iostream>
#include <type_traits>
#include <vector>


// Base class has a pure virtual function for cloning
class AbstractShape {
public:
  virtual ~AbstractShape()                                           = default;
  [[nodiscard]] virtual std::unique_ptr<AbstractShape> clone() const = 0;
  virtual void describe() const                                      = 0;
};

// This CRTP class implements clone() for Derived
template<typename Derived>
class Shape : public AbstractShape {
public:
  [[nodiscard]] std::unique_ptr<AbstractShape> clone() const override {
    return std::make_unique<Derived>(static_cast<const Derived&>(*this));
  }


protected:
  // We make clear Shape class needs to be inherited
  Shape()                 = default;
  Shape(const Shape&)     = default;
  Shape(Shape&&) noexcept = default;
};

// Every derived class inherits from CRTP class instead of abstract class

class Square : public Shape<Square> {
public:
  void describe() const override { std::cout << "I am a Square\n"; }

private:
  static constexpr int d = 1;
};

class Circle : public Shape<Circle> {
public:
  void describe() const override { std::cout << "I am a Circle\n"; }

private:
  static constexpr int d = 0;
};

void crtp_shape_client() {
  std::unique_ptr<AbstractShape> square = std::make_unique<Square>();
  std::unique_ptr<AbstractShape> circle = std::make_unique<Circle>();

  std::vector<std::unique_ptr<AbstractShape>> vec;

  vec.push_back(std::move(square));
  vec.push_back(std::move(circle));
  for (const auto& as: vec) {
    as->describe();
  }

  auto sq2   = vec[0]->clone();
  auto circ2 = vec[1]->clone();

  square = std::make_unique<Square>();

  auto s3 = square->clone();
  s3->describe();

  auto is_circle = [](AbstractShape* shape, std::string obj_name) {
    std::cout << "\'" << obj_name << "\' is "
              << (dynamic_cast<Circle*>(shape) ? "" : "NOT") << " a circle\n";
  };

  auto is_square = [](AbstractShape* shape, std::string obj_name) {
    std::cout << "\'" << obj_name << "\' is "
              << (dynamic_cast<Square*>(shape) ? "" : "NOT") << " a square\n";
  };

  is_circle(s3.get(), "s3");
  is_square(s3.get(), "s3");

  is_circle(circ2.get(), "circ2");
  is_square(circ2.get(), "circ2");

  is_circle(nullptr, "nullptr");

  circ2 = std::make_unique<Square>();
  circ2->describe();
}

// "C++ Templates, 2nd ed." (Vandevoorde, Josuttis, Gregor)
template<typename CountedType>
class ObjectCounter {
private:
  inline static std::size_t count = 0;

protected:
  ObjectCounter() noexcept { ++count; }
  ObjectCounter(const ObjectCounter<CountedType>&) noexcept { ++count; }
  ObjectCounter(ObjectCounter<CountedType>&&) noexcept { ++count; }

  ~ObjectCounter() { --count; }

public:
  static std::size_t live() { return count; }
};

struct Pod : public ObjectCounter<Pod> {
  Pod() = default;
  explicit Pod(int d) : data(d) {}
  int data;
};

void crtp_counter_client() {
  Pod some_pod(9);
  std::vector<Pod> vec(11);
  std::cout << std::format("Pod count:\t {}\n", Pod::live());
  std::cout << std::format("Pod default data: {}\n", vec[0].data);
}

int main() {
  std::cout << std::format("{}, {}!\n", "Hello", "World");

  crtp_shape_client();

  std::cout << std::format("\n-------------------------------------\n");

  crtp_counter_client();

  return 0;
}
