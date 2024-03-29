#include <cstring>
#include <string>
#include <iostream>
#include <algorithm>
#include <endian.h>
#include "Client.h"
#include "../common_src/GameConstants.h"
#include "clientWindow/StringList.h"

#define UINT32_SIZE sizeof(uint32_t)

#define NEW_GAME 0
#define JOIN_GAME 1
#define FLOAT_SIZE sizeof(float)

#define UINT_ATTRIBUTES 5 // lives, hp, weapon, has_key, is_shooting
#define INT_ATTRIBUTES 2  // ammo, score
#define PLAYER_INFO_SIZE (5 + 2 * sizeof(int)) // lives, hp, key, weapon, ammo(int), score(int)

#define PLAYING 0

#define PLAYER_COORDS 6
#define PLAYER_COORDS_SIZE (6 * sizeof(float))

#define OBJECTS_ATTRIBUTES 3
#define OBJECTS_SIZE (2 * sizeof(float) + 1)

#define SOUND_SIZE (sizeof(float) + 1)

#define SLIDERS_SIZE 1

#define OTHER_PLAYERS_COORDS 5
#define OTHER_PLAYERS_SIZE (4 * sizeof(float) + 1)
#define MAX_MESSAGE_SIZE 256

Client::Client(const std::string &host, const std::string &service,
               BlockingQueue<int> &instructions,
               ProtectedQueue<UI_Info> &drawing_info):
        socket(host.c_str(), service.c_str(), 0, false),
        peer(socket.getPeer()), instructions(instructions),
        drawing_info(drawing_info), playing(true) {}


std::vector<std::vector<int>> Client::receiveMap() {
    uint8_t n_row, n_col;
    this->peer.recv(&n_row, 1);
    this->peer.recv(&n_col, 1);
    uint16_t bytes_to_receive = n_col * n_row;

    std::vector<std::vector<int>> received_map(n_row, std::vector<int>(n_col));
    std::vector<uint8_t> received_uints(bytes_to_receive);

    peer.recv(received_uints.data(), bytes_to_receive);

    for (int i=0; i<received_uints.size(); i++) {
        received_map[i/n_col][i%n_col] = received_uints[i];
    }
    return received_map;
}

void Client::sendInstruction() {
    try{
        ssize_t sent = 0;
        while (playing && sent >= 0 && !instructions.isWorking()){
            uint8_t instruction = this->instructions.pop();
            sent = this->peer.send(&instruction, sizeof(uint8_t));
        }
    } catch (WolfensteinException& e){
        std::cerr << "Connection ended.\n";
    }
}

void Client::sendName(const std::string& username) {
    uint8_t  name_len;
    uint8_t bytes_sent[MAX_MESSAGE_SIZE];
    memset(bytes_sent, 0, MAX_MESSAGE_SIZE);

    name_len = username.size();
    this->peer.send(&name_len, 1);
    memcpy(bytes_sent, username.c_str(), name_len);
    this->peer.send(bytes_sent, name_len);
}

void Client::sendJoinGameChoice(){
    uint8_t uint_choice = (uint8_t)JOIN_GAME;
    this->peer.send(&uint_choice, sizeof(uint8_t));
}
void Client::sendNewGameChoice(){
    uint8_t uint_choice = (uint8_t)NEW_GAME;
    this->peer.send(&uint_choice, sizeof(uint8_t));
}

ssize_t Client::receiveInformation() {
    while(playing){
        uint8_t splitted_bytes_to_receive[UINT32_SIZE];
        memset(splitted_bytes_to_receive, 0, UINT32_SIZE);
        this->peer.recv(splitted_bytes_to_receive, UINT32_SIZE);

        uint32_t bytes_to_receive;
        memcpy(&bytes_to_receive, splitted_bytes_to_receive, UINT32_SIZE);
        bytes_to_receive = le32toh(bytes_to_receive);
        bytes_to_receive = (uint32_t)*(uint32_t*)& bytes_to_receive;

        std::vector<uint8_t> bytes_received(bytes_to_receive);
        bool important = false;

        if (bytes_to_receive > 0) {
            this->peer.recv(bytes_received.data(), bytes_to_receive);
            DirectedPositionable player(0, 0, 0, 0, None);
            PlayerView view;

            int already_parsed = 0;
            bool not_playing;
            std::vector<float> coordinates;
            std::vector<int> player_info;
            std::vector<Positionable> objects;
            std::vector<DirectedPositionable> directed_objects; // jugadores y objetos moviles
            std::vector<int> doors_states;
            std::vector<std::pair<int, float>> sounds;

            _assignPlayerInfo(player_info, bytes_received, important,already_parsed);
            _assignPlayerState(not_playing, bytes_received, already_parsed);
            _assignPlayerCoordenates(player, view, coordinates, bytes_received, already_parsed);
            _assignItemsCoordenates(bytes_received, objects, coordinates, already_parsed);
            _assignSounds(bytes_received, sounds, important, already_parsed);
            _assignSlidersStates(bytes_received, doors_states, already_parsed);
            _assignOtherPlayersCoordenates(bytes_received, bytes_to_receive,directed_objects,
                                           coordinates,already_parsed);

            UI_Info new_info(player, view, player_info, objects,
                             directed_objects, doors_states,
                             sounds, not_playing, important);

            this->drawing_info.push(new_info);
        } else {
            this->playing = false;
            this->instructions.doneAdding();
        }
    }
    return 0;
}


void Client::loadLeaderboard(GameInterface &interface) {
    uint32_t number, current_byte = 0;
    uint8_t bytes_to_receive, name_len;
    std::string name;

    this->peer.recv(&bytes_to_receive, 1);
    std::vector<uint8_t> received(bytes_to_receive);
    std::vector<std::string> names;
    std::vector<int> values;
    this->peer.recv(received.data(), bytes_to_receive);

    for (int i = 0; i < LEADERBOARD_ELEMENTS; i++) {
        name_len = received[current_byte];
        current_byte++;

        name = std::string(reinterpret_cast<const char*>(received.data() + current_byte),
                           static_cast<size_t>(name_len));
        current_byte += name_len;

        memcpy(&number, received.data() + current_byte, sizeof(int));
        number = le32toh(number);
        current_byte += sizeof(uint32_t);

        names.push_back(name);
        values.push_back((uint32_t)*(uint32_t*)&number);
    }
    interface.showLeaderboard(names, values);
}

bool Client::isPlaying() const {
    return playing;
}

void Client::stopInGameInteraction() {
    this->playing = false;
    this->instructions.doneAdding();
}

void Client::shutdown() {
    this->playing = false;
    this->socket.stop();
    this->instructions.doneAdding();
}

Client::~Client() {
    if (playing) shutdown();
}


//-------------------------- Metodos privados --------------------------------//

// Asigna la informacion del jugador (vida, balas, arma, ...) a sus atributos
void Client::_assignPlayerInfo(std::vector<int> &info, std::vector<uint8_t> &bytes_received,
                               bool &important, int &already_parsed) {
    uint8_t received_uint8;
    int received_int;
    for(int i=0; i< UINT_ATTRIBUTES; i++){
        memcpy(&received_uint8, bytes_received.data() + i * sizeof(uint8_t), sizeof(uint8_t));
        info.push_back(int(received_uint8));
    }
    for(int i=0; i< INT_ATTRIBUTES; i++){
        memcpy(&received_int, bytes_received.data() +
                UINT_ATTRIBUTES*sizeof(uint8_t) + i*sizeof(int), sizeof(int)); //ammo
        info.push_back(le32toh(received_int));
    }
    important = (important || info[FIRING_HUD] != 0);
    already_parsed += PLAYER_INFO_SIZE;
}

// Parsea el booleano que indica si el jugador se encuentra jugando o no,
void Client::_assignPlayerState(bool &not_playing, std::vector<uint8_t> &bytes_received,
                                int &already_parsed) {
    uint8_t state;
    memcpy(&state, bytes_received.data() + already_parsed, sizeof(uint8_t));
    not_playing = (state != PLAYING);
    already_parsed += sizeof(uint8_t);
}

// Asigna las coordenadas recibidas a los atributos del jugador
void
Client::_assignPlayerCoordenates(DirectedPositionable &player, PlayerView &view,
                                 std::vector<float> &coordinates,
                                 std::vector<uint8_t> &bytes_received, int &already_parsed) {
    uint32_t received;
    for (std::size_t i = 0; i < PLAYER_COORDS; i++) {
        memcpy(&received, bytes_received.data() + already_parsed + FLOAT_SIZE * i, FLOAT_SIZE);
        received = le32toh(received);
        coordinates.push_back((float)*(float*)&received);
    }
    player.setX(coordinates[0]);
    player.setY(coordinates[1]);
    player.setDirX(coordinates[2]);
    player.setDirY(coordinates[3]);
    view.movePlaneX(coordinates[4]);
    view.movePlaneY(coordinates[5]);
    coordinates.clear();
    already_parsed += PLAYER_COORDS_SIZE;
}

// Asigna las coordenadas de los objetos del mapa.
void Client::_assignItemsCoordenates(std::vector<uint8_t> &bytes_received,
                                     std::vector<Positionable> &objects,
                                     std::vector<float> &coordinates,
                                     int &already_parsed) {
    uint32_t received;
    uint8_t texture, objects_parsed;
    memcpy(&objects_parsed, bytes_received.data() + already_parsed, 1);

    for (int i = 0; i < objects_parsed; i++) {
        memcpy(&received, bytes_received.data() + already_parsed + i*OBJECTS_SIZE + 1, FLOAT_SIZE);
        received = le32toh(received);
        coordinates.push_back((float)*(float*)&received);
        memcpy(&received, bytes_received.data() + already_parsed + i*OBJECTS_SIZE + FLOAT_SIZE + 1, FLOAT_SIZE);
        received = le32toh(received);
        coordinates.push_back((float)*(float*)&received);
        memcpy(&texture, bytes_received.data() + already_parsed + i*OBJECTS_SIZE + 2*FLOAT_SIZE + 1, 1);
        coordinates.push_back((float)texture);
    }

    for(std::size_t j=0; j < coordinates.size(); j+=OBJECTS_ATTRIBUTES){
        Positionable object(coordinates[j],coordinates[j + 1],
                            TextureID(coordinates[j+2]));
        objects.push_back(object);
    }
    coordinates.clear();
    already_parsed += (objects_parsed * OBJECTS_SIZE) + 1;
}

// Parsea los sonidos del juego.
void Client::_assignSounds(std::vector<uint8_t> &bytes_received,
                           std::vector<std::pair<int, float>> &sounds,
                           bool &important,
                           int &already_parsed) {
    uint8_t sounds_amount, sound_id;
    uint32_t distance;
    memcpy(&sounds_amount, bytes_received.data() + already_parsed, 1);

    for (int i=0; i<sounds_amount; i++) {
        memcpy(&sound_id, bytes_received.data() + already_parsed + i*SOUND_SIZE + 1, 1);
        memcpy(&distance, bytes_received.data() + already_parsed + i*SOUND_SIZE + 2, sizeof(float));
        distance = le32toh(distance);
        sounds.emplace_back(sound_id, (float)*(float*)&distance);
    }
    already_parsed += (sounds_amount * SOUND_SIZE) + 1;
    important = (important || sounds_amount!=0);
}


// Parsea los estados de las puertas.
void Client::_assignSlidersStates(std::vector<uint8_t> &bytes_received,
                                  std::vector<int> &sliders_states,
                                  int &already_parsed) {
    uint8_t slider_state, sliders_amount;
    memcpy(&sliders_amount, bytes_received.data() + already_parsed, 1);

    for (int i=0; i<sliders_amount; i++) {
        memcpy(&slider_state, bytes_received.data() + already_parsed + i*SLIDERS_SIZE + 1, 1);
        sliders_states.emplace_back(slider_state);
    }
    already_parsed += (sliders_amount * SLIDERS_SIZE) + 1;
}

// Asigna las coordenadas recibidas a los demas jugadores del mapa.
void Client::_assignOtherPlayersCoordenates(std::vector<uint8_t> &bytes_received,
                                            uint32_t bytes_to_receive,
                                            std::vector<DirectedPositionable> &players,
                                            std::vector<float> &coordinates,
                                            int &already_parsed) {
    uint32_t received;
    uint8_t texture;
    int players_amount = (bytes_to_receive - already_parsed) / OTHER_PLAYERS_SIZE;

    for (int i = 0; i < players_amount; i++) {
        memcpy(&received, bytes_received.data() + already_parsed + i*OTHER_PLAYERS_SIZE, FLOAT_SIZE);
        received = le32toh(received);
        coordinates.push_back((float)*(float*)&received);
        memcpy(&received, bytes_received.data() + already_parsed + i*OTHER_PLAYERS_SIZE + FLOAT_SIZE, FLOAT_SIZE);
        received = le32toh(received);
        coordinates.push_back((float)*(float*)&received);
        memcpy(&received, bytes_received.data() + already_parsed + i*OTHER_PLAYERS_SIZE + 2*FLOAT_SIZE, FLOAT_SIZE);
        received = le32toh(received);
        coordinates.push_back((float)*(float*)&received);
        memcpy(&received, bytes_received.data() + already_parsed + i*OTHER_PLAYERS_SIZE + 3*FLOAT_SIZE, FLOAT_SIZE);
        received = le32toh(received);
        coordinates.push_back((float)*(float*)&received);
        memcpy(&texture, bytes_received.data() + already_parsed + i*OTHER_PLAYERS_SIZE + 4*FLOAT_SIZE, 1);
        coordinates.push_back((float)texture);
    }

    for(std::size_t j=0; j < coordinates.size(); j+=OTHER_PLAYERS_COORDS){
        DirectedPositionable other_player(coordinates[j],
                                          coordinates[j + 1],
                                          coordinates[j+2],
                                          coordinates[j+3],
                                          TextureID(coordinates[j+4]));
        players.push_back(other_player);
    }
    coordinates.clear();
}

void Client::getMaps(StringList& _maps) {

    uint8_t maps_to_receive, string_length, max_players;
    uint8_t bytes_received[MAX_MESSAGE_SIZE]; //almacena info recibida
    memset(bytes_received, 0, MAX_MESSAGE_SIZE);

    std::vector<std::string> map_names;
    std::vector<int> players_amounts;

    this->peer.recv(&maps_to_receive, 1);

    for (int i=0; i<maps_to_receive; i++){
        this->peer.recv(&string_length, 1);
        this->peer.recv(bytes_received, string_length);
        this->peer.recv(&max_players, 1);

        std::ostringstream convert;
        for (int j = 0; j < string_length; j++) {
            convert << (char)bytes_received[j];
        }
        std::string map_name = convert.str();

        map_names.push_back(map_name);
        players_amounts.push_back(int(max_players));
    }

    for (int i = 0; i < maps_to_receive; i++) {
        _maps.addItem(i, map_names[i]
                        + "(" + std::to_string(players_amounts[i]) + " pjs" +")");
    }
}

void Client::sendMapChoice(uint8_t choice){
    this->peer.send(&choice, sizeof(uint8_t));

}
void Client::sendPlay() {
    uint8_t choice = (uint8_t) 'p'; //ver esto, meter un sleep?
    this->peer.send(&choice, sizeof(uint8_t));
}

void Client::getGames(StringList &list) {

    uint8_t games, game_id, string_length, actual_players, players_limit;
    uint8_t bytes_received[MAX_MESSAGE_SIZE]; //almacena info recibida
    memset(bytes_received, 0, MAX_MESSAGE_SIZE);

    std::vector<std::string> map_names;
    std::vector<int> ids, connected_players, max_players;

    this->peer.recv(&games, 1);

    for (int i=0; i<games; i++) {
        this->peer.recv(&game_id, 1);
        this->peer.recv(&string_length, 1);
        this->peer.recv(bytes_received, string_length);
        this->peer.recv(&actual_players, 1);
        this->peer.recv(&players_limit, 1);

        std::ostringstream convert;
        for (int j = 0; j < string_length; j++) {
            convert << (char)bytes_received[j];
        }
        std::string map_name = convert.str();

        map_names.push_back(map_name);
        ids.push_back(int(game_id));
        connected_players.push_back(int(actual_players));
        max_players.push_back(int(players_limit));
    }
    for(int i = 0 ; i < games ; i++) {
        list.addItem(ids[i], map_names[i] + " ("+ std::to_string(connected_players[i])
        +"/"+ std::to_string(max_players[i])+ ")");
    }

}

void Client::sendMatchChoice(uint8_t i) {
    this->peer.send(&i, sizeof(uint8_t));
}





