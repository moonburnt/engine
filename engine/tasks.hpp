#pragma once

#include <vector>

// Task manager, etc

class Task {
public:
    ~Task() = default;
    virtual void perform_task() {};
}


class TaskManager {
protected:
    std::vector<Task*> tasks;

public:
    void add_task(Task* task) {
        std::vector.push_back(task);
    }

    void perform_tasks() {
        if (!tasks.empty()) {
            for (auto i: tasks) {
                i->perform_task();
            }
            tasks.clear();
        }
    }
};

