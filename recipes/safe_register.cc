// safe_register.cc
// This example illustrates a widget manager, who manages some widgets. Each
// widget is owned by the thread created by `Widget::run()`. The manager only
// holds the weak references.
// The manager only exposes a `get()` method to the caller side for easy use.
// The lifetime of widgets are managed by the manager. Once a widget was
// expired, i.e. the task was completed, the widget would be removed from the
// cache of the manager.
#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
using namespace std;

class WidgetManager;

class Widget : public std::enable_shared_from_this<Widget> {
  friend std::ostream& operator<<(std::ostream& os, const Widget& w) {
    return (os << "Widget " << w.name_ << " @ "
               << static_cast<const void*>(&w));
  }

 public:
  Widget(const std::string& name, WidgetManager& manager)
      : name_(name), manager_(manager) {
    std::cout << "Construct Widget " << name << " @ " << this << std::endl;
  }

  ~Widget() {
    std::cout << "Destruct Widget " << name_ << " @ " << this << std::endl;
    unregister();
  }

  void run() {
    auto self = shared_from_this();
    std::thread t{[self] {
      num_tasks++;
      std::this_thread::sleep_for(std::chrono::seconds(1));
      std::cout << *self << " run" << std::endl;
      num_tasks--;
    }};
    t.detach();
  }

  static int wait(int max_millis = 3000) {
    while (num_tasks > 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      max_millis--;
    }
    return num_tasks;
  }

 private:
  std::string name_;
  WidgetManager& manager_;
  static std::atomic_int num_tasks;

  void unregister();
};

std::atomic_int Widget::num_tasks{0};

class WidgetManager {
  friend std::ostream& operator<<(std::ostream& os,
                                  const WidgetManager& manager) {
    std::lock_guard<std::mutex> lock(manager.mutex_);
    os << "WidgetManager";
    for (auto && [ name, weak_widget ] : manager.widgets_) {
      auto widget = weak_widget.lock();
      if (widget) {
        os << "\n  " << name << " => " << widget
           << " refcnt: " << widget.use_count();
      } else {
        os << "\n  " << name << " => (null)";
      }
    }
    return os;
  }

 public:
  auto get(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = widgets_.find(name);
    if (iter != widgets_.end()) {
      auto widget = iter->second.lock();
      if (widget) {
        return widget;
      } else {
        std::cout << "Widget of " << name
                  << " has been destructed, remove it from cache" << std::endl;
        widgets_.erase(iter);
      }
    }

    auto widget = std::make_shared<Widget>(name, *this);
    widgets_.emplace(name, widget);
    return widget;
  }

  auto remove(const std::string& name, Widget* widget_to_remove) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = widgets_.find(name);
    if (iter != widgets_.end()) {
      auto widget = iter->second.lock();
      if (widget && widget.get() == widget_to_remove) {
        widgets_.erase(name);
        std::cout << "Remove " << name << " " << (*widget_to_remove)
                  << std::endl;
      }
    }
  }

 private:
  mutable std::mutex mutex_;
  std::map<std::string, std::weak_ptr<Widget>> widgets_;
};

void Widget::unregister() { manager_.remove(name_, this); }

int main(int argc, char* argv[]) {
  WidgetManager manager;
  manager.get("A")->run();
  std::this_thread::sleep_for(std::chrono::milliseconds(600));
  manager.get("B")->run();
  std::cout << manager << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(600));
  std::cout << manager << std::endl;

  auto num_incompleted_tasks = Widget::wait();
  if (num_incompleted_tasks == 0) {
    std::cout << manager << std::endl;
  } else {
    std::cout << num_incompleted_tasks << " tasks are not done" << std::endl;
  }

  return 0;
}
