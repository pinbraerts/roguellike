#ifndef ROGUELLIKE_SCENE_HPP
#define ROGUELLIKE_SCENE_HPP

#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <string>

enum class Step {
    Next, Pop, Push, Quit
};

struct Game;
struct IActivity {
    virtual Step step(Game& g) = 0;
    virtual void load(std::istream& s) = 0;
    virtual void describe(std::ostream& s) = 0;
};

struct Game {
    std::vector<std::reference_wrapper<IActivity>> activities;
    std::vector<std::unique_ptr<IActivity>> activity_pool;
    using factory = std::unique_ptr<IActivity>(*)();
    std::map<std::string, factory> factories;
    using action_ref = int;

    Step action(action_ref i) {
        if(i == 0) return Step::Pop;
        else if(i < 0) {
            activities.back() = *activity_pool[-i];
            return Step::Push;
        }
        else {
            activities.emplace_back(*activity_pool[i]);
            return Step::Push;
        }
    }

    void load(std::istream& s) {
        while(s && !s.eof()) {
            bool append = false;
            s >> std::ws;
            if(s.peek() == '+') {
                s.get();
                append = true;
            }
            auto x = load_activity(s);
            if(x == nullptr)
                return;
            auto& y = activity_pool.emplace_back(std::move(x));
            if(append)
                activities.emplace_back(*y);
            s >> std::ws;
        }
    }

    std::unique_ptr<IActivity> load_activity(std::istream& s) {
        std::string str;
        s >> str;
        auto i = factories.find(str);
        if(i == factories.end())
            return nullptr;
        auto x = (*i->second)();
        x->load(s);
        return x;
    }

    void run() {
        while(!activities.empty()) {
            switch(activities.back().get().step(*this)) {
            case Step::Quit:
                return;
            case Step::Pop:
                activities.pop_back();
                break;
            case Step::Push: case Step::Next:
                break;
            }
        }
    }
};

#endif // !ROGUELLIKE_SCENE_HPP
