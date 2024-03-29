#include "BulletItem.h"
#include "../Exceptions/ItemException.h"

#define KEY_MAX_BULLETS "max_bullets"

BulletItem::BulletItem(const Configuration& config, float _x, float _y)
				: Item(_x, _y, Bullets, config.getInt(KEY_VALUE),
						config.getFloat(KEY_RADIUS)),
						MAX_BULLETS(config.getInt(KEY_MAX_BULLETS)) {}

BulletItem::BulletItem(float _x, float _y, int _value, float _radius,
					int max_bullets)
				: Item(_x, _y, Bullets, _value, _radius),
						MAX_BULLETS(max_bullets) {}

void BulletItem::equipTo(PlayerActions& action) {
	action.use(this);
}

int BulletItem::operator+(int bullets) const {
	if (bullets >= this->MAX_BULLETS) 
		throw ItemException("Can't take more bullets.");

	int new_bullets = this->value + bullets;

	if (new_bullets > this->MAX_BULLETS) new_bullets = this->MAX_BULLETS;

	return new_bullets;
}

BulletItem::~BulletItem() {}
