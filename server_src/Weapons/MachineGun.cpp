#include "MachineGun.h"
#include "../../common_src/MachineGunItem.h"

MachineGun::MachineGun(const float time_between_shots,
                    const float time_between_bursts,
                    const int bullets_per_burst,
                    TextureID texture, float item_radius) :
                    Weapon(MACHINE_GUN, texture, item_radius),
                    lastShotDelay(-1), burstBulletCounter(0),
                    isInBurstRecover(false),
                    TIME_BETWEEN_SHOTS(time_between_shots),
                    TIME_BETWEEN_BURSTS(time_between_bursts),
                    BULLETS_PER_BURST(bullets_per_burst) {}

void MachineGun::startShooting() {
    this->weaponIsShooting = true;
    this->lastShotDelay = -1;
    this->burstBulletCounter = 0;
    this->isInBurstRecover = false;
    this->fireTimer.start();
}

void MachineGun::fireTheGun(std::vector<Player*> &players, int shootingPlayerNumber, const Map &map) {
    if (this->lastShotDelay == -1) {
        this->shoot(players, shootingPlayerNumber, map);  /* First shot */
        this->burstBulletCounter++;
        this->lastShotDelay = 0;

    } else if (((this->fireTimer.getTime()/1000) + this->lastShotDelay >= this->TIME_BETWEEN_SHOTS) &&
                (this->burstBulletCounter < this->BULLETS_PER_BURST) && (this->isInBurstRecover == false)) {
        this->shoot(players, shootingPlayerNumber, map);
        this->lastShotDelay = this->lastShotDelay + (this->fireTimer.getTime()/1000) - this->TIME_BETWEEN_SHOTS;
        this->burstBulletCounter++;
        this->fireTimer.start();
    }

    if (this->burstBulletCounter >= this->BULLETS_PER_BURST) {
        this->burstTimer.start();
        this->burstBulletCounter = 0;
        this->isInBurstRecover = true;

    } else if (this->isInBurstRecover && ((this->burstTimer.getTime()/1000) > this->TIME_BETWEEN_BURSTS)) {
        this->isInBurstRecover = false;
        this->fireTimer.start();  /* Si bien no disparo empiezo a contar aca */
    }
}

Item* MachineGun::getWeaponItem(float x, float y) {
    return new MachineGunItem(x, y, this->texture, this->id, this->item_radius, 
                            this->TIME_BETWEEN_SHOTS, this->TIME_BETWEEN_BURSTS,
                            this->BULLETS_PER_BURST);
}
/*
TextureID MachineGun::getTextureItem() const {
    return this->textureItem;
}

int MachineGun::getID() const {
    return this->id;
}

float MachineGun::getRadius() const {
    return this->radius;
}

float MachineGun::getTimeBetweenShots() const {
    return this->TIME_BETWEEN_SHOTS;
}

float MachineGun::getTimeBetweenBursts() const {
    return this->TIME_BETWEEN_BURSTS;
}

float MachineGun::getBulletsPerBurst() const {
    return this->BULLETS_PER_BURST;
}
*/
MachineGun::~MachineGun() {}
