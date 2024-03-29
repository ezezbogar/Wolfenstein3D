#include "Bot.h"
#include <vector>
#include <iostream> //borrar

#define KEY_PATH "bot_path"
#define POS_KNIFE 0

Bot::Bot(const Configuration& config_stats,
         const Configuration& config_map,
         const uint8_t _player_number,
         std::string botName,
         std::vector<Sound>& sounds) :
         Player(config_stats, config_map, _player_number, botName, sounds) {
    this->action.setWeapon(POS_KNIFE);
    this->L = luaL_newstate();
    luaL_openlibs(this->L);
    luaL_loadfile(this->L, config_stats.getString(KEY_PATH).c_str());
    lua_pcall(this->L, 0, LUA_MULTRET, 0);
}

void Bot::getState(std::vector<Player*> &players, int botNumber,
                    const Map &map) {
    uint8_t newState;
    this->_pushMap(map);
    this->_pushPlayersPositions(players, botNumber);
    this->_pushBotInfo();
    lua_pcall(this->L, 6, 1, 0);
    newState = static_cast<uint8_t>(lua_tonumber(this->L, -1));
    lua_pop(this->L, 1);
    this->setState(newState);
    this->action.setWeapon(POS_KNIFE);
}

void Bot::_pushMap(const Map& map){
    lua_getglobal(this->L, "getBotInstruction");
    lua_newtable(this->L);

    for (int j = 0; j < map.get_n_row(); j++) {
        lua_pushnumber(this->L, j);
        lua_newtable(this->L);
        for (int i = 0; i < map.get_n_col(); i++) {
            /* Push the index number */
            lua_pushnumber(this->L, i);
            /* Push the value at 'i' */
            lua_pushnumber(this->L, map.get(j, i));
            /* The table is now at -3 on the stack */
            /* This tells Lua to put "stack[-1]" at "stack[-2]" and pop
             * them off leaving the table at the top of the stack */
            lua_settable(this->L, -3);
        }
        lua_settable(this->L, -3);
    }
}

void Bot::_pushPlayersPositions(std::vector<Player*> &players, int botNumber) {
    int j = 0;
    lua_newtable(this->L);
    for (unsigned long int i = 0; i < players.size(); i++) {
        if (i != static_cast<unsigned long int>(botNumber)) {
            lua_pushnumber(this->L, j);
            lua_pushnumber(this->L, players[i]->getX());
            lua_settable(this->L, -3);
            j++;
            lua_pushnumber(this->L, j);
            lua_pushnumber(this->L, players[i]->getY());
            lua_settable(this->L, -3);
            j++;
        }
    }
}

void Bot::_pushBotInfo() {
    lua_pushnumber(this->L, this->getX());
    lua_pushnumber(this->L, this->getY());
    lua_pushnumber(this->L, this->getDirX());
    lua_pushnumber(this->L, this->getDirY());
}

Bot::~Bot() {
    lua_close(this->L);
}
