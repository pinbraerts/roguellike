#ifndef ROGUELLIKE_DIALOG_HPP
#define ROGUELLIKE_DIALOG_HPP

#include <set>

#include "Action.hpp"

struct Dialog: IActivity {
    std::string replica;
    std::map<std::string, unsigned> answers;

    void describe(std::ostream& s) override {
        s << "talk";
    }

    void load(std::istream& s) override {
        s >> std::ws;
        std::getline(s, replica);
        std::set<std::reference_wrapper<unsigned>> waits_for_assign;

        while(s && !s.eof() && s.peek() == ' ') {
            s.get();
            if(s.peek() == ' ') {
                unsigned id;
                s >> id;
                for(auto x: waits_for_assign)
                    x.get() = id;
                waits_for_assign.clear();
            }
            else {
                std::string str;
                std::getline(s, str);
                waits_for_assign.emplace(answers[str]);
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
        for(auto& i: answers) {
            std::cout << ' ' << i.first << std::endl;
        }
        std::string s;
        std::cin >> s;
        auto i = answers.find(s);
        if(i == answers.end()) {
            i = answers.find(""); // default answer
            if(i == answers.end())
                return Step::Quit;
        }
        return g.action(-i->second);
    }
};

#endif // !ROGUELLIKE_DIALOG_HPP
