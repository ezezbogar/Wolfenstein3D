#include "Rocket.h"
#include "../Player.h"
#include <cstring>
#include <endian.h>

#define WALL_SIZE 1
#define WALKABLE 0

Rocket::Rocket(float _move_speed, float _size, float _max_damage,
            float _max_damage_distance, float pos_x, float pos_y,
            float dir_x, float dir_y, int shootingPlayerNumber) :
        DirectedPositionable(pos_x, pos_y, dir_x, dir_y, Missile_0),
        MOVE_SPEED(_move_speed), SIZE(_size), MAX_DAMAGE(_max_damage),
        MAX_DAMAGE_DISTANCE(_max_damage_distance) {
    this->shootingPlayerNumber = shootingPlayerNumber;
    this->exploded = false;
}

void Rocket::update(std::vector<Player*>& players, const Map& map, double timeSlice) {
    this->x += this->dir_x * this->MOVE_SPEED * timeSlice;
    this->y += this->dir_y * this->MOVE_SPEED * timeSlice;
    if (_collided(players, map)) {
        _explode(players, map);
        this->exploded = true;
    }
}

void Rocket::_explode(std::vector<Player*>& players, const Map& map) {
    for (size_t i = 0; i < players.size(); ++i) {
        float distance = this->distanceTo(*players[i]);
        uint8_t damage = _calculateDamage(distance);
        if (distance <= MAX_DAMAGE_DISTANCE) {
            players[i]->receiveShot(damage);
            if (players[i]->isDead()) {
                players[this->shootingPlayerNumber]->increaseKillCounter();
            }
        }
    }
}

bool Rocket::_collided(std::vector<Player*>& players, const Map& map) {
    Collider collider(this->x, this->y, SIZE);
    bool collided = (this->_lookForWallCollision(map, collider) || this->_lookForPlayerCollision(players, collider));
    return collided;
}

bool Rocket::_lookForWallCollision(const Map& map, const Collider& collider) {
    bool collided = false;
    for (int i = this->x-1; i <= this->x+1; ++i) {
        for (int j = this->y-1; j <= this->y+1; ++j) {
            if (map.get(i, j) != WALKABLE) {
                if (collider.collidesWith(i, j, WALL_SIZE, WALL_SIZE)) {
                    collided = true;
                }
            }
        }
    }
    return collided;
}

bool Rocket::_lookForPlayerCollision(std::vector<Player*>& players, const Collider& collider) {
    bool collided = false;
    for (size_t i = 0; i < players.size(); ++i) {
        if (static_cast<int>(i) != this->shootingPlayerNumber) {
            if (collider.collidesWith(players[i]->getCollider()) == true) {
                collided = true;
            }
        }
    }
    return collided;
}

uint8_t Rocket::_calculateDamage(float distance) {
    float damageDealt = (-1/MAX_DAMAGE_DISTANCE * distance) + 1;
    if (damageDealt < 0) {
        damageDealt = 0;
    }
    damageDealt = damageDealt * MAX_DAMAGE;
    return static_cast<uint8_t>(damageDealt);
}

void Rocket::getPositionData(uint8_t *msg) {
    uint32_t aux;

    aux = htole32(*reinterpret_cast<uint32_t *>((&this->x)));
    memcpy(msg, &aux, sizeof(float));

    aux = htole32(*reinterpret_cast<uint32_t *>((&this->y)));
    memcpy(msg + sizeof(float), &aux, sizeof(float));

    aux = htole32(*reinterpret_cast<uint32_t *>((&this->dir_x)));
    memcpy(msg + 2 * sizeof(float), &aux, sizeof(float));

    aux = htole32(*reinterpret_cast<uint32_t *>((&this->dir_y)));
    memcpy(msg + 3 * sizeof(float), &aux, sizeof(float));
}

bool Rocket::hasExploded() const {
    return this->exploded;
}

Rocket::~Rocket() {}
