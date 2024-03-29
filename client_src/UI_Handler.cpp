#include <thread>
#include "UI_Handler.h"
#include "../common_src/GameConstants.h"
#define TTF_TEXTURES 5
#define WEAPONS 5
#define BJ_FACES 8
#define TOTAL_HP 100

#define SCORE 0
#define LIVES 1
#define HP_ 2
#define AMMO 3
#define DEATH_MESSAGE 4

UI_Handler::UI_Handler(SDL_Renderer *renderer, Raycaster &raycaster,
                       TexturesContainer &tex, const std::string& font_path, int width,
                       int height) : renderer(renderer), tex(tex), raycaster(raycaster),
                       leaderboard(renderer, tex, font_path, height, width){
    int cell_height = (4*height)/18;
    int cell_y_pos = (14*height)/18;

    this->elements.score = {width/32, (16*height)/18, (9*width)/32, cell_height};
    this->elements.lives = {(10*width)/32, (16*height)/18, (4*width)/32, cell_height};
    this->elements.bj_face = {(14*width)/32, cell_y_pos, (4*width)/32, cell_height};
    this->elements.hp = {(18*width)/32, (16*height)/18, (4*width)/32, cell_height};
    this->elements.key = {(22*width)/32, cell_y_pos, (3*width)/32, cell_height};
    this->elements.weapon = {(25*width)/32, cell_y_pos, (6*width)/32, cell_height};
    this->elements.ammo = {(28*width)/32, (9*height)/10, (3*width)/32, cell_height};
    this->elements.weapon_animation = {(10*width)/32, (7*height)/18, (12*width)/32, (7*height)/18};
    this->elements.death_message = {0, height/144, width, height/16};

    SDL_Color color = {240, 250, 252};
    int font_size = width/16;
    for(int i=0; i < TTF_TEXTURES-1; i++)
        this->font_textures.emplace_back(font_path, font_size, color, renderer, "0");

    this->font_textures.emplace_back(font_path, font_size/2, color, renderer, "0");

    for(int i=0; i < WEAPONS; i++){
        this->dynamic.emplace_back(tex.getDynamic(TextureID(Knife_Pl+i)), false);
    }
}

void UI_Handler::raycast(DirectedPositionable &player_pos, PlayerView &view,
                         std::vector<Positionable> &objects,
                         std::vector<DirectedPositionable> &directed_objects,
                         std::vector<int> &doors_states, bool not_playing) {
    this->raycaster.draw(player_pos, view, objects, directed_objects,
                         doors_states, not_playing);
}

void UI_Handler::render() {
    SDL_RenderPresent(this->renderer);;
}

void UI_Handler::clearScreen() {
    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
    SDL_RenderClear(this->renderer);
}

void UI_Handler::loadBackground() {
    tex.getStatic(Background)->render(nullptr, nullptr);
}

void UI_Handler::loadPlayerHUD(std::vector<int> &player_info) {
    int bj_face_tex = int(BJ_0) + int((BJ_FACES*(TOTAL_HP-player_info[HP_HUD]-1))/TOTAL_HP);

    DynamicTexture& weapon = dynamic[player_info[WEAPON_HUD]];
    if (player_info[FIRING_HUD]){
        weapon.getTexture(1)->render(nullptr, &this->elements.weapon_animation);
    } else {
        weapon.getTexture(0)->render(nullptr, &this->elements.weapon_animation);
    }
    tex.getStatic(HUD)->render(nullptr, nullptr);
    tex.getStatic(TextureID(int(Knife_HUD) + player_info[WEAPON_HUD]))->render(nullptr, &this->elements.weapon);
    tex.getStatic(TextureID(bj_face_tex))->render(nullptr, &this->elements.bj_face);
    tex.getStatic(TextureID(int(HasNotKey) + player_info[KEY_HUD]))->render(nullptr, &this->elements.key);

    font_textures[SCORE].renderHorizontallyCentered(std::to_string(player_info[SCORE_HUD]), nullptr, &this->elements.score);
    font_textures[LIVES].renderHorizontallyCentered(std::to_string(player_info[LIVES_HUD]), nullptr, &this->elements.lives);
    font_textures[HP_].renderHorizontallyCentered(std::to_string(player_info[HP_HUD]), nullptr, &this->elements.hp);
    if (player_info[WEAPON_HUD] != 0)
        font_textures[AMMO].renderHorizontallyCentered(std::to_string(player_info[AMMO_HUD]), nullptr, &this->elements.ammo);
}

void UI_Handler::loadLeaderboard(std::vector<std::string> &names,
                                 std::vector<int> &values) {
    leaderboard.render(names, values);
}

void UI_Handler::loadGameOverMessage() {
    std::string message = "You lost all your lives. Specting another player.";
    font_textures[DEATH_MESSAGE].renderHorizontallyCentered(message, nullptr,
                                                            &this->elements.death_message);
}

void UI_Handler::loadDeathTilt() {
    int color = rand()/256;
    SDL_SetRenderDrawColor(this->renderer, 255, color, color, 255);
    SDL_RenderClear(this->renderer);
}

