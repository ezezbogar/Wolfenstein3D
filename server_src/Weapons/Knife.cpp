#include "Knife.h"
#include <cstring>
#include <vector>

#define POSITION_DATA_SIZE 16
#define FLOAT_SIZE sizeof(float)

#define KEY_STABS "time_between_stabs"
#define KEY_RANGE "range"

Knife::Knife(const Configuration& config) : Weapon(KNIFE),
            TIME_BETWEEN_STABS(config.getFloat(KEY_STABS)),
            KNIFE_RANGE(config.getFloat(KEY_RANGE)) {
    this->firstStab = true;
}

void Knife::startShooting() {
    if (this->firstStab == false && this->stabTimer.getTime()/1000 > this->TIME_BETWEEN_STABS) {
        this->weaponIsShooting = true;
    } else if (this->firstStab == true) {
        this->weaponIsShooting = true;
    }

}

void Knife::_stab(std::vector<Player*> &players, int shootingPlayerNumber,
                const Map &map) {
    uint8_t thisPlayerInfo[POSITION_DATA_SIZE];
    players[shootingPlayerNumber]->getPositionData(thisPlayerInfo);

    players[shootingPlayerNumber]->makeSound(KnifeSFX);

    for (size_t i = 0; i < players.size(); i++) {
        if (static_cast<int>(i) != shootingPlayerNumber) {
            uint8_t otherPlayerInfo[POSITION_DATA_SIZE];
            players[i]->getPositionData(otherPlayerInfo);
            if (_isInTheKnifeRange(thisPlayerInfo, otherPlayerInfo)) {
                players[i]->receiveShot(static_cast<uint8_t>(
                                        this->_randomNumberGenerator() * 10)
                                        + 1);  // Rango [1, 10]
                if (players[i]->isDead()) {
                    players[shootingPlayerNumber]->increaseKillCounter();
                }
                break;
            }
        }
    }
}

void Knife::fireTheGun(std::vector<Player*> &players,
                        int shootingPlayerNumber, const Map &map) {
    this->_stab(players, shootingPlayerNumber, map);
    this->stabTimer.start();
    this->weaponIsShooting = false;
    this->firstStab = false;
}

bool Knife::_isInTheKnifeRange(uint8_t *thisPlayerInfo,
                                uint8_t *otherPlayerInfo) {
    float xP1, xP2, yP1, yP2;
    float dirX, dirY, distance;
    memcpy(&xP1, thisPlayerInfo, FLOAT_SIZE);
    memcpy(&yP1, thisPlayerInfo + FLOAT_SIZE, FLOAT_SIZE);
    memcpy(&xP2, otherPlayerInfo, FLOAT_SIZE);
    memcpy(&yP2, otherPlayerInfo + FLOAT_SIZE, FLOAT_SIZE);

    dirX = xP2 - xP1;
    dirY = yP2 - yP1;

    distance = std::sqrt(dirX * dirX + dirY * dirY);

    return (distance <= this->KNIFE_RANGE);
}

Knife::~Knife() {}
