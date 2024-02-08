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
    f_mutex.lock();
    finished.insert(save_file);
    if (finished.size() > 10) [[likely]] {
        ++processed;
        finished.erase(finished.begin());
    }
    f_mutex.unlock();
}

void GameSolver::progress_state(Player current) {
    if (!current.day_start_skip) {
        Game::start_of_day(current);

        if (current.food < 0) {
            return; // Starved to death
        }

        // Merchant Event
        if (merch_on && !current.merchant_happened and (current.call_merchant || day_in_array(merch_days, current.day))) {
            for (auto event : ALL_MERCH_OPTIONS) {
                Player merch = current;
                int worked = Game::event_merchant(merch, event);
                if (worked) {
                    p_mutex.lock();
                    to_process.emplace_back(merch);
                    p_mutex.unlock();
                }
            }
        }

        // Witch Event
        if (witch_on && !current.witch_happened and (day_in_array(witch_days, current.day))) {
            for (auto event : ALL_WITCH_OPTIONS) {
                Player witch = current;
                Game::event_witch(witch, event);
                p_mutex.lock();
                to_process.emplace_back(witch);
                p_mutex.unlock();

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
        int food_multi = days_left > 1 ? 2 : 1; // Would buy for today and tomorrow or no point
        int max_food = min(
                min(current.gold / max(current.item_shop[_Food][_buy], 1),  // Max you can afford
                    Game::check_weight_left(current) / max(current.item_shop[_Food][_weight], 1)), // Max you can carry
                (current_food_cost * days_left) + (days_left * (days_left - 1)) - current.food); // Max needed (Today + tomorrow+2 + Day3+4 + etc)
        int needed_food = max(0, food_multi * current_food_cost - current.food);

        for (; needed_food <= max_food; ++needed_food) {
            Player to_buy_everything = current;
            Game::find_buy(to_buy_everything, needed_food);
            p_mutex.lock();
            to_process.emplace_back(to_buy_everything);
            p_mutex.unlock();
            // In theory, we should only buy just food if it gets us to another day or 2 + 2*Food Cost (aka enough to buy npc tomorrow)
            if (needed_food >= min(current_food_cost - (current.food % max(current_food_cost, 1)), 2*current_food_cost+2 - current.food)  && current.gold - (needed_food * current.item_shop[_Food][_buy]) >= current.item_shop[_Spice][_buy]) {
                Player to_buy_food = current;
                Game::find_buy(to_buy_food, needed_food, true);
                p_mutex.lock();
                to_process.emplace_back(to_buy_food);
                p_mutex.unlock();
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
                p_mutex.lock();
                to_process.emplace_back(sell);
                p_mutex.unlock();
            } else {
                add_finished(sell);
            }
        }

        // Trader Event
        // Check cost and new food cost
        // Will never buy do last day
        if (current.gold >= current.npc_shop[_Trader][_cost] && current.day < current.max_day && current.food >= current_food_cost + current.npc_shop[_Trader][_eat]) {
            Player trader = current;
            Game::event_buy_npc(trader, _Trader);
            p_mutex.lock();
            to_process.emplace_back(trader);
            p_mutex.unlock();

        }

        // Camel Event
        // Check cost and new food cost
        // Will never do on last day
        if (current.gold >= current.npc_shop[_Camel][_cost] && current.day < current.max_day && current.food >= current_food_cost + current.npc_shop[_Camel][_eat]) {
            Player camel = current;
            Game::event_buy_npc(camel, _Camel);
            p_mutex.lock();
            to_process.emplace_back(camel);
            p_mutex.unlock();

        }

        // Interest Event
        // Lowest buy sell combo is *1.65 for 2 days or *1.284 per day
        // ALl the rest are at least *2 for 2 days or *1.41 per day
        // This means interest at 1.5 and 1.4 is better aka at least 2 in a run
        // Will never do if before day 4 for search space limiting
        if (current.day > 3 && (current.interest_rate > 1.2 || current.day == current.max_day)) {
            Player interest = current;
            Game::event_interest(interest);
            if (interest.day != interest.max_day) {
                p_mutex.lock();
                to_process.emplace_back(interest);
                p_mutex.unlock();
            } else {
                add_finished(interest);
            }
        }
    }

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

void GameSolver::find_solve_threads(const int amount) {
    bool first = true;
    for (int i = 0; i < amount; ++i) {
        std::thread th([this, first] {this->find_solve_depth(first);});
        threads.push_back(std::move(th));
        first = false;
    }

    for (int i = 0; i < amount; ++i) {
        threads[i].join();
    }

}

void GameSolver::find_solve_depth(const bool main_thread) {
    int round_num = 0;
    bool check = true;
    while (check) {
        p_mutex.lock();
        Player current = to_process.back();
        to_process.pop_back();
        p_mutex.unlock();

        progress_state(current);

        if (main_thread and processed >= print_counter) {
            round_num += 1;
            cout << "Processing Round " << round_num << "\n";
            print_solves(1);
            processed = 0;
        }
        p_mutex.lock();
        check = !to_process.empty();
        p_mutex.unlock();
    }
    if (main_thread) {
        print_solves(10);
    }
}

void GameSolver::find_solve_breath(const bool continuous) {
    int round_num = 0;
    unsigned long long count = 0;
    unsigned long long goal = 0;
    int day = to_process[0].day + 1;
    if (!continuous) {
        goal = to_process.size();
        cout << "Processing Day " << day << " - #" << to_process.size() << "\n";
    }

    while(!to_process.empty()) {
        Player current = to_process.front();
        to_process.pop_front();

        progress_state(current);

        // Limit array per day
        ++count;
        if (!continuous and count >= goal) {
            if (to_process.size() > array_limit) {
                sort(to_process.begin(), to_process.end(), greater <>());
                to_process.resize(array_limit);
            }
            goal = to_process.size();
            count = 0;
            day += 1;
            cout << "Processing Day " << day << " - #" << to_process.size() << "\n";
        }

        // Print temp results
        if (processed >= print_counter) {
            round_num += 1;
            cout << "Processing Round " << round_num << "\n";
            print_solves(1);
            processed = 0;
        }
    }
    print_solves(10);
}