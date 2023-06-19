#include <vector>
#include "enums.h"
#include "GameSolver.h"


int main() {
    std::vector<ga_data> que;
    que.emplace_back(_a_npc, _Trader);
    que.emplace_back(_a_buy, 9, 2, 0, 0, 0, 0);

    GameSolver solver(250000, _L_Exquise, true, true);
    solver.find_solve_loop();
    return 0;
}
