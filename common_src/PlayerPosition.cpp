#include "PlayerPosition.h"
#include <cmath>

PlayerPosition::PlayerPosition(float pos_x, float pos_y, float dir_x, float dir_y):
                    pos_x(pos_x), pos_y(pos_y), dir_x(dir_x), dir_y(dir_y){
    float delta = 0.01;
    float norm = std::sqrt(dir_x*dir_x + dir_y*dir_y);
    if ((norm - delta) > 1 || (norm + delta) < 1)
        throw ; // direccion debe tener norma 1
}

PlayerPosition PlayerPosition::operator=(const PlayerPosition &other) {
    this->pos_x = other.pos_x;
    this->pos_y = other.pos_y;
    this->dir_x = other.dir_x;
    this->dir_y = other.dir_y;
    return *this;
}

void PlayerPosition::moveHorizontally(float movement) {
    this->pos_x += movement;
}

void PlayerPosition::moveVertically(float movement) {
    this->pos_y += movement;
}

float PlayerPosition::getPosX() const {
    return this->pos_x;
}

float PlayerPosition::getPosY() const {
    return this->pos_y;
}

void PlayerPosition::setDirX(float new_dir) {
    this->dir_x = new_dir;
}

void PlayerPosition::setDirY(float new_dir) {
    this->dir_y = new_dir;
}

float PlayerPosition::getDirX() const {
    return this->dir_x;
}

float PlayerPosition::getDirY() const {
    return this->dir_y;
}
