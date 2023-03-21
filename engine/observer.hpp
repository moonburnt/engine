#pragma once

#include <string>
#include <vector>
#include <functional>

// TODO: allow for multiple types per notify_observers/update() pair to
// co-exist.
// E.g:
// class Bar: public Observer { void update(std::string) {} };
// class Baz: public Observer { void update(std::string) {} void update(int)};
// class Foo: public Observer;
// Foo.register_observer(Bar);
// Foo.register_observer(Baz);
// Foo.notify_observers("asda"); // Notify both
// Foo.notify_observers(42); // Notify only Baz

template <typename... Args> class Observer {
public:
    virtual void update(Args... args) = 0;
    virtual ~Observer() = default;
};

template <typename ...Args> class Subject {
private:
    std::vector<Observer<Args...>*> observers = {};

    bool _is_changed = false;

public:
    void register_observer(Observer<Args...>* o) {
        observers.push_back(o);
    }

    // Keep in mind that this does not delete the observer pointer!
    void remove_observer(Observer<Args...>* o) {
        typename std::vector<Observer<Args...>*>::iterator it;

        it = find_if(
            observers.begin(),
            observers.end(),
            [&o](Observer<Args...>* i) { return i == o; }
        );

        if (it != observers.end()) {
            observers.at(std::distance(observers.begin(), it)) = nullptr;
        }
    }

    void notify_observers(Args... args) {
        if (is_changed()) {
            for (auto i: observers) {
                if (i != nullptr) {
                    i->update(args...);
                }
            }
            _is_changed = false;
        }

        observers.erase(
            std::remove_if(
                observers.begin(),
                observers.end(),
                [](Observer<Args...>* i){ return i == nullptr; }
            ),
            observers.end()
        );
    }

    void set_changed() {
        _is_changed = true;
    }

    bool is_changed() {
        return _is_changed;
    }
};


// Same as observer, but in reverse - many notifiers inform a single listener


// tests
// class IntegersObserver: public Observer<int> {
// public:
//     void update(int i) override {
//         printf("%i\n", i);
//     }
// };


// class IntegersSubj: public Subject<int> {};

// int main() {

//     IntegersObserver* x = new IntegersObserver;

//     IntegersSubj s;

//     s.register_observer(x);
//     s.register_observer(new IntegersObserver);

//     s.notify_observers(65);

//     s.remove_observer(x);

//     s.notify_observers(48);

//     return 0;
// }
