#ifndef WOLFENSTEIN3D_WEAPON_H
#define WOLFENSTEIN3D_WEAPON_H

#include "../../common_src/Map.h"
#include "../Player.h"
#include "WeaponID.h"
#include <vector>
#include <random>

class Player;
class Item;

class Weapon {
protected:
    bool weaponIsShooting;
    std::random_device rd;
    uint8_t id;

    const float item_radius;

public:
    /* Constructor */
    explicit Weapon(uint8_t _id);

    Weapon(uint8_t _id, float radius);

    /* Este metodo es implementado por las clases hijas, cada una lo implementa de manera tal
     * que cumpla con la consigna de cada arma */
    virtual void fireTheGun(std::vector<Player*>& players,
                            int shootingPlayerNumber, const Map& map) = 0;

    /* AKA started pulling the trigger */
    virtual void startShooting() = 0;

    /* AKA stopped pulling the trigger */
    void stopShooting();

    /* Return true if the player is pulling the trigger */
    bool isShooting();

    bool operator==(int other_id) const;

    /* Devuelve el ID del arma */
    uint8_t getWeaponID() const;

    virtual Item* getWeaponItem(float x, float y);

    /* Destructor */
    virtual ~Weapon();

protected:
    /* Logica de disparar una sola bala */
    void shoot(std::vector<Player*>& players, int shootingPlayerNumber,
                const Map& map);

    /* Este metodo es equivalente a una distribucion uniforme [0,1] */
    float _randomNumberGenerator();

private:
    /* Devuelve true si no hay paredes entre los jugadores,
     * en otro caso false */
    bool _isInTheFieldOfView(uint8_t* thisPlayerInfo,
                            uint8_t* otherPlayerInfo, const Map& map);

    /* Probability decays linearly with the angle */
    float _angleProbabilityFunction(float angle);

    /* Probability decays linearly with the distance */
    float _distanceProbabilityFunction(float distance);

    /* Genera una probabilidad de impacto segun el angulo y la distancia */
    bool _runProbability(uint8_t* thisPlayerInfo, uint8_t* otherPlayerInfo);
};

#endif  // WOLFENSTEIN3D_WEAPON_H
