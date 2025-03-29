#include <iostream>
#include <ostream>
#include <utility>
#include <ranges>
#include <algorithm>
#include <random>

std::vector<std::string> split(std::string s, std::string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

class Event {
    friend std::ostream &operator<<(std::ostream &os, const Event &obj) {
        return os
               << "year: " << obj.year
               << " turn: " << obj.turn
               << " pool: " << obj.pool
               << " chance: " << obj.chance
               << " description: " << obj.description;
    }

public:
    Event(const int year, const int turn, std::string pool, const int chance, std::string description)
        : year(year), turn(turn), pool(std::move(pool)), chance(chance), description(std::move(description)) {
    }

    int year;
    int turn;
    std::string pool;
    int chance;
    std::string description;
};

template<typename T>
int d10(T &g) {
    std::uniform_int_distribution<> distrib(1, 10);
    return distrib(g);
}

template<typename T>
void moveRandomChits(std::vector<int> &source, std::vector<int> &target, int number_of_chits_to_move, T &g) {
    std::shuffle(source.begin(), source.end(), g);
    for (int i = 0; i < number_of_chits_to_move; i++) {
        target.push_back(source.back());
        source.pop_back();
    }
}

void printPools(int year, int turn, const std::vector<int> &entry_ge, const std::vector<int> &entry_ja) {
    const int ge = std::accumulate(entry_ge.begin(), entry_ge.end(), 0);
    const int ja = std::accumulate(entry_ja.begin(), entry_ja.end(), 0);
    std::cout << year << ',' << turn << ',' << entry_ge.size() << ',' << ge << ',' << entry_ja.size() << ',' << ja <<
            ',' << entry_ge.size() + entry_ja.size() << ',' << ja + ge << std::endl;
}

int main() {
    std::cout << "year,turn,countGe,totalGe,countJa,totalJa,count,total" << std::endl;
    const std::vector chits1939{
        1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5
    };
    const std::vector chits1940{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3};
    const std::vector chits1941{2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 6};

    std::vector<Event> events;
    for (std::string line; std::getline(std::cin, line);) {
        const std::vector values = split(line, ",");
        if (values.size() == 5) {
            int year = std::stoi(values[0]);
            int turn = std::stoi(values[1]);
            const std::string &pool = values[2];
            const int chance = std::stoi(values[3]);
            const std::string &description = values[4];
            events.emplace_back(year, turn, pool, chance, description);
        }
    }
    for (int i = 0; i < 10000; i++) {
        std::mt19937 g(i);
        int year = 1939;
        int turn = 5;
        std::vector<int> chits = chits1939;
        std::vector<int> entryJA;
        std::vector<int> entryGE;
        std::vector<int> tensionJA;
        std::vector<int> tensionGE;
        std::vector<int> borderGE;
        std::vector<int> borderJA;
        std::vector<int> borderRU;

        for (const auto &event: events) {
            // When we change year, we add the next year's chits.
            if (event.year != year) {
                year = event.year;
                if (year == 1940) {
                    chits.insert(chits.end(), chits1940.begin(), chits1940.end());
                } else if (year == 1941) {
                    chits.insert(chits.end(), chits1941.begin(), chits1941.end());
                }
            }

            if (event.turn != turn) {
                printPools(year, turn, entryGE, entryJA);
                turn = event.turn;
                // In M/J '41, we put back all the chits in the border pools.
                if (year == 1941 && event.turn == 3) {
                    chits.insert(chits.end(), borderGE.begin(), borderGE.end());
                    chits.insert(chits.end(), borderRU.begin(), borderRU.end());
                }
            }

            // One chit is moved for every 10.
            // A die is rolled for any remainder and a chit is moved if die <= this value.
            const int numberOfChitsToMove = abs(event.chance) / 10 + (d10(g) <= abs(event.chance) % 10 ? 1 : 0);
            std::vector<int> *target = nullptr;
            if (event.pool == "USEGe") {
                target = &entryGE;
            } else if (event.pool == "USEJa") {
                target = &entryJA;
            } else if (event.pool == "PactGe") {
                target = &borderGE;
            } else if (event.pool == "PactJa") {
                target = &borderJA;
            } else if (event.pool == "PactRu") {
                target = &borderRU;
            } else {
                std::cerr << "Unknown pool: " << event.pool << std::endl;
                return -1;
            }
            if (event.chance > 0) {
                // Positive events pull chits from the pool.
                moveRandomChits(chits, *target, numberOfChitsToMove, g);
            } else {
                moveRandomChits(entryGE, *target, numberOfChitsToMove, g);
            }
        }
        printPools(year, turn, entryGE, entryJA);
    }
    return 0;
}
