#include "Map.h"
#include "Exceptions/ErrorMap.h"
#include "GameConstants.h"

#define KEY_MAP "map"
#define KEY_ROW "length"
#define KEY_COL "width"

Map::Map(const Configuration& config) : n_row(config.getInt(KEY_ROW)),
										n_col(config.getInt(KEY_COL)) {
	this->map = new int*[n_row];

	for (int i = 0; i < n_row; ++i) {
		map[i] = new int[n_col];
	}

	config.initializeMatrix(this->n_row, this->n_col, this->map, KEY_MAP);
}

Map::Map(const std::vector<std::vector<int>>& initial_map)
    : n_row(initial_map.size()), n_col(initial_map[0].size()) {
    this->map = new int*[n_row];

    for (int i = 0; i < n_row; ++i) {
        map[i] = new int[n_col];
    }

    for (int i = 0; i < n_row; ++i) {
        for (int j = 0; j < n_col; j++){
            map[i][j] = initial_map[i][j];
        }
    }
}

bool Map::outOfRange(int x, int y) const {
	return ((x >= this->n_row) || (x < 0) || (y >= this->n_col) || (y < 0));
}

int Map::get(int x, int y) const {
	if (Map::outOfRange(x, y)) throw ErrorMap("Out of range");
	return map[x][y];
}

long Map::get_n_row() const{
    return this->n_row;
}

long Map::get_n_col() const{
    return this->n_col;
}

void Map::set(int x, int y, int value) {
    map[x][y] = value;
}

Map::~Map() {
	for (int i = 0; i < n_row; ++i) {
		delete[] map[i];
	}

	delete[] map;
}


