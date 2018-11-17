#include <iostream>
#include <map>
#include <fstream>
#include <vector>

#include "Idle.hpp"
#include "Dialog.hpp"

#define add_factory(X) factories[ #X ] = (std::unique_ptr<IActivity>(*)())(std::unique_ptr<X>(*)())std::make_unique<X>

int main() {
    Game g;
    g.add_factory(Idle);
    g.add_factory(Dialog);
    std::ifstream f("map.txt");
    g.load(f);
    g.run();
    return 0;
}
