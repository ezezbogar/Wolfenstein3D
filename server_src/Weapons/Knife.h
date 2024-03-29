#ifndef WOLFENSTEIN3D_KNIFE_H
#define WOLFENSTEIN3D_KNIFE_H

#include <vector>

#include "Weapon.h"
#include "../../common_src/Timer.h"
#include "../../common_src/Configuration.h"

class Knife : public Weapon {
private:
    Timer stabTimer;
    bool firstStab;
    const float TIME_BETWEEN_STABS, KNIFE_RANGE;

public:
    /* Constructor */
    explicit Knife(const Configuration& config);

    void startShooting() override;

    void fireTheGun(std::vector<Player*> &players,
                    int shootingPlayerNumber, const Map &map) override;

    /* Destructor */
    ~Knife();

private:
    bool _isInTheKnifeRange(uint8_t* thisPlayerInfo,
                            uint8_t* otherPlayerInfo);

    void _stab(std::vector<Player*> &players, int shootingPlayerNumber,
                const Map &map);
};

#endif //WOLFENSTEIN3D_KNIFE_H
