#include <array>
#include <iostream>
#include <variant>
#include <vector>

class FloppyDisk;
class Card;
class Chassis;
class Bus;

class EquipmentVisitor {
public:
  virtual ~EquipmentVisitor() = default;

  virtual void VisitFloppyDisk(FloppyDisk*) = 0;
  virtual void VisitCard(Card*)             = 0;
  virtual void VisitChassis(Chassis*)       = 0;
  virtual void VisitBus(Bus*)               = 0;
};

class Equipment {
public:
  virtual ~Equipment() = default;

  const char* Name() const { return name; };

  virtual int Power() const         = 0;
  virtual int NetPrice() const      = 0;
  virtual int DiscountPrice() const = 0;

  virtual void Accept(EquipmentVisitor&) = 0;

protected:
  Equipment(const char* n) : name(n) {}

private:
  const char* name;
};

class FloppyDisk : public Equipment {
public:
  FloppyDisk() : Equipment("FloppyDisk") {}
  void Accept(EquipmentVisitor& visitor) override {
    visitor.VisitFloppyDisk(this);
  }

  int Power() const override { return 7; }
  int NetPrice() const override { return 7; }
  int DiscountPrice() const override { return 7; }
};

class Card : public Equipment {
public:
  Card() : Equipment("Card") {}
  void Accept(EquipmentVisitor& visitor) override { visitor.VisitCard(this); }

  int Power() const override { return 9; }
  int NetPrice() const override { return 9; }
  int DiscountPrice() const override { return 9; }
};

class Chassis : public Equipment {
public:
  Chassis() : Equipment("Chassis") {}
  void Accept(EquipmentVisitor& visitor) override {
    visitor.VisitChassis(this);
  }

  int Power() const override { return 11; }
  int NetPrice() const override { return 11; }
  int DiscountPrice() const override { return 11; }
};

class Bus : public Equipment {
public:
  Bus() : Equipment("Bus") {}
  void Accept(EquipmentVisitor& visitor) override { visitor.VisitBus(this); }

  int Power() const override { return 13; }
  int NetPrice() const override { return 13; }
  int DiscountPrice() const override { return 13; }
};

class PricingVisitor : public EquipmentVisitor {
public:
  void VisitFloppyDisk(FloppyDisk*) override {}
  void VisitCard(Card*) override {};
  void VisitChassis(Chassis*) override {}
  void VisitBus(Bus*) override {}
};

class InventoryVisitor : public EquipmentVisitor {
public:
  InventoryVisitor()
      : disk_count(0), card_count(0), chassis_count(0), bus_count(0) {}

  void VisitFloppyDisk(FloppyDisk*) override { ++disk_count; }
  void VisitCard(Card*) override { ++card_count; };
  void VisitChassis(Chassis*) override { ++chassis_count; }
  void VisitBus(Bus*) override { ++bus_count; }

private:
  int disk_count;
  int card_count;
  int chassis_count;
  int bus_count;
};

void client_code() {
  std::vector<EquipmentVisitor*> visitors = {new InventoryVisitor(),
                                             new PricingVisitor()};

  std::vector<Equipment*> equipment = {new Bus(), new Card(), new Chassis(),
                                       new FloppyDisk()};


  for (auto& visitor: visitors) {
    for (auto& eqp: equipment) {
      //visitor->VisitBus(eqp);
    }
  }

  for (auto& eqp: equipment) {
    delete eqp;
  }

  for (auto& visitor: visitors) {
    delete visitor;
  }
}

using var_eqp_t = std::variant<FloppyDisk, Card, Chassis, Bus>;

/**
 * This version has the advantage of being able to aggregate info for elements
 * in the object structure.
 *
 * The function overloading method (further below) can't do this
 */
void VisitInventory(const var_eqp_t& equipment) {
  static int disk_count    = 0;
  static int card_count    = 0;
  static int chassis_count = 0;
  static int bus_count     = 0;

  if (std::holds_alternative<FloppyDisk>(equipment)) {
    auto fd = std::get<FloppyDisk>(equipment);
    std::cout << fd.Name() << '\n';
    ++disk_count;
  } else if (std::holds_alternative<Card>(equipment)) {
    auto card = std::get<Card>(equipment);
    std::cout << card.Name() << '\n';
    ++card_count;
  } else if (std::holds_alternative<Chassis>(equipment)) {
    auto chassis = get<Chassis>(equipment);
    std::cout << chassis.Name() << '\n';
    ++chassis_count;
  } else if (std::holds_alternative<Bus>(equipment)) {
    auto bus = get<Bus>(equipment);
    std::cout << bus.Name() << '\n';
    ++bus_count;
  }
}

// above stuff in a Functor
class VIFn {
public:
  VIFn() : disk_count(0), card_count(0), chassis_count(0), bus_count(0) {}

  void operator()(const var_eqp_t& equipment) {
    if (std::holds_alternative<FloppyDisk>(equipment)) {
      auto fd = std::get<FloppyDisk>(equipment);
      std::cout << "VIFn: " << fd.Name() << '\n';
      ++disk_count;
    } else if (std::holds_alternative<Card>(equipment)) {
      auto card = std::get<Card>(equipment);
      std::cout << "VIFn: " << card.Name() << '\n';
      ++card_count;
    } else if (std::holds_alternative<Chassis>(equipment)) {
      auto chassis = get<Chassis>(equipment);
      std::cout << "VIFn: " << chassis.Name() << '\n';
      ++chassis_count;
    } else if (std::holds_alternative<Bus>(equipment)) {
      auto bus = get<Bus>(equipment);
      std::cout << "VIFn: " << bus.Name() << '\n';
      ++bus_count;
    }
  }

private:
  int disk_count;
  int card_count;
  int chassis_count;
  int bus_count;
};

void VizInv(const FloppyDisk& fd) {
  static int disk_count = 0;
  ++disk_count;
}

void VizInv(const Card& card) {
  static int card_count = 0;
  ++card_count;
}

void VizInv(const Chassis& chassis) {
  static int chassis_count = 0;
  ++chassis_count;
}

void VizInv(const Bus& bus) {
  static int bus_count = 0;
  ++bus_count;
}

class VizInvFn {
public:
  VizInvFn() : disk_count(0), card_count(0), chassis_count(0), bus_count(0) {}

  void operator()(const FloppyDisk& fd) {
    std::cout << "VizInvFn: " << fd.Name() << '\n';
    ++disk_count;
  }

  void operator()(const Card& card) {
    std::cout << "VizInvFn: " << card.Name() << '\n';
    ++card_count;
  }

  void operator()(const Chassis& chassis) {
    std::cout << "VizInvFn: " << chassis.Name() << '\n';
    ++chassis_count;
  }

  void operator()(const Bus& bus) {
    std::cout << "VizInvFn: " << bus.Name() << '\n';
    ++bus_count;
  }

private:
  int disk_count;
  int card_count;
  int chassis_count;
  int bus_count;
};

class VizPowFn {
public:
  VizPowFn() : total_power(0) {}

  void operator()(const FloppyDisk& fd) { total_power += fd.Power(); }

  void operator()(const Card& card) { total_power += card.Power(); }

  void operator()(const Chassis& chassis) { total_power += chassis.Power(); }

  void operator()(const Bus& bus) { total_power += bus.Power(); }

  int get_power() const { return total_power; }

private:
  int total_power;
};

int main() {
  PricingVisitor pv;
  InventoryVisitor iv;

  Bus bus;
  Card card;
  FloppyDisk fdisk;
  Chassis chassis;

  Equipment* eq_arr[] = {&bus, &card, &fdisk, &chassis};

  for (auto& eqp: eq_arr) {
    eqp->Accept(iv);
  }

  for (auto& eqp: eq_arr) {
    eqp->Accept(pv);
  }

  std::array<var_eqp_t, 4> equipment = {bus, card, fdisk, chassis};

  // no need for std::visit
  for (const auto& eqp: equipment) {
    VisitInventory(eqp);
  }

  std::cout << '\n';

  VIFn vi_fn;
  for (const auto& eqp: equipment) {
    vi_fn(eqp);
  }

  for (const auto& eqp: equipment) {
    std::visit([](auto&& arg) { VizInv(arg); }, eqp);
  }

  std::cout << '\n';

  VizInvFn viz_fn;
  for (const auto& eqp: equipment) {
    std::visit([&viz_fn](auto&& arg) { viz_fn(arg); }, eqp);
  }

  VizPowFn viz_pow_fn;
  for (const auto& eqp: equipment) {
    std::visit([&viz_pow_fn](auto&& arg) { viz_pow_fn(arg); }, eqp);
  }
  std::cout << "\nTotal Power: " << viz_pow_fn.get_power() << "\n";

  std::cout << "\nle fin\n";
}