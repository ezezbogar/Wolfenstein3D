#include "Player.h"
#include "../common_src/Exceptions/ErrorMap.h"
#include <algorithm>
#include <iostream> //Borrar
#include <cmath>
#include <cstring>

#define WALKABLE 0
#define PLAYER_SIZE 0.1
#define RAYS_AMOUNT 20
#define PI 3.14159

//Actions
#define ISNOTMOVING 0
#define ISMOVINGFORWARDS 1
#define ISMOVINGBACKWARDS 2
#define ISTURNINGLEFT 3
#define ISTURNINGRIGHT 4
/*
Player::Player(float moveSpeed, float rotSpeed, float posX, float posY) :
                DirectedPositionable(posX, posY, -1, 0, None) {
    this->moveSpeed = moveSpeed;
    this->rotSpeed = rotSpeed;
    this->camPlaneX = 0;  // Perpendicular to direction
    this->camPlaneY = 1;  // Perpendicular to direction
    this->state = ISNOTMOVING;
}
*/
Player::Player(const Configuration& config, const std::string& player_number)
            : DirectedPositionable(config.getSubFloat(player_number, "pos_x"),
                            config.getSubFloat(player_number, "pos_y"),
                            config.getSubInt(player_number, "dir_x"), 
                            config.getSubInt(player_number, "dir_y"), None),
            action(config) {
    this->moveSpeed = config.getSubFloat(player_number, "move_speed");
    this->rotSpeed = config.getSubFloat(player_number, "rot_speed");
    this->camPlaneX = this->dir_y; // Rotation matrix 90 degrees clockwise
    this->camPlaneY = -this->dir_x; // Rotation matrix 90 degrees clockwise
    this->state = ISNOTMOVING;
}

static void initialize_rays(float *x, float *y, float radius, int rays) {
    float two_pi = 2*PI;
    float step = two_pi/rays;

    for (int i = 0; i < rays; ++i){
        x[i] = radius * cos (step*i);
        y[i] = radius * sin (step*i);
    }
}

void Player::look_for_collision(const Map& map) {
	float player_x_rays[RAYS_AMOUNT], player_y_rays[RAYS_AMOUNT];
	initialize_rays(player_x_rays, player_y_rays, PLAYER_SIZE, RAYS_AMOUNT);
	
	for (int i = 0; i < RAYS_AMOUNT; i++) {
        if (map.get(int(this->x + player_x_rays[i]),
                    int(this->y + player_y_rays[i])) != WALKABLE) {
            throw ErrorMap("Collision detected.");
        }
	}
}

void Player::look_for_item(Items& items) {
	for (size_t i = 0; i < items.size(); ++i) {
//		if (items[i].collidesWith(player)) {
            try {
                items[i]->equipTo(this->action);
                items.remove(i);
            } catch (const std::exception& e) {
                std::cout << e.what() << std::endl;
            }
    //    }
	}
}

void Player::updatePlayer(const Map& map, Items& items) {
    float old_x = this->x, old_y = this->y;

    if (this->state != ISNOTMOVING) {
        if (this->state == ISMOVINGFORWARDS) {
            this->_moveForwards();
        } else if (this->state == ISMOVINGBACKWARDS) {
            this->_moveBackwards();
        } else if (this->state == ISTURNINGLEFT) {
            this->_turnLeft();
        } else if (this->state == ISTURNINGRIGHT) {
            this->_turnRight();
        } else {
            throw GameException("Player has an invalid state!");
        }
    }

    try {
        Player::look_for_collision(map);
        Player::look_for_item(items);
    } catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
        this->x = old_x;
        this->y = old_y;
    }
    // Borrar
    std::cout << "posX: " << this->x;
    std::cout << ", posY: " << this->y;
    std::cout << ", dir_x: " << this->dir_x;
    std::cout << ", dir_y: " << this->dir_y << std::endl;
}

void Player::setState(uint8_t newState) {
    this->state = newState;
}

void Player::die() {
    //Reiniciar posicion
    //this->action.respawn()
}

bool Player::isDead() {
    return this->action.isDead();
}

void Player::shoot() {
//    this->ammo--;
    /* Logica del disparo */
    //this->action.shoot();
}

void Player::receiveShot(int damage) {
 /*   this->health -= damage;
    if (this->health <= 0) {
        this->die();
    } */
}

void Player::_moveForwards() {
    this->x += this->dir_x * this->moveSpeed;
    this->y += this->dir_y * this->moveSpeed;
}

void Player::_moveBackwards() {
    this->x -= this->dir_x * this->moveSpeed;
    this->y -= this->dir_y * this->moveSpeed;
}

void Player::_turnLeft() {
    float oldDirX = this->dir_x;
    this->dir_x = (this->dir_x * cos(this->rotSpeed) - this->dir_y * sin(this->rotSpeed));
    this->dir_y = (oldDirX * sin(this->rotSpeed) + this->dir_y * cos(this->rotSpeed));

    float oldPlaneX = this->camPlaneX;
    this->camPlaneX = this->camPlaneX * cos(this->rotSpeed) - this->camPlaneY * sin(this->rotSpeed);
    this->camPlaneY = oldPlaneX * sin(this->rotSpeed) + this->camPlaneY * cos(this->rotSpeed);
}

void Player::_turnRight() {
    float oldDirX = this->dir_x;
    this->dir_x = (this->dir_x * cos(-this->rotSpeed) - this->dir_y * sin(-this->rotSpeed));
    this->dir_y = (oldDirX * sin(-this->rotSpeed) + this->dir_y * cos(-this->rotSpeed));

    float oldPlaneX = this->camPlaneX;
    this->camPlaneX = (this->camPlaneX * cos(-this->rotSpeed) - this->camPlaneY * sin(-this->rotSpeed));
    this->camPlaneY = (oldPlaneX * sin(-this->rotSpeed) + this->camPlaneY * cos(-this->rotSpeed));
}

void Player::getPositionData(uint8_t *msg) {
    memcpy(msg, &this->x, sizeof(float));
    memcpy(msg + sizeof(float), &this->y, sizeof(float));
    memcpy(msg + 2 * sizeof(float), &this->dir_x, sizeof(float));
    memcpy(msg + 3 * sizeof(float), &this->dir_y, sizeof(float));
}

void Player::getPositionDataWithPlane(uint8_t *msg) {
    this->getPositionData(msg);
    memcpy(msg + 4 * sizeof(float), &this->camPlaneX, sizeof(float));
    memcpy(msg + 5 * sizeof(float), &this->camPlaneY, sizeof(float));
}

void Player::getHUDData(uint8_t *msg) {
    this->action.geHUDInfo(msg);
}

Player::~Player() {}
