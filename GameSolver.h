//
// Created by Jay on 6/14/2023.
//

#ifndef TRADING_CARAVAN_SOLVER_GAMESOLVER_H
#define TRADING_CARAVAN_SOLVER_GAMESOLVER_H

#include "Game.h"
#include "enums.h"


class GameSolver {
public:
    explicit GameSolver (const int _limit, const town_options town=_Normalia, const bool merch_on = true, const bool witch_on = true, const vector<ga_data> &que={}) {
        game = new Game(merch_on, witch_on);
        Game::apply_town(start_player, town);
        limit = _limit;
        if (!que.empty()) {
            Game::auto_game_loop(start_player, que);
        }
        to_process.emplace_back(start_player);
    }

    void find_solve_loop();
    void find_solve_threads();

private:
    Game *game;
    Player start_player;
    vector<Player> to_process = {};
    vector<Player> finished = {};
    int limit;

    void sort_solves();
    vector<vector<Player>> progress_states(vector<Player> to_process);
    static bool day_in_array(const int (&list)[3], const int &value);
    void print_solves();
};


#endif //TRADING_CARAVAN_SOLVER_GAMESOLVER_H
