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
};

struct Game {
    std::vector<std::reference_wrapper<IActivity>> activities;
    std::vector<std::unique_ptr<IActivity>> activity_pool;
    using factory = std::unique_ptr<IActivity>(*)();
    std::map<std::string, factory> factories;

    void load(std::istream& s) {
        while(s) {
            bool append = false;
            s >> std::ws;
            if(s.peek() == '+') {
                s.get();
                append = true;
            }
            auto x = load_activity(s);
            if(x == nullptr)
                return;
            if(append)
                activities.emplace_back(*x);
        }
    }

    std::unique_ptr<IActivity> load_activity(std::istream& s) {
        std::string str;
        s >> str;
        auto i = factories.find(str);
        if(i == factories.end())
            return nullptr;
        auto& x = activity_pool.emplace_back((*i->second)());
        x->load(s);
        return std::move(x);
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
