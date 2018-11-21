#ifndef ROGUELLIKE_IDLE_HPP
#define ROGUELLIKE_IDLE_HPP

#include <iostream>
#include <memory>

#include "Game.hpp"
#include "Field.hpp"

#define UNIX_CONSOLE

struct CellDescription {
    std::string description;
    bool reachable;
};

enum class CellType {
    Rock, Road, Grass, Water
};

CellDescription __c2d[] = {
    { "rock", false },
    { "road", true },
    { "grass", true },
    { "water", false }
};
CellDescription& c2d(CellType& c) {
    return __c2d[(size_t)c];
}

CellType char2c(char c) {
    switch (c) {
    case '.':
        return CellType::Grass;
    case '^':
        return CellType::Rock;
    case '-':
        return CellType::Road;
    case '~':
        return CellType::Water;
    default:
        return CellType::Grass;
    }
}
char c2char(CellType c) {
    switch (c) {
    case CellType::Rock:
        return '^';
    case CellType::Road:
        return '-';
    case CellType::Grass:
        return '.';
    case CellType::Water:
        return '~';
    }
}

struct Cell {
    CellType type;
    unsigned action = 0;

    operator CellType&() {
        return type;
    }
};

struct Idle: IAction {
    Field<Cell> map;
    size_t x, y;

    void load(std::istream& stream) override {
        size_t w, h;
        stream >> w >> h >> x >> y;
        map.resize(w, h);
        for(auto& cell: map.raw()) {
            char c;
            stream >> c;
            cell.type = char2c(c);
            stream >> std::ws;
            if(isdigit(stream.peek()))
                stream >> cell.action;
        }
    }
    void describe(std::ostream& s) override {
        s << "open map";
    }

    auto& at(size_t _x, size_t _y) {
        return map.at(_x, _y);
    }
    CellDescription& get(size_t _x, size_t _y) {
        return c2d(at(_x, _y));
    }
    CellDescription& get() {
        return get(x, y);
    }

    void inform(Game& g) {
#ifndef UNIX_CONSOLE
        std::cout << "You're on " << get(x, y).description << std::endl;
        if(x >= 1)
            std::cout << "On the left is " << get(x - 1, y).description << std::endl;
        if(y >= 1)
            std::cout << "Upper is " << get(x, y - 1).description << std::endl;
        if(x < 9)
            std::cout << "On the right is " << get(x + 1, y).description << std::endl;
        if(y < 9)
            std::cout << "Down is " << get(x, y + 1).description << std::endl;
#else
        std::cout << "\033[0;0H\033[2J";
        for(auto& i: map) {
            for(auto& j: i)
                std::cout << c2char(j.type) << ' ';
            std::cout << std::endl;
        }
        std::cout << "\033[" << (y + 1) << ';' << (x * 2 + 1) << 'H' << '@' << "\033[11;0H";
        auto& cell = at(x, y);
        std::cout << "You're on " << c2d(cell).description;
        if(cell.action != 0)
            g.get(cell.action).describe(std::cout << ". Press e to ");
        std::cout << std::endl;
#endif
    }

    action_err step(Game& g) override {
        while(true) {
            inform(g);
            while(true) {
                char c;
#ifdef UNIX_CONSOLE
                std::cout << "\033[12;0H";
#endif
                std::cout << "What to do? ";
                std::cin >> c;
#ifdef UNIX_CONSOLE
                std::cout << "\033[2K";
#endif
                switch(c) {
                case '?':
                    std::cout
                            << "? to show help"     << std::endl
                            << "w to go up"         << std::endl
                            << "a to go left"       << std::endl
                            << "s to go down"       << std::endl
                            << "d to go right"      << std::endl
                            << "q to quit"          << std::endl
                            << "e to do action"     << std::endl;
                    break;
                case 'w':
                    if(y < 1 || !get(x, y - 1).reachable) {
                        std::cout << "You can't go up" << std::endl;
                        continue;
                    }
                    --y;
                    break;
                case 's':
                    if(y >= map.height() - 1 || !get(x, y + 1).reachable) {
                        std::cout << "You can't go down" << std::endl;
                        continue;
                    }
                    ++y;
                    break;
                case 'a':
                    if(x < 1 || !get(x - 1, y).reachable) {
                        std::cout << "You can't go left" << std::endl;
                        continue;
                    }
                    --x;
                    break;
                case 'd':
                    if(x >= map.width() - 1 || !get(x + 1, y).reachable) {
                        std::cout << "You can't go right" << std::endl;
                        continue;
                    }
                    ++x;
                    break;
                case 'e':
                    if(at(x, y).action == 0) {
                        std::cout << "There is no action here" << std::endl;
                        continue;
                    }
                    return g.action(at(x, y).action);
                case 'q':
                    return g.action();
                }
                break;
            }
        }
        return ok;
    }
};

#endif // !ROGUELLIKE_IDLE_HPP
