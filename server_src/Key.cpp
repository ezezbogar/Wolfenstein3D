#include "Key.h"
#include "Exceptions/GameException.h"
#include "../common_src/KeyItem.h"
#include <iostream>

#define KEY_TEXTURE "texture"
#define KEY_RADIUS "radius"

Key::Key(const Configuration& config) : 
		item_texture(static_cast<TextureID>(config.getInt(KEY_TEXTURE))),
		item_radius(config.getFloat(KEY_RADIUS)) , available(false) {}

void Key::equip() {
	if (this->available) throw GameException("Can't equip key,"
											" already has one.");

	this->available = true;
}

void Key::open(ManualDoor& door) {
	try {
		door.interact(this->available);
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		this->available = false;
	}
}

bool Key::has() const {
	return this->available;
}

Item* Key::getItem(float x, float y) const {
	return new KeyItem(x, y, this->item_texture, this->item_radius);
}

void Key::used() {
	this->available = false;
}

Key::~Key() {}
