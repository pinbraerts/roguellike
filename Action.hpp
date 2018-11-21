#ifndef ROGUELLIKE_ACTION_HPP
#define ROGUELLIKE_ACTION_HPP

#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <string>

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

#endif // !ROGUELLIKE_ACTION_HPP
