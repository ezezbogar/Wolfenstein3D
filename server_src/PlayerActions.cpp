#include "PlayerActions.h"
#include "Items/HealthRecover.h"
#include "Items/Treasure.h"
#include "Items/BulletItem.h"
#include "Items/Items.h"
#include "Exceptions/GameException.h"
#include <cstring>
#include <endian.h>

#define KEY_INITIAL_HEALTH "initial_health"
#define KEY_INITIAL_SCORE "initial_score"
#define KEY_TOTAL_LIVES "total_lives"
#define KEY_INITIAL_BULLETS "initial_bullets"
#define KEY_WEAPONS "weapons"
#define KEY_KEY "key"

PlayerActions::PlayerActions(const Configuration& config) :
                            key(Configuration(config, KEY_KEY)),
                            weapons(Configuration(config, KEY_WEAPONS)) {
	this->health = config.getInt(KEY_INITIAL_HEALTH);
    this->score = config.getInt(KEY_INITIAL_SCORE);
    this->lives = config.getInt(KEY_TOTAL_LIVES);
    this->initialHealth = config.getInt(KEY_INITIAL_HEALTH);
    this->bulletsCounter = 0;
    this->killsCounter = 0;
    this->hasLost = false;
}

void PlayerActions::use(HealthRecover* recover) {
	this->health = (*recover) + this->health;
}

void PlayerActions::use(Treasure* treasure) {
	this->score = (*treasure) + this->score;
}

void PlayerActions::use(BulletItem* bullet) {
    this->weapons.equip(bullet);
}

bool PlayerActions::hasWeapon(int id) const {
	return this->weapons.hasWeapon(id);
}

void PlayerActions::equip(Weapon* weapon) {
	this->weapons.equip(weapon);
}

void PlayerActions::equipKey() {
    this->key.equip();
}

bool PlayerActions::hasKey() const {
    return this->key.has();
}

void PlayerActions::interactWith(ManualDoor& door) {
    this->key.open(door);
}

void PlayerActions::die(Items* items, float x, float y) {
    RandomPosition rd_pos(x, y);
    weapons.reset(items, rd_pos);
    
    if (this->key.has()) {
        items->push_back(this->key.getItem(rd_pos.getX(), rd_pos.getY()));
        this->key.used();
    }

    this->health = this->initialHealth;

    if (this->lives == 0) {
        this->hasLost = true;
    } else {
        this->lives--;
    }
}

bool PlayerActions::isDead() const {
	return (this->health == 0);
}

bool PlayerActions::playerHasLost() {
    return this->hasLost;
}

int PlayerActions::getKills() {
    return this->killsCounter;
}

int PlayerActions::getScore() {
    return this->score;
}

int PlayerActions::getBulletsFired() {
    return this->bulletsCounter;
}

void PlayerActions::startShooting() {
    this->weapons.startShooting();
}

void PlayerActions::stopShooting() {
    this->weapons.stopShooting();
}

void PlayerActions::nextWeapon() {
    this->weapons.nextWeapon();
}

void PlayerActions::setWeapon(uint8_t id) {
    this->weapons.setWeapon(id);
}

void PlayerActions::prevWeapon() {
    this->weapons.prevWeapon();
}

uint8_t PlayerActions::getWeaponID() const {
    return this->weapons.getWeaponID();
}

bool PlayerActions::isShooting() const {
    return this->weapons.isShooting();
}

void PlayerActions::fireTheGun(std::vector<Player*>& players,
                            uint8_t shooting_player_number, const Map& map) {
    this->weapons.fireTheGun(players, shooting_player_number,
                                            map);
}

void PlayerActions::receiveShot(uint8_t damage) {
    if (damage > this->health){
        this->health = 0;  
    } else {
        this->health -= damage;
    }
}

void PlayerActions::increaseBulletCounter(uint8_t bulletsAmount) {
    this->bulletsCounter += bulletsAmount;
}

void PlayerActions::increaseKillCounter() {
    this->killsCounter++;
}

void PlayerActions::useBullets(uint8_t bulletsAmount) {
    this->weapons.useBullets(bulletsAmount);
}

bool PlayerActions::hasBullets(uint8_t bulletsAmount) {
    return this->weapons.hasBullets(bulletsAmount);
}

void PlayerActions::getHUDInfo(uint8_t* msg) {
    memcpy(msg, &this->lives, sizeof(uint8_t));
    memcpy(msg + sizeof(uint8_t), &this->health, sizeof(uint8_t));

    uint8_t current_weapon = this->weapons.getWeaponID();
    memcpy(msg + 2 * sizeof(uint8_t), &current_weapon, sizeof(uint8_t));

    bool has_key = this->key.has();
    memcpy(msg + 3 * sizeof(uint8_t), &has_key, sizeof(bool));

    uint8_t weapon_is_shooting = this->weapons.isShooting();
    memcpy(msg + 3 * sizeof(uint8_t) + sizeof(bool), &weapon_is_shooting,
            sizeof(bool));


    int bullets = this->weapons.getBullets();
    uint32_t aux;

    aux = htole32(*reinterpret_cast<uint32_t *>((&bullets)));

    memcpy(msg + 3 * sizeof(uint8_t) + 2 * sizeof(bool), &aux,
           sizeof(int));

    aux = htole32(*reinterpret_cast<uint32_t *>((&this->score)));

    memcpy(msg + 3 * sizeof(uint8_t) + 2 * sizeof(bool) + sizeof(int),
           &aux, sizeof(int));
}

PlayerActions::~PlayerActions() {}
