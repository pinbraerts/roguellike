#ifndef ROGUELLIKE_IDLE_HPP
#define ROGUELLIKE_IDLE_HPP

#include <iostream>
#include <memory>

#include "Action.hpp"
#include "Field.hpp"

#define UNIX_CONSOLE

struct CellDescription {
    std::string description;
    bool reachable;
};

enum class CellType {
    Rock, Road, Grass, Water
};

CellDescription c2d[] = {
        { "rock", false },
        { "road", true },
        { "grass", true },
        { "water", false }
};

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
    }
    return CellType::Grass;
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
    return 0;
}

struct Idle: IActivity {
    Field<CellType> map;
    size_t x, y;

    void load(std::istream& stream) override {
        size_t w, h;
        stream >> w >> h >> x >> y;
        map.resize(w, h);
        for(auto& cell: map.raw()) {
            char c;
            stream >> c;
            cell = char2c(c);
        }
    }

    CellType& at(size_t _x, size_t _y) {
        return map.at(_x, _y);
    }
    CellDescription& get(size_t _x, size_t _y) {
        return c2d[(size_t)at(_x, _y)];
    }
    CellDescription& get() {
        return get(x, y);
    }

    void inform() {
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
                std::cout << c2char(j) << ' ';
            std::cout << std::endl;
        }
        std::cout << "\033[" << (y + 1) << ';' << (x * 2 + 1) << 'H' << '@' << "\033[11;0H";
        std::cout << "You're on " << get(x, y).description << std::endl;
#endif
    }

    Step step(Game& g) override {
        inform();
        while(true) {
            char c;
#ifdef UNIX_CONSOLE
            std::cout << "\033[12;0H";
#endif
            std::cout << "What to do? ";
            std::cin >> c;
            switch(c) {
                case '?':
                    std::cout
                            << "? to show help" << std::endl
                            << "w to go up" << std::endl
                            << "a to go left" << std::endl
                            << "s to go down" << std::endl
                            << "d to go right" << std::endl
                            << "q to quit" << std::endl;
                    break;
                case 'w':
                    if(y >= 1 && get(x, y - 1).reachable) {
                        --y;
                        return Step::Next;
                    }
                    std::cout << "You can't go up" << std::endl;
                    break;
                case 's':
                    if(y < 9 && get(x, y + 1).reachable) {
                        ++y;
                        return Step::Next;
                    }
                    std::cout << "You can't go down" << std::endl;
                    break;
                case 'a':
                    if(x >= 1 && get(x - 1, y).reachable) {
                        --x;
                        return Step::Next;
                    }
                    std::cout << "You can't go left" << std::endl;
                    break;
                case 'd':
                    if(x < 9 && get(x + 1, y).reachable) {
                        ++x;
                        return Step::Next;
                    }
                    std::cout << "You can't go right" << std::endl;
                    break;
                case 'q':
                    return Step::Quit;
            }
        }
        return Step::Next;
    }
};

#endif // !ROGUELLIKE_IDLE_HPP
