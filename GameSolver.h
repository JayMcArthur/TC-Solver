//
// Created by Jay on 6/14/2023.
//

#ifndef TRADING_CARAVAN_SOLVER_GAMESOLVER_H
#define TRADING_CARAVAN_SOLVER_GAMESOLVER_H

#include <set>
#include "Game.h"
#include "enums.h"


class GameSolver {
public:
    explicit GameSolver (const int _limit, const town_options town=_Normalia, const bool _merch_on = true, const bool _witch_on = true, const vector<ga_data> &que={}) {
        limit = _limit;
        game = new Game();
        Game::apply_town(start_player, town);
        if (!que.empty()) {
            Game::auto_game_loop(start_player, que);
        }
        to_process.emplace_back(start_player);
        merch_on = _merch_on;
        witch_on = _witch_on;
    }
    ~GameSolver() {
        delete game;
    }
    void find_solve_loop();
    void find_solve_threads();
    void find_solve_depth();

private:
    Game *game;
    Player start_player;
    vector<Player> to_process = {};
    set<Player> finished;
    int limit;
    int processed;
    bool merch_on;
    const int merch_days[3] = {5, 6, 7};
    bool witch_on;
    const int witch_days[3] = {9, 10, 11};

    vector<vector<Player>> progress_states(vector<Player> &to_process);
    static bool day_in_array(const int (&list)[3], const int &value);
    void print_solves(const int &amount);
    void add_finished(const Player &save_file);
};


#endif //TRADING_CARAVAN_SOLVER_GAMESOLVER_H
