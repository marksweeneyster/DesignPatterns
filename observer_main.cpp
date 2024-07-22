#include <chrono>
#include <ctime>
#include <format>
#include <iostream>
#include <list>
#include <memory>
#include <thread>
#include <utility>

/**
 * Based on GoF Observer sample code.
 * Observer pattern also known as Publish-Subscribe
 */

class Subject;
using SubjectSPtr = std::shared_ptr<Subject>;

class ClockTimer;
using ClockTimerSPtr = std::shared_ptr<ClockTimer>;

class Observer {
public:
  virtual ~Observer()                                = default;
  virtual void Update(SubjectSPtr theChangedSubject) = 0;

protected:
  Observer() = default;
};

// Subject aka "observed"
class Subject : public std::enable_shared_from_this<Subject> {
public:
  virtual ~Subject() = default;

  /*
     * Attach and Detach are only called in ctors and dtors passing in "this". Raw pointers seem OK
     */
  virtual void Attach(Observer* o) { observers.push_back(o); }
  virtual void Detach(Observer* o) { observers.remove(o); }
  /**
   * Notify, aka publish,
   */
  virtual void Notify() {
    for (auto& o: observers) {
      o->Update(shared_from_this());
    }
  }

protected:
  Subject() = default;

private:
  std::list<Observer*> observers;
};

class ClockTimer : public Subject {
public:
  ClockTimer() : nowTime(nullptr) {
    Tick();// do this to ensure nowTime is not null
  }

  virtual int GetHour() { return nowTime->tm_hour; }
  virtual int GetMinute() { return nowTime->tm_min; }
  virtual int GetSecond() { return nowTime->tm_sec; }

  void Tick() {
    auto now           = time(nullptr);
    const auto now_now = std::chrono::system_clock::now();
    auto secs          = time_point_cast<std::chrono::hours>(now_now);
    std::tm t{};
    nowTime = localtime(&now);
    Notify();
  }

private:
  // raw pointer, yucky
  tm* nowTime;
};

class DigitalClock : public Observer {
public:
  explicit DigitalClock(ClockTimerSPtr s) {
    subject = std::move(s);
    subject->Attach(this);
  }
  ~DigitalClock() override { subject->Detach(this); }

  void Update(SubjectSPtr theChangedSubject) override {
    if (theChangedSubject == subject) {
      Draw();
    }
  }

  void Draw() {

    auto h = subject->GetHour();
    auto m = subject->GetMinute();
    auto s = subject->GetSecond();

    std::cout << std::format("h:m:s - {:#02d}:{:#02d}:{:#02d}\n", h, m, s);
  }

private:
  ClockTimerSPtr subject;
};

int main() {
  std::cout << "Hello, World!" << std::endl;

  auto clockTimer = std::make_shared<ClockTimer>();

  // The observer is instantiated with a subject instance
  // digitalClock is subscribing to what clockTimer publishes
  auto digitalClock = std::make_shared<DigitalClock>(clockTimer);

  for (int ii = 0; ii < 100; ++ii) {
    clockTimer->Tick();
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  return 0;
}
