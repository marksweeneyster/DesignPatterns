#include <iostream>
#include <memory>
#include <string>
#include <vector>

/**
* From: https://www.modernescpp.com/index.php/type-erasure/
*/

class Object {

public:
  // templated ctor allows the Object type to store heterogeneous types
  template<typename T>
  explicit Object(T&& obj)
      : object(std::make_shared<Model<T>>(std::forward<T>(obj))) {}

  std::string getName() const { return object->getName(); }

  // "captures the concept of an [objects with `getName`]"
  struct Concept {
    virtual ~Concept()                  = default;
    virtual std::string getName() const = 0;
  };

  // "models the concrete types as instances of the concept"
  template<typename T>
  struct Model : Concept {
    explicit Model(const T& t) : object(t) {}
    std::string getName() const override { return object.getName(); }

  private:
    T object;
  };

  std::shared_ptr<const Concept> object;
};

void printName(const std::vector<Object>& vec) {// (7)
  for (const auto& v: vec) std::cout << v.getName() << '\n';
}

/**
 * Bar and Foo are two types with a common interface but don't inherit from
 * a base class.  We'd like to create heterogeneous collections.
 */

struct Bar {
  //Bar() {std::cout << " bar ctor\n";}
  std::string getName() const { return "Bar"; }
};

struct Foo {
  //Foo() {std::cout << " foo ctor\n";}
  std::string getName() const { return "Foo"; }
};

int main() {
  std::cout << '\n';

  std::vector<Object> vec{Object(Foo()), Object(Bar())};

  {
    Foo foo;
    Foo& ref = foo;
    vec.emplace_back(ref);

    const Bar bar;
    vec.emplace_back(bar);
  }

  printName(vec);

  std::cout << '\n';
  return 0;
}