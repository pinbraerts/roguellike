#ifndef ROGUELLIKE_DIALOG_HPP
#define ROGUELLIKE_DIALOG_HPP

#include <set>
#include <sstream>
#include <iterator>

#include "Game.hpp"

struct Replica: IAction {
    std::vector<std::pair<std::string, action_ref>> answers;

    void describe(std::ostream& s) override {
        s << "talk";
    }

    void load(std::istream& s) override {
        while(s.peek() == ' ') {
            for(s.get(); s.peek() == ' '; s.get());
            auto& p = answers.emplace_back();
            std::getline(s, p.first, '|');
            s >> p.second;
            if(s.peek() == '\n')
                s.get();
        }
    }

    action_err step(Game& g) override {
        if(answers.empty())
            return g.action();

        std::cout << "\033[0;0H\033[2J";
        std::cout << answers.front().first << std::endl;
        if(answers.size() == 1) {
            std::cin.get();
            std::cin.get();
            return g.action();
        }

        std::cout << "0) quit" << std::endl;
        for(size_t i = 1; i < answers.size(); ++i)
            std::cout << i << ") " << answers[i].first << std::endl;

        size_t i;
        std::cin >> i;
        if(i >= answers.size())
            i = 0;
        return g.action(answers[i].second);
    }
};

#endif // !ROGUELLIKE_DIALOG_HPP
