//
// Created by Jay on 6/13/2023.
//

#ifndef TRADING_CARAVAN_SOLVER_ENUMS_H
#define TRADING_CARAVAN_SOLVER_ENUMS_H

#include <string>
#include <unordered_map>


enum items {
    _Food = 0,
    _Spice,
    _Pottery,
    _Marble,
    _Silk,
    _Jewelry
};

enum item_data {
    _buy = 0,
    _sell,
    _weight
};

enum npcs {
    _Trader = 0,
    _Camel
};

enum npc_data {
    _cost = 0,
    _effect,
    _eat
};

enum merch_options {
    _Cornucopia = 0,   // Food consumption -2 TODO This does not apply to the future...
    _Route_to_Mahaji,  // Spice sell +20
    _Wooden_Statuette, // Gain a Statue (+500 points per Trader and Camel)
    _Canvas_Bag,       // Carry Weight +20
    _Leaders_Necklace, // Gain one Trader
    _Hand_of_Midas,    // When you sell everything > Gold +100
    _Sturdy_Saddle,    // Camels weight +20
    _Magic_Cleppsydra, // Max Day +1
    _Blue_Treasure     // Interest Rate +20%
};
const merch_options ALL_MERCH_OPTIONS[] = {
        _Cornucopia,
        _Route_to_Mahaji,
        _Wooden_Statuette,
        _Canvas_Bag,
        _Leaders_Necklace,
        _Hand_of_Midas,
        _Sturdy_Saddle,
        _Magic_Cleppsydra,
        _Blue_Treasure
    };
const std::unordered_map<merch_options, const std::string > mo_conversion = {
        {_Cornucopia,       "Cornucopia"},
        {_Route_to_Mahaji,  "Route to Mahaji"},
        {_Wooden_Statuette, "Wooden Statuette"},
        {_Canvas_Bag,       "Canvas Bag"},
        {_Leaders_Necklace, "Leaders Necklace"},
        {_Hand_of_Midas,    "Hand of Midas"},
        {_Sturdy_Saddle,    "Sturdy Saddle"},
        {_Magic_Cleppsydra, "Magic Cleppsydra"},
        {_Blue_Treasure,    "Blue Treasure"},
};

enum witch_options {
    _Presents_Gift = 0,  // Carry Weight +20        -- Interest Rate -20%
    _Vertue_of_Patience, // Interest Rate is stable -- Jewelry cost +100
    _Midas_was_a_Trader, // Trader income +150      -- Marble sell *0
    _Camelization,       // Camel weight +10        -- Camel eat 2x
    _Time_is_Money,      // Max Day +1              -- Jewelry sell -500
    _Animal_Lover,       // Gain two Camels         -- Max Day -1
    _Oasis_of_Sanctifan, // Crew don't eat          -- Silk buy +200
    _The_Stonecutter     // Marble weight -15       -- Marble cost +60
};
const witch_options ALL_WITCH_OPTIONS[] = {
        _Presents_Gift,
        _Vertue_of_Patience,
        _Midas_was_a_Trader,
        _Camelization,
        _Time_is_Money,
        _Animal_Lover,
        _Oasis_of_Sanctifan,
        _The_Stonecutter
};
const std::unordered_map<witch_options, const std::string > wo_conversion = {
        {_Presents_Gift,      "Present's Gift"},
        {_Vertue_of_Patience, "Vertue of Patience"},
        {_Midas_was_a_Trader, "Midas was a Trader"},
        {_Camelization,       "Camelization"},
        {_Time_is_Money,      "Time is Money"},
        {_Animal_Lover,       "Animal Lover"},
        {_Oasis_of_Sanctifan, "Oasis of Sanctifan"},
        {_The_Stonecutter, "The Stonecutter"}
};

enum town_options {
    _Normalia = 0, // 00 - No Effect
    _Fang_Mo,      // 01 - Begin with 1 Pottery
    _L_Exquise,    // 02 - Traders don't need Food
    _Mahaji,       // 03 - Spice sell +5
    _Tocaccialli,  // 04 - Max Day +1
    _Bajaar,       // 05 - x2 Trader income
    _Doba_Lao,     // 06 - Interest Rate +20%
    _Huangoz,      // 07 - Marble Cost -20
    _Port_Willam,  // 08 - Carry Weight +15
    _Moonin,       // 09 - Call Merchant at anytime
    _Cornecopea,   // 10 - Food is free
    _Eduming,      // 11 - Buying trader > Interest Rate +10%
    _Octoyashi,    // 12 - Begin with +3 Food
    _Kifuai,       // 13 - Begin with +10 Gold
    _Skjollird,    // 14 - Camel cost +10 and weight +10
    _Petrinov,     // 15 - Interest rate doesn't decrease
    _Jilliqo,      // 16 - Daily Gold Income +10
    _Gayawaku,     // 17 - Food is weightless
    _Vilparino,    // 18 - Strange Merchant 20% Sale!
    _Sanctifan,    // 19 - One Free Collect Interest Action TODO - see if this doesn't use a day
    _Mehaz,        // 20 - Camels don't need food
    _Ipartus       // 21 - Silk Cost -100
};
const std::unordered_map<town_options, const std::string > to_conversion = {
        {_Normalia,    "Normalia"},
        {_Fang_Mo,     "Fang-Mo"},
        {_L_Exquise,   "L'Exquise"},
        {_Mahaji,      "Mahaji"},
        {_Tocaccialli, "Tocaccialli"},
        {_Bajaar,      "Bajaar"},
        {_Doba_Lao,    "Doba Lao"},
        {_Huangoz,     "Huangoz"},
        {_Port_Willam, "Port-Willam"},
        {_Moonin,     "Moonin"},
        {_Cornecopea, "Cornecopea"},
        {_Eduming,   "Eduming"},
        {_Octoyashi, "Octoyashi"},
        {_Kifuai,    "Kifuai"},
        {_Skjollird, "Skjollird"},
        {_Petrinov,  "Petrinov"},
        {_Jilliqo,   "Jilliqo"},
        {_Gayawaku,  "Gayawaku"},
        {_Vilparino, "Vilparino"},
        {_Sanctifan, "Sanctifan"},
        {_Mehaz,     "Mehaz"},
        {_Ipartus,   "Ipartus"},
};

enum game_actions {
        _a_buy = 0,
        _a_sell,
        _a_npc,
        _a_interest,
        _a_merchant,
        _a_witch
};

struct ga_data {
    ga_data(game_actions _action, const int food, const int spice, const int pottery, const int marble, const int silk, const int jewelry) {
        action = _action;
        buy_data[0] = food;
        buy_data[1] = spice;
        buy_data[2] = pottery;
        buy_data[3] = marble;
        buy_data[4] = silk;
        buy_data[5] = jewelry;
    };
    ga_data(game_actions _action,  const npcs &_npc_data) {
        action = _action;
        npc_data = _npc_data;
    };
    ga_data(game_actions _action, const merch_options _merch_data) {
        action = _action;
        merch_data = _merch_data;
    };
    ga_data(game_actions _action, const witch_options _witch_data) {
        action = _action;
        witch_data = _witch_data;
    };
    explicit ga_data(game_actions _action) {
        action = _action;
    };
    game_actions action;
    int buy_data[6] {};
    npcs npc_data {};
    merch_options merch_data {};
    witch_options witch_data {};


};

#endif //TRADING_CARAVAN_SOLVER_ENUMS_H