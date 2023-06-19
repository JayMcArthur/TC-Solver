//
// Created by Jay on 6/14/2023.
//

#include <iostream>
#include <bits/stdc++.h>
#include "GameSolver.h"

void GameSolver::print_solves() {
    std::cout << "Printing Solves\n\n";
    sort(finished.begin(), finished.end(), greater <>());
    finished.resize(10);
    int place = 1;
    for (const auto& player : finished) {
        cout << "Place " << place << ": " << player.points << "\n";
        copy(player.actions.begin(),
                  player.actions.end(),
                  std::ostream_iterator<string>(std::cout, ", "));
        cout << "\n\n";
        place++;
    }
}

vector<vector<Player>> GameSolver::progress_states(vector<Player> &to_process_temp) {
    vector<vector<Player>> hold = {{}, {}};
    const int reprocess = 0;
    const int solved = 1;
    while (!to_process_temp.empty()) {
        Player current = to_process_temp.back();
        to_process_temp.pop_back();

        if (!current.day_start_skip) {
            Game::start_of_day(current);

            if (current.food < 0) {
                continue; // Starved to death
            }

            // Merchant Event
            if (game->merch_on && !current.merchant_happened and (current.call_merchant || day_in_array(game->merch_days, current.day))) {
                for (auto event : ALL_MERCH_OPTIONS) {
                    to_process_temp.emplace_back(current);
                    Game::event_merchant(to_process_temp.back(), event);
                    to_process_temp.back().day_start_skip = true;

                    //Player merch_event = current;
                    //Game::event_merchant(merch_event, event);
                    //merch_event.day_start_skip = true;
                    //to_process_temp.emplace_back(merch_event);
                }
            }

            // Witch Event
            if (game->witch_on && !current.witch_happened and (day_in_array(game->witch_days, current.day))) {
                for (auto event : ALL_WITCH_OPTIONS) {
                    Player witch_event = current;
                    Game::event_witch(witch_event, event);
                    witch_event.day_start_skip = true;
                    to_process_temp.emplace_back(witch_event);
                }
            }
        } else {
            current.day_start_skip = false;
        }

        int current_food_cost = Game::check_food_cost(current);

        // Buy Event
        if (current.day < current.max_day && !current.bought_last) {
            int days_left = current.max_day - current.day;
            int food_multi = days_left > 1 ? 2 : 1;
            int max_food = min(min(current.gold / current.item_shop[_Food][_buy], Game::check_weight_left(current) / current.item_shop[_Food][_weight]), (current_food_cost * days_left) + 2 * (days_left - 1) - current.food);
            int needed_food = max(0, food_multi * current_food_cost - current.food);

            for (; needed_food <= max_food; needed_food++) {
                Player to_buy_everything = current;
                Game::find_buy(to_buy_everything, needed_food);
                hold[reprocess].emplace_back(to_buy_everything);
                if (needed_food > 0 && current.gold - (needed_food * current.item_shop[_Food][_buy]) >= current.item_shop[_Spice][_buy]) {
                    Player to_buy_food = current;
                    Game::find_buy(to_buy_food, needed_food, true);
                    hold[reprocess].emplace_back(to_buy_food);
                }
            }
        }

        if (current.day == current.max_day || current.food >= current_food_cost) {
            // Sell Event
            // Check you have stuff to sell
            if (Game::check_has_items(current)) {
                Player sell = current;
                Game::event_sell_items(sell);
                if (sell.day == sell.max_day) {
                    hold[solved].emplace_back(sell);
                } else {
                    hold[reprocess].emplace_back(sell);
                }
            }

            // Trader Event
            // Check cost and you have new food cost
            if (current.gold >= current.npc_shop[_Trader][_cost] && (current.day == current.max_day || current.food >= current_food_cost + current.npc_shop[_Trader][_eat])) {
                Player buy_trader = current;
                Game::event_buy_npc(buy_trader, _Trader);
                if (buy_trader.day == buy_trader.max_day) {
                    hold[solved].emplace_back(buy_trader);
                } else {
                    hold[reprocess].emplace_back(buy_trader);
                }
            }

            // Camel Event
            // Check cost and you have new food cost
            if (current.gold >= current.npc_shop[_Camel][_cost] && (current.day == current.max_day || current.food >= current_food_cost + current.npc_shop[_Camel][_eat])) {
                Player buy_camel = current;
                Game::event_buy_npc(buy_camel, _Camel);
                if (buy_camel.day == buy_camel.max_day) {
                    hold[solved].emplace_back(buy_camel);
                } else {
                    hold[reprocess].emplace_back(buy_camel);
                }
            }

            // Interest Event
            // Lowest buy sell is *1.65 for 2 days or *1.284 per day
            if (current.interest_rate > 1.2 || current.day == current.max_day) {
                Player interest = current;
                Game::event_interest(interest);
                if (interest.day == interest.max_day) {
                    hold[solved].emplace_back(interest);
                } else {
                    hold[reprocess].emplace_back(interest);
                }
            }
        }

        if (hold[solved].size() >= 1000) {
            sort(hold[solved].begin(), hold[solved].end(), greater <>());
            hold[solved].resize(10);
        }
    }
    return hold;
}

bool GameSolver::day_in_array(const int (&list)[3], const int &value) {
    if (value == list[0]) {
        return true;
    }
    if (value == list[1]) {
        return true;
    }
    if (value == list[2]) {
        return true;
    }
    return false;
}

void GameSolver::find_solve_threads() {
}

void GameSolver::find_solve_loop() {
    int day = to_process[0].day;

    while (!to_process.empty()) {
        day += 1;
        cout << "Processing Day " << day << " - #" << to_process.size() << "\n";

        vector<vector<Player>> result = progress_states(to_process);
        to_process.clear();
        if (result[0].size() > limit) {
            sort(result[0].begin(), result[0].end(), greater <>());
            result[0].resize(limit);
        }
        to_process = std::move(result[0]);
        finished.insert(finished.end(),
                        std::make_move_iterator(result[1].begin()),
                        std::make_move_iterator(result[1].end()));
        result.clear();
    }
    print_solves();
}