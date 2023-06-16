//
// Created by Jay on 6/13/2023.
//

#include <cmath>
#include <bits/stdc++.h>
#include "Game.h"

class negative_gold_error : public std::exception {};
class negative_food_error : public std::exception {};


void Game::event_buy_items(Player &player, const int (&to_buy)[6]) {
    player.gold -= to_buy[_Food] * player.item_shop[_Food][_buy] +
                   to_buy[_Spice] * player.item_shop[_Spice][_buy] +
                   to_buy[_Pottery] * player.item_shop[_Pottery][_buy] +
                   to_buy[_Marble] * player.item_shop[_Marble][_buy] +
                   to_buy[_Silk] * player.item_shop[_Silk][_buy] +
                   to_buy[_Jewelry] * player.item_shop[_Jewelry][_buy];
    if (player.gold < 0){
        string gold_value = to_string(player.gold);
        throw runtime_error(gold_value);
    }
    player.food += to_buy[_Food];
    player.spice += to_buy[_Spice];
    player.pottery += to_buy[_Pottery];
    player.marble += to_buy[_Marble];
    player.silk += to_buy[_Silk];
    player.jewelry += to_buy[_Jewelry];
    player.bought_last = true;

    stringstream action;
    action << "Buy {" << to_buy[_Food] << ", " << to_buy[_Spice] << ", " << to_buy[_Pottery] << ", " << to_buy[_Marble];
    action << ", " << to_buy[_Silk] << ", " << to_buy[_Jewelry] << "}";
    end_of_day(player, action.str());
}

void Game::event_sell_items(Player &player) {
    player.gold += player.spice * player.item_shop[_Spice][_sell] +
                   player.pottery * player.item_shop[_Pottery][_sell] +
                   player.marble * player.item_shop[_Marble][_sell] +
                   player.silk * player.item_shop[_Silk][_sell] +
                   player.jewelry * player.item_shop[_Jewelry][_sell] +
                   (player.hand_of_midas ? 100 : 0);
    player.spice = 0;
    player.pottery = 0;
    player.marble = 0;
    player.silk = 0;
    player.jewelry = 0;
    player.bought_last = false;

    end_of_day(player, "Sell");
}

void Game::event_buy_npc(Player &player, const npcs &selection) {
    if (selection == _Trader && player.gold >= player.npc_shop[_Trader][_cost]) {
        player.gold -= player.npc_shop[_Trader][_cost];
        player.trader += 1;
        player.interest_rate += player.trader_interest ? 0.1 : 0;
        player.bought_last = false;
        end_of_day(player, "Trader");
    }
    if (selection == _Camel && player.gold >= player.npc_shop[_Camel][_cost]) {
        player.gold -= player.npc_shop[_Camel][_cost];
        player.camel += 1;
        player.bought_last = false;
        end_of_day(player, "Camel");
    }
}

void Game::event_interest(Player &player) {
    player.gold = (int)round(player.interest_rate * player.gold);
    if (not (player.set_interest || player.one_stable_interest)) {
        player.interest_rate -= 0.1;
    }
    player.one_stable_interest = false;
    player.bought_last = false;

    end_of_day(player, "Interest");
}

void Game::event_merchant(Player &player, const merch_options &selection) {
    switch (selection) {
        case _Cornucopia:
            if (player.gold >= (player.merch_discount ? 20 : 25)) {
                player.gold -= (player.merch_discount ? 20 : 25);
                player.cornucopia = true;
                player.actions.emplace_back("Merch Event " + mo_conversion.at(_Cornucopia));
                player.merchant_happened = true;
            }
            break;
        case _Route_to_Mahaji:
            if (player.gold >= (player.merch_discount ? 20 : 25)) {
                player.gold -= (player.merch_discount ? 20 : 25);
                player.item_shop[_Spice][_sell] += 20;
                player.actions.emplace_back("Merch Event " + mo_conversion.at(_Route_to_Mahaji));
                player.merchant_happened = true;
            }
            break;
        case _Wooden_Statuette:
            if (player.gold >= (player.merch_discount ? 20 : 25)) {
                player.gold -= (player.merch_discount ? 20 : 25);
                player.statue = true;
                player.actions.emplace_back("Merch Event " + mo_conversion.at(_Wooden_Statuette));
                player.merchant_happened = true;
            }
            break;
        case _Canvas_Bag:
            if (player.gold >= (player.merch_discount ? 40 : 50)) {
                player.gold -= (player.merch_discount ? 40 : 50);
                player.backpack += 20;
                player.actions.emplace_back("Merch Event " + mo_conversion.at(_Canvas_Bag));
                player.merchant_happened = true;
            }
            break;
        case _Leaders_Necklace:
            if (player.gold >= (player.merch_discount ? 40 : 50)) {
                player.gold -= (player.merch_discount ? 40 : 50);
                player.trader += 1;
                player.actions.emplace_back("Merch Event " + mo_conversion.at(_Leaders_Necklace));
                player.merchant_happened = true;
            }
            break;
        case _Hand_of_Midas:
            if (player.gold >= (player.merch_discount ? 40 : 50)) {
                player.gold -= (player.merch_discount ? 40 : 50);
                player.hand_of_midas = true;
                player.actions.emplace_back("Merch Event " + mo_conversion.at(_Hand_of_Midas));
                player.merchant_happened = true;
            }
            break;
        case _Sturdy_Saddle:
            if (player.gold >= (player.merch_discount ? 80 : 100)) {
                player.gold -= (player.merch_discount ? 80 : 100);
                player.npc_shop[_Camel][_weight] += 20;
                player.actions.emplace_back("Merch Event " + mo_conversion.at(_Sturdy_Saddle));
                player.merchant_happened = true;
            }
            break;
        case _Magic_Cleppsydra:
            if (player.gold >= (player.merch_discount ? 80 : 100)) {
                player.gold -= (player.merch_discount ? 80 : 100);
                player.max_day += 1;
                player.actions.emplace_back("Merch Event " + mo_conversion.at(_Magic_Cleppsydra));
                player.merchant_happened = true;
            }
            break;
        case _Blue_Treasure:
            if (player.gold >= (player.merch_discount ? 80 : 100)) {
                player.gold -= (player.merch_discount ? 80 : 100);
                player.interest_rate += 0.2;
                player.actions.emplace_back("Merch Event " + mo_conversion.at(_Blue_Treasure));
                player.merchant_happened = true;
            }
            break;
    }
}

void Game::event_witch(Player &player, const witch_options &selection) {
    switch (selection) {
        case _Presents_Gift:
            player.backpack += 20;
            player.interest_rate -= 0.2;
            player.actions.emplace_back("Witch Event " + wo_conversion.at(_Presents_Gift));
            player.witch_happened = true;
            break;
        case _Vertue_of_Patience:
            player.set_interest = true;
            player.item_shop[_Jewelry][_buy] += 100;
            player.actions.emplace_back("Witch Event " + wo_conversion.at(_Vertue_of_Patience));
            player.witch_happened = true;
            break;
        case _Midas_was_a_Trader:
            player.npc_shop[_Trader][_effect] += 150;
            player.item_shop[_Marble][_sell] *= 0;
            player.actions.emplace_back("Witch Event " + wo_conversion.at(_Midas_was_a_Trader));
            player.witch_happened = true;
            break;
        case _Camelization:
            player.npc_shop[_Camel][_effect] += 10;
            player.npc_shop[_Camel][_eat] *= 2;
            player.actions.emplace_back("Witch Event " + wo_conversion.at(_Camelization));
            player.witch_happened = true;
            break;
        case _Time_is_Money:
            player.max_day += 1;
            player.item_shop[_Jewelry][_sell] -= 500;
            player.actions.emplace_back("Witch Event " + wo_conversion.at(_Time_is_Money));
            player.witch_happened = true;
            break;
        case _Animal_Lover:
            player.camel += 2;
            player.max_day -= 1;
            player.actions.emplace_back("Witch Event " + wo_conversion.at(_Animal_Lover));
            player.witch_happened = true;
            break;
        case _Oasis_of_Sanctifan:
            player.npc_shop[_Trader][_eat] = 0;
            player.npc_shop[_Camel][_eat] = 0;
            player.item_shop[_Silk][_buy] += 200;
            player.actions.emplace_back("Witch Event " + wo_conversion.at(_Oasis_of_Sanctifan));
            player.witch_happened = true;
            break;
        case _The_Stonecutter:
            player.item_shop[_Marble][_weight] -= 15;
            player.item_shop[_Marble][_buy] += 60;
            player.actions.emplace_back("Witch Event " + wo_conversion.at(_The_Stonecutter));
            player.witch_happened = true;
            break;
    }
}

void Game::start_of_day(Player &player) {
    if (player.day != 0) {
        player.food -= check_food_cost(player);
        if (player.food < 0) {
            throw negative_food_error();
        }
    }
    player.day += 1;
}

void Game::end_of_day(Player &player, const std::string &action) {
    string type;
    player.gold += player.daily_income + player.trader * player.npc_shop[_Trader][_effect];
    player.actions.emplace_back(action);
    if (player.day == player.max_day) {
        type = "Final";
    } else {
        type = "Day End";
    }
    player.points = check_points(player, type);
}

int Game::check_food_cost(const Player& player) {
    int cost = 0;
    if (player.day != player.max_day) {
        cost = max(0, player.trader * player.npc_shop[_Trader][_eat] +
                      player.camel * player.npc_shop[_Camel][_eat] +
                      player.cornucopia * -2);
    }
    return cost;
}

int Game::check_has_items(const Player &player) {
    return player.spice + player.pottery + player.marble + player.silk + player.jewelry;
}

int Game::check_points(const Player &player, const std::string &type) {
    return (player.gold +
            player.spice * player.item_shop[_Spice][_buy] +
            player.pottery * player.item_shop[_Pottery][_buy] +
            player.marble * player.item_shop[_Marble][_buy] +
            player.silk * player.item_shop[_Silk][_buy] +
            player.jewelry * player.item_shop[_Jewelry][_buy] +
            player.trader * player.npc_shop[_Trader][_cost] +
            player.camel * player.npc_shop[_Camel][_cost] +
            (player.statue ? (player.trader + player.camel) * (type == "Final" ? 500 : 25) : 0)
    );
}

int Game::check_weight_left(const Player &player) {
    return (player.food * -player.item_shop[_Food][_weight] +
            player.spice * -player.item_shop[_Spice][_weight] +
            player.pottery * -player.item_shop[_Pottery][_weight] +
            player.marble * -player.item_shop[_Marble][_weight] +
            player.silk * -player.item_shop[_Silk][_weight] +
            player.jewelry * -player.item_shop[_Jewelry][_weight] +
            player.camel * player.npc_shop[_Camel][_effect] +
            player.backpack
    );
}

double Game::check_item_worth(const Player &player, const items &item) {
    return double (player.item_shop[item][_weight]) / (player.item_shop[item][_sell] - player.item_shop[item][_buy]);
}

void Game::find_buy(Player &player, const int &food_needed, const bool &only_food) {
    int what_to_buy[6] = {food_needed, 0, 0, 0, 0, 0};
    int gold_left = player.gold - player.item_shop[_Food][_buy] * food_needed;
    int weight_left = check_weight_left(player) - player.item_shop[_Food][_weight] * food_needed;

    if (not only_food) {
        // Sort reference based on value
        // TODO - this doesn't consider if you will buy something that changes item shop
        pair<double, items> item_pair[6] = {
                {99.0, _Food},
                {check_item_worth(player,_Spice), _Spice},
                {check_item_worth(player,_Pottery), _Pottery},
                {check_item_worth(player,_Marble), _Marble},
                {check_item_worth(player,_Silk), _Silk},
                {check_item_worth(player,_Jewelry), _Jewelry}
        };
        sort(item_pair, item_pair + 6);
        for (auto &i : item_pair) {
            if (i.second == _Food) {
                continue;
            }
            while (weight_left >= player.item_shop[i.second][_weight] && gold_left >= player.item_shop[i.second][_buy]) {
                weight_left -= player.item_shop[i.second][_weight];
                gold_left -= player.item_shop[i.second][_buy];
                what_to_buy[i.second] += 1;
            }
        }
    }

    event_buy_items(player, what_to_buy);
}

void Game::auto_game_loop(Player &player, const vector<ga_data> &event_list) {
    for (auto &event : event_list) {
        start_of_day(player);
        switch (event.action) {
            case _a_buy:
                event_buy_items(player, event.buy_data);
                break;
            case _a_sell:
                event_sell_items(player);
                break;
            case _a_npc:
                event_buy_npc(player, event.npc_data);
                break;
            case _a_interest:
                event_interest(player);
                break;
            case _a_merchant:
                event_merchant(player, event.merch_data);
                break;
            case _a_witch:
                event_witch(player, event.witch_data);
                break;
        }
    }
}

void Game::apply_town(Player &player, const town_options town) {
    switch (town) {
        case _Fang_Mo:
            player.pottery += 1;
            break;
        case _L_Exquise:
            player.npc_shop[_Trader][_eat] = 0;
            break;
        case _Mahaji:
            player.item_shop[_Spice][_sell] += 5;
            break;
        case _Tocaccialli:
            player.max_day += 1;
            break;
        case _Bajaar:
            player.npc_shop[_Trader][_effect] += 20;
            break;
        case _Doba_Lao:
            player.interest_rate += 0.2;
            break;
        case _Huangoz:
            player.item_shop[_Marble][_buy] -= 20;
            break;
        case _Port_Willam:
            player.backpack += 15;
            break;
        case _Moonin:
            player.call_merchant = true;
            break;
        case _Cornecopea:
            player.item_shop[_Food][_buy] = 0;
            break;
        case _Eduming:
            player.trader_interest = true;
            break;
        case _Octoyashi:
            player.food += 3;
            break;
        case _Kifuai:
            player.gold += 10;
            break;
        case _Skjollird:
            player.npc_shop[_Camel][_cost] += 10;
            player.npc_shop[_Camel][_effect] += 10;
            break;
        case _Petrinov:
            player.set_interest = true;
            break;
        case _Jilliqo:
            player.daily_income += 10;
            break;
        case _Gayawaku:
            player.item_shop[_Food][_weight] = 0;
            break;
        case _Vilparino:
            player.merch_discount = true;
            break;
        case _Sanctifan:
            player.one_stable_interest = true;
            break;
        case _Mehaz:
            player.npc_shop[_Camel][_eat] = 0;
            break;
        case _Ipartus:
            player.item_shop[_Silk][_buy] -= 100;
            break;
        case _Normalia:
            break;
    }
}