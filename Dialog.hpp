#ifndef ROGUELLIKE_DIALOG_HPP
#define ROGUELLIKE_DIALOG_HPP

#include <set>

#include "Action.hpp"

struct Dialog: IActivity {
    std::string replica;
    std::map<std::string, size_t> answers;

    void load(std::istream& s) override {
        s >> std::ws;
        std::getline(s, replica);
        std::set<std::reference_wrapper<size_t>> waits_for_assign;

        while(s.peek() == '\t') {
            s.get();
            if(s.peek() == '\t') {
                size_t id;
                s >> id;
                for(auto x: waits_for_assign)
                    x.get() = id;
                waits_for_assign.clear();
            }
            std::string str;
            std::getline(s, str);
            waits_for_assign.emplace(answers[str]);
        }
        for(auto x: waits_for_assign)
            x.get() = 0;
    }

    Step step(Game& g) override {
        std::cout << replica << std::endl;
        if(answers.empty())
            return Step::Pop;
        for(auto& i: answers)
            std::cout << ' ' << i.first << std::endl;
        std::string s;
        std::cin >> s;
        auto i = answers.find(s);
        if(i == answers.end()) {
            i = answers.find(""); // default answer
            if(i == answers.end())
                return Step::Quit;
        }
        if(i->second == 0)
            return Step::Push;
        g.activities.back() = *g.activity_pool[i->second];
        return Step::Push;
    }
};

#endif // !ROGUELLIKE_DIALOG_HPP
