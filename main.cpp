#include <vector>
#include "enums.h"
#include "GameSolver.h"


int main() {
    std::vector<ga_data> que;
    que.emplace_back(_a_npc, _Trader);
    que.emplace_back(_a_buy, 10, 0, 0, 0, 0, 0);
    que.emplace_back(_a_npc, _Trader);
    que.emplace_back(_a_npc, _Camel);
    //que.emplace_back(_a_buy, 18, 0, 0, 0, 0, 0);
    //que.emplace_back(_a_merchant, _Magic_Cleppsydra);
    //que.emplace_back(_a_npc, _Trader);
    //que.emplace_back(_a_npc, _Trader);
    //que.emplace_back(_a_buy, 24, 0, 0, 0, 0, 0);
    //que.emplace_back(_a_witch, _Midas_was_a_Trader);
    //que.emplace_back(_a_npc, _Camel);
    //que.emplace_back(_a_npc, _Camel);
    //que.emplace_back(_a_buy, 18, 1, 12, 0, 1, 1);
    //que.emplace_back(_a_sell);
    //que.emplace_back(_a_buy, 27, 0, 1, 0, 0, 5);
    //que.emplace_back(_a_sell);
    //que.emplace_back(_a_interest);

    GameSolver solver(100000, _Normalia, true, true, que);
    //solver.find_solve_loop();
    solver.find_solve_depth();
    return 0;
}