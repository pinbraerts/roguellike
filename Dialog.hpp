#ifndef ROGUELLIKE_DIALOG_HPP
#define ROGUELLIKE_DIALOG_HPP

#include <set>

#include "Action.hpp"

struct Dialog: IActivity {
    std::string replica;
    std::vector<std::pair<std::string, unsigned>> answers;

    void describe(std::ostream& s) override {
        s << "talk";
    }

    void load(std::istream& s) override {
        s >> std::ws;
        std::getline(s, replica);
        std::set<std::reference_wrapper<unsigned>> waits_for_assign;

        answers.emplace_back();
        while(s && !s.eof() && s.peek() == ' ') {
            s.get();
            if(s.peek() == ' ') {
                unsigned id;
                s >> id;
                for(auto x: waits_for_assign)
                    x.get() = id;
                waits_for_assign.clear();
            }
            else if(s.peek() == ':') {
                std::getline(std::cin, answers[0].first);
                waits_for_assign.emplace(answers[0].second);
            }
            else {
                std::string str;
                std::getline(s, str);
                waits_for_assign.emplace(answers.emplace_back(std::move(str), 0).second);
            }
        }
        for(auto x: waits_for_assign)
            x.get() = 0;
    }

    Step step(Game& g) override {
        std::cout << "\033[0;0H\033[2J";
        std::cout << replica << std::endl;
        if(answers.empty()) {
            std::cin.get();
            std::cin.get();
            return Step::Pop;
        }
        for(size_t i = 1; i < answers.size(); ++i) {
            std::cout << i << ") " << answers[i].first << std::endl;
        }
        std::cout << "0) quit" << std::endl;
        size_t i;
        std::cin >> i;

        if(i >= answers.size())
            i = 0;
        return g.action(-answers[i].second);
    }
};

#endif // !ROGUELLIKE_DIALOG_HPP
