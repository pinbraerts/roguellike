#ifndef ROGUELLIKE_SCENE_HPP
#define ROGUELLIKE_SCENE_HPP

#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <string>

using namespace std::literals;

struct Game;

using action_ref = int;

using action_err = std::string;
action_err ok;
action_err invalid_index = "Invalid index";

template<class T> using factory = std::unique_ptr<T>(*)();

struct IAction {
    virtual action_err step(Game& g) = 0;
    virtual void load(std::istream& s) = 0;
    virtual void describe(std::ostream& s) = 0;
    virtual ~IAction() = default;
};

struct Game {
    std::vector<size_t> activities;
    std::vector<std::unique_ptr<IAction>> pool;
    std::map<std::string, factory<IAction>> factories;

    IAction& get(action_ref i) {
        return *pool[activities.back() + abs(i)];
    }
    IAction* getp(action_ref i) {
        auto x = activities.back() + abs(i);
        if(x >= pool.size())
            return nullptr;
        return &get(i);
    }

    action_err action(action_ref i = 0) {
        if(abs(i) >= pool.size())
            return invalid_index;
        if(i == 0) {
            activities.pop_back();
        }
        else if(i < 0)
            activities.back() -= i;
        else
            activities.emplace_back(activities.back() + i);
        return ok;
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
            auto& y = pool.emplace_back(std::move(x));
            if(append)
                activities.emplace_back(pool.size() - 1);
            s >> std::ws;
        }
    }

    std::unique_ptr<IAction> load_activity(std::istream& s) {
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
            action_err err = pool[activities.back()]->step(*this);
            if(err != ok) {
                std::cerr << err << std::endl;
                break;
            }
        }
    }
};

#endif // !ROGUELLIKE_SCENE_HPP
