//
// Created by Jay on 6/14/2023.
//

#include <iostream>
#include <bits/stdc++.h>
#include "GameSolver.h"

void GameSolver::print_solves(const int &amount) {
    if (amount != 1) [[unlikely]] {
        std::cout << "Printing Solves\n";
    }
    vector<Player> temp_places;
    int place = 1;
    for (auto p = finished.rbegin(); p != finished.rend(); p++) {
        cout << "Place " << place << ": " << p->points << "\n";
        for (const auto & action : p->actions) {
            if (&action != &p->actions.front()) [[likely]] {
                std::cout << ", "<<  action ;
            } else [[unlikely]]{
                std::cout <<  action;
            }
        }
        std::cout << "\n";
        ++place;
        if (place > amount) {
            std::cout << "\n";
            break;
        }
    }
}

void GameSolver::add_finished(const Player &save_file) {
    finished.insert(save_file);
    if (finished.size() > 10) [[likely]] {
        ++processed;
        finished.erase(finished.begin());
    }
}

vector<vector<Player>> GameSolver::progress_states(vector<Player> &to_process_temp) {
    vector<vector<Player>> hold = {{}, {}};
    const int reprocess = 0;
    const int solved = 1;
    while (!to_process_temp.empty()) {
        Player current = to_process_temp.back();
        to_process_temp.pop_back();

        if (!current.day_start_skip) [[unlikely]] {
            Game::start_of_day(current);

            if (current.food < 0) {
                continue; // Starved to death
            }

            // Merchant Event
            if (merch_on && !current.merchant_happened and (current.call_merchant || day_in_array(merch_days, current.day))) {
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
            if (witch_on && !current.witch_happened and (day_in_array(witch_days, current.day))) {
                for (auto event : ALL_WITCH_OPTIONS) {
                    Player witch_event = current;
                    Game::event_witch(witch_event, event);
                    witch_event.day_start_skip = true;
                    to_process_temp.emplace_back(witch_event);
                }
            }
        } else [[likely]] {
            current.day_start_skip = false;
        }

        int current_food_cost = current.food_consumption;

        // Buy Event
        if (current.day < current.max_day && !current.bought_last) {
            int days_left = current.max_day - current.day;
            int food_multi = days_left > 1 ? 2 : 1;
            int max_food = min(min(current.gold / max(current.item_shop[_Food][_buy],1), Game::check_weight_left(current) / max(current.item_shop[_Food][_weight], 1)), (current_food_cost * days_left) + 2 * (days_left - 1) - current.food);
            int needed_food = max(0, food_multi * current_food_cost - current.food);

            for (; needed_food <= max_food; ++needed_food) {
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

void GameSolver::find_solve_depth() {
    int round_num = 0 ;
    while (!to_process.empty()) {
        Player current = to_process.back();
        to_process.pop_back();

        if (!current.day_start_skip) {
            Game::start_of_day(current);

            if (current.food < 0) {
                continue; // Starved to death
            }

            // Merchant Event
            if (merch_on && !current.merchant_happened and (current.call_merchant || day_in_array(merch_days, current.day))) {
                for (auto event : ALL_MERCH_OPTIONS) {
                    to_process.emplace_back(current);
                    Game::event_merchant(to_process.back(), event);
                    to_process.back().day_start_skip = true;
                }
            }

            // Witch Event
            if (witch_on && !current.witch_happened and (day_in_array(witch_days, current.day))) {
                for (auto event : ALL_WITCH_OPTIONS) {
                    Player witch_event = current;
                    Game::event_witch(witch_event, event);
                    witch_event.day_start_skip = true;
                    to_process.emplace_back(witch_event);
                }
            }
        } else {
            current.day_start_skip = false;
        }

        int current_food_cost = current.food_consumption;

        // Buy Event
        // Will never buy do last day or two days in a row
        if (current.day < current.max_day && !current.bought_last) {
            int days_left = current.max_day - current.day;
            int food_multi = days_left > 1 ? 2 : 1;
            int max_food = min(min(current.gold / max(current.item_shop[_Food][_buy],1), Game::check_weight_left(current) / max(current.item_shop[_Food][_weight], 1)), (current_food_cost * days_left) + 2 * (days_left - 1) - current.food);
            int needed_food = max(0, food_multi * current_food_cost - current.food);

            for (; needed_food <= max_food; ++needed_food) {
                Player to_buy_everything = current;
                Game::find_buy(to_buy_everything, needed_food);
                to_process.emplace_back(to_buy_everything);
                if (needed_food > 0 && current.gold - (needed_food * current.item_shop[_Food][_buy]) >= current.item_shop[_Spice][_buy]) {
                    Player to_buy_food = current;
                    Game::find_buy(to_buy_food, needed_food, true);
                    to_process.emplace_back(to_buy_food);
                }
            }
        }

        if (current.day == current.max_day || current.food >= current_food_cost) {
            // Sell Event
            // Check you have stuff to sell
            if (Game::check_has_items(current)) {
                Player sell = current;
                Game::event_sell_items(sell);
                if (sell.day != sell.max_day) {
                    to_process.emplace_back(sell);
                } else {
                    add_finished(sell);
                }
            }

            // Trader Event
            // Check cost and new food cost
            // Will never buy do last day
            if (current.gold >= current.npc_shop[_Trader][_cost] && current.day < current.max_day && current.food >= current_food_cost + current.npc_shop[_Trader][_eat]) {
                Player buy_trader = current;
                Game::event_buy_npc(buy_trader, _Trader);
                if (buy_trader.day != buy_trader.max_day) {
                    to_process.emplace_back(buy_trader);
                } else {
                    add_finished(buy_trader);
                }
            }

            // Camel Event
            // Check cost and new food cost
            // Will never do on last day
            if (current.gold >= current.npc_shop[_Camel][_cost] && current.day < current.max_day && current.food >= current_food_cost + current.npc_shop[_Camel][_eat]) {
                Player buy_camel = current;
                Game::event_buy_npc(buy_camel, _Camel);
                if (buy_camel.day != buy_camel.max_day) {
                    to_process.emplace_back(buy_camel);
                } else {
                    add_finished(buy_camel);
                }
            }

            // Interest Event
            // Lowest buy sell is *1.65 for 2 days or *1.284 per day
            // Will never do if before day 4
            if (current.day > 3 && (current.interest_rate > 1.2 || current.day == current.max_day)) {
                Player interest = current;
                Game::event_interest(interest);
                if (interest.day != interest.max_day) {
                    to_process.emplace_back(interest);
                } else {
                    add_finished(interest);
                }
            }
        }

        if (processed >= limit) {
            round_num += 1;
            cout << "Processing Round " << round_num << "\n";
            print_solves(1);
            processed = 0;
        }
    }
    print_solves(10);
}

void GameSolver::find_solve_loop() {
    // TODO - Consider if your already at max day or less then 10
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
        for (const auto& player : result[1]) {
            add_finished(player);
        }
        result.clear();
    }
    print_solves(10);
}