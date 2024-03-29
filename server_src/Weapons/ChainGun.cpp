#include "ChainGun.h"
#include "../Items/ChainGunItem.h"

#include <vector>

ChainGun::ChainGun(const float time_between_shots, float item_radius) :
                    Weapon(CHAIN_GUN, item_radius),
                    lastShotDelay(-1),
                    TIME_BETWEEN_SHOTS(time_between_shots) {}

void ChainGun::startShooting() {
    this->weaponIsShooting = true;
    this->lastShotDelay = -1;
    this->fireTimer.start();
}

void ChainGun::fireTheGun(std::vector<Player*> &players,
                        int shootingPlayerNumber, const Map &map) {
    if (this->lastShotDelay == -1) {
        this->shoot(players, shootingPlayerNumber, map);  /* First shot */
        players[shootingPlayerNumber]->makeSound(ChainGunSFX);
        this->lastShotDelay = 0;

    } else if (((this->fireTimer.getTime()/1000) + this->lastShotDelay) >=
                TIME_BETWEEN_SHOTS) {
        this->shoot(players, shootingPlayerNumber, map);
        players[shootingPlayerNumber]->makeSound(ChainGunSFX);
        this->lastShotDelay = this->lastShotDelay +
                                (this->fireTimer.getTime()/1000) -
                                TIME_BETWEEN_SHOTS;
        this->fireTimer.start();
    }
}

Item* ChainGun::getWeaponItem(float x, float y) {
    return new ChainGunItem(x, y, this->item_radius,
                            this->TIME_BETWEEN_SHOTS);
}

ChainGun::~ChainGun() {}
