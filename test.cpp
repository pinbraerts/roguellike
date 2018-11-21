#include <iostream>
#include <map>
#include <fstream>
#include <vector>

#include "Idle.hpp"
#include "Replica.hpp"

#define add_factory(X) factories[ #X ] = (factory<IAction>)(factory<X>)std::make_unique<X>

int main() {
    Game g;
    g.add_factory(Idle);
    g.add_factory(Replica);
    std::ifstream f("map.txt");
    g.load(f);
    g.run();
    return 0;
}
