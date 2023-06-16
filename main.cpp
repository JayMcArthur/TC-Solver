#include <vector>
#include "enums.h"
#include "GameSolver.h"


int main() {
    std::vector<ga_data> que;
    que.emplace_back(_a_npc, _Trader);
    que.emplace_back(_a_npc, _Trader);
    que.emplace_back(_a_npc, _Trader);
    que.emplace_back(_a_npc, _Camel);

    GameSolver solver(10000, _Normalia, true, true);
    solver.find_solve_loop();
    return 0;
}
