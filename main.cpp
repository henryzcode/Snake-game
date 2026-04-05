#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <string>
#include <SDL2/SDL_ttf.h>

// make high score system

// segment struct
struct seg {
    int x;
    int y;
    bool operator==(const seg& other) const {
        return x == other.x && y == other.y;
    }
};

// snake's food struct
struct food {
    int x;
    int y;
};

// const variables
const int CELL = 20;
const int GRID_W = 64;
const int GRID_H = 39;
const int WIDTH = GRID_W * CELL;
const int HEIGHT = GRID_H * CELL;
const int SEC_W = 30;
const int SEC_H = 20;
const int SEC_WIDTH = SEC_W * CELL;
const int SEC_HEIGHT = SEC_H * CELL;
const int max_fps = 10;
double fps = 7.25;
double basefps = 7.25;

// predefine functions
void draw_main(SDL_Renderer* renderer, const std::vector<seg>& snake, food apple, int score, TTF_Font* font, bool nosizeboard, bool portal, Uint64 close_t);
void draw_sec(SDL_Renderer* renderer, std::vector<seg>& snake, int score, TTF_Font* Font, Uint64 close_t, bool portal, std::vector<food> sec_apple, bool nosizeboard);
bool if_spawn(std::vector<food> apples, std::vector<seg> snake, food pos);

// vector snake
std::vector<seg> snake;
std::vector<food> sec_apple;

// main
int main(int argc, char* argv[]) {
    // snake direction & variables
    int dirX = 1;
    int dirY = 0;
    int _dirX = 1;
    int _dirY = 0;

    int score = 3;
    bool nosizeboard = false;
    int world = 0;

    // random devices for main world
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distX(1, GRID_W - 2);
    std::uniform_int_distribution<> distY(1, GRID_H - 2);
    std::uniform_int_distribution<> time(30, 50);

    // for world portal
    std::uniform_int_distribution<> distSECY(1, SEC_H - 2);
    std::uniform_int_distribution<> distSECX(1, SEC_W - 2);

    // define apple's pos
    food apple = {distX(gen), distY(gen)};
    food back_apple;

    // snake's original body
    snake.push_back({GRID_W / 2, GRID_H / 2});
    snake.push_back({GRID_W / 2 - 1, GRID_H / 2});
    snake.push_back({GRID_W / 2 - 2, GRID_H / 2});

    // SDL init
    if (SDL_Init(SDL_INIT_VIDEO) != 0) return 1;
    if (TTF_Init() != 0) return 1;

    SDL_Window* window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) return 1;

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return 1;

    TTF_Font* font = TTF_OpenFont("PixelOperator.ttf", 40);
    if (!font) {
        std::cout << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return 1; 
    }

    Uint64 port_delay_time = time(gen);
    Uint64 start_time = 0;
    bool portal = false;

    bool running = true;
    SDL_Event event;

    // update time calculation
    Uint64 lastTime = SDL_GetTicks64();
    double accumulator = 0.0;
    double step = 1.0 / fps;

    // main game loop
    while (running) {
        Uint64 currentTime = SDL_GetTicks64();
        double deltaTime = (currentTime - lastTime) / 1000.0;
        lastTime = currentTime;

        accumulator += deltaTime;

        // events handling
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;

            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_DELETE) running = false;
                if (event.key.keysym.sym == SDLK_LSHIFT || event.key.keysym.sym == SDLK_RSHIFT) {
                    if (basefps == fps) {
                        fps = basefps * 2.5;
                    }
                    else {
                        fps = basefps;
                        basefps += 0.1;
                        fps += 0.1;
                    }
                    step = 1.0 / fps;
                }

                if ((event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) && dirY != 1) {
                    _dirX = 0; _dirY = -1;
                }
                else if ((event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) && dirY != -1) {
                    _dirX = 0; _dirY = 1;
                }
                else if ((event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a) && dirX != 1) {
                    _dirX = -1; _dirY = 0;
                }
                else if ((event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d) && dirX != -1) {
                    _dirX = 1; _dirY = 0;
                }
            }
        }

        if (accumulator >= step) {
            accumulator -= step;

            dirX = _dirX;
            dirY = _dirY;

            int nextX = snake.front().x + dirX;
            int nextY = snake.front().y + dirY;

            // world logic
            if (world == 0){
                if (portal && nextY == GRID_H - 1 && nextX == (int)GRID_W / 2) {
                    world = 1;
                    snake.clear();
                    back_apple = apple;

                    sec_apple.clear();
                    // SPAWN 4 APPLES IN PORTAL
                    for (int i = 0; i < 4; i++) {
                        food pos;
                        do {
                            pos = {distSECX(gen), distSECY(gen)};
                        } while (!if_spawn(sec_apple, snake, pos));
                        sec_apple.push_back(pos);
                    }

                    int midX = (int)(SEC_W/2);
                    for(int i = score; i >=1; i--){
                        snake.push_back({midX, i});
                    }
                    dirX = 0; dirY = 1;
                    _dirX = 0; _dirY = 1;
                    continue;
                }
                if (nextX < 1 || nextX >= GRID_W -1 || nextY < 1 || nextY >= GRID_H -1){
                    running = false;
                    std::cout << "Snake Game Ended\nReason: The snake hits the border\n------------------------------\nSize = " << score << std::endl;
                    break;
                }
            }
            else {
                if (portal && nextY < 1 && nextX == (int)SEC_W / 2) {
                    world = 0;
                    snake.clear();
                    apple = back_apple;
                    int midX = (int)(GRID_W/2);
                    for(int i = score; i >= 1; i--){
                        snake.push_back({midX, GRID_H-i});
                    }
                    dirX = 0; dirY = -1;
                    _dirX = 0; _dirY = -1;
                    continue;
                }
                if (nextX < 1 || nextX >= SEC_W -1 || nextY < 1 || nextY >= SEC_H -1){
                    running = false;
                    std::cout << "Snake Game Ended\nReason: The snake hit the border\n--------------------------------\nSize = " << score << std::endl;
                    break;
                }
            }

            // collision detection
            bool growing = (world == 0 && nextX == apple.x && nextY == apple.y);
            bool sec_grow = false;
            int eaten_index = -1;

            if (world == 1) {
                for (size_t i = 0; i < sec_apple.size(); i++) {
                    if (sec_apple[i].x == nextX && sec_apple[i].y == nextY) {
                        sec_grow = true;
                        eaten_index = i;
                        break;
                    }
                }
            }

            if (growing) {
                snake.insert(snake.begin(), {nextX, nextY});
                apple.x = distX(gen);
                apple.y = distY(gen);
                score += 1;
            }
            else if (sec_grow){
                snake.insert(snake.begin(), {nextX, nextY});
                if (eaten_index != -1) {
                    sec_apple.erase(sec_apple.begin() + eaten_index);
                }
                // Maintain 4 apples
                food pos;
                do {
                    pos = {distSECX(gen), distSECY(gen)};
                } while (!if_spawn(sec_apple, snake, pos));
                sec_apple.push_back(pos);
                score += 1;
            }
            else {
                snake.pop_back();
                snake.insert(snake.begin(), {nextX, nextY});
            }

            if (std::find(snake.begin() + 1, snake.end(), snake.front()) != snake.end()) {
                running = false;
                std::cout << "Snake Game Ended\nReason: The snake hit its body\n---------------------------------\nSize = " << score << std::endl;
                break;
            }

            if (world == 1 && !portal) {
                running = false;
                std::cout << "Snake Game Ended\nReason: The snake was trapped in the portal\n------------------------------\nSize = " << score << std::endl;
            }

            if (snake.front().x < 22 && snake.front().y < 3) nosizeboard = true;
            else nosizeboard = false;

            if (SDL_GetTicks64() / 1000 >= start_time + (portal ? 30 : port_delay_time)) {
                start_time = SDL_GetTicks64() / 1000;
                portal = !portal;
                if (!portal) port_delay_time = time(gen);
            }
        }

        Uint64 close_t = portal ? 30 - (SDL_GetTicks64() / 1000 - start_time) : 0;

        if (world == 1) draw_sec(renderer, snake, score, font, close_t, portal, sec_apple, nosizeboard);
        else draw_main(renderer, snake, apple, score, font, nosizeboard, portal, close_t);

        SDL_RenderPresent(renderer);
        SDL_Delay(8.333);
    }

    SDL_Delay(1000);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void draw_main(SDL_Renderer* renderer, const std::vector<seg>& snake, food apple, int score, TTF_Font* font, bool nosizeboard, bool portal, Uint64 close_t){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    Uint8 txt_trans=255;

    SDL_SetRenderDrawColor(renderer, 230, 40, 40, 255);
    for (int i = 0; i < GRID_W; i++) {
        SDL_Rect border = {i*CELL,0,CELL,CELL};
        SDL_RenderFillRect(renderer, &border);
    }
    for (int i = 0; i < GRID_W; i++) {
        if (i == (int)GRID_W / 2 && portal) continue;
        SDL_Rect border = {i*CELL,(GRID_H-1)*CELL,CELL,CELL};
        SDL_RenderFillRect(renderer, &border);
    }
    for (int i = 0; i < GRID_H; i++) {
        SDL_Rect border = {(GRID_W-1)*CELL,i*CELL,CELL,CELL};
        SDL_RenderFillRect(renderer, &border);
    }
    for (int i = 0; i < GRID_H; i++) {
        SDL_Rect border = {0,i*CELL,CELL,CELL};
        SDL_RenderFillRect(renderer, &border);
    }

    SDL_SetRenderDrawColor(renderer, 38, 38, 38, 255);
    for (int x = 2; x < GRID_W-1; x++) {
        SDL_Rect wgrid = {x * CELL,1*CELL,1,GRID_H*CELL - 2*CELL};
        SDL_RenderFillRect(renderer, &wgrid);
    }
    for (int x = 2; x < GRID_H-1; x++) {
        SDL_Rect hgrid = {1 * CELL,x*CELL,GRID_W*CELL - 2*CELL,1};
        SDL_RenderFillRect(renderer, &hgrid);
    }

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect appleRect = {apple.x * CELL,apple.y * CELL,CELL,CELL};
    SDL_RenderFillRect(renderer, &appleRect);

    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_Rect headRect = {snake[0].x * CELL,snake[0].y * CELL,CELL,CELL};
    SDL_RenderFillRect(renderer, &headRect);

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    for (size_t i = 1; i < snake.size(); i++) {
        SDL_Rect rect = {snake[i].x * CELL,snake[i].y * CELL,CELL,CELL};
        SDL_RenderFillRect(renderer, &rect);
    }

    std::string score_txt = portal ? "Size: " + std::to_string(score) + " | PORTAL CLOSING: " + std::to_string(close_t): "Size: " + std::to_string(score);
    if (nosizeboard) txt_trans = 100;

    SDL_Surface* txt = TTF_RenderText_Blended(font, score_txt.c_str(), {255,255,255,txt_trans});
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, txt);
    SDL_Rect txt_rect = {30,20,txt->w,txt->h};
    SDL_FreeSurface(txt);
    SDL_RenderCopy(renderer, texture, nullptr, &txt_rect);
    SDL_DestroyTexture(texture);
}

void draw_sec(SDL_Renderer* renderer, std::vector<seg>& snake, int score, TTF_Font* font, Uint64 close_t, bool portal, std::vector<food> sec_apple, bool nosizeboard) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

    for (int i = 0; i < SEC_W; i++) {
        if (i == SEC_W /2 && portal) continue;
        SDL_Rect border = {i*CELL,0,CELL,CELL};
        SDL_RenderFillRect(renderer, &border);
    }
    for (int i = 0; i < SEC_W; i++) {
        SDL_Rect border = {i*CELL,(SEC_H-1)*CELL,CELL,CELL};
        SDL_RenderFillRect(renderer, &border);
    }
    for (int i = 0; i < SEC_H; i++) {
        SDL_Rect border = {(SEC_W-1)*CELL,i*CELL,CELL,CELL};
        SDL_RenderFillRect(renderer, &border);
    }
    for (int i = 0; i < SEC_H; i++) {
        SDL_Rect border = {0,i*CELL,CELL,CELL};
        SDL_RenderFillRect(renderer, &border);
    }
    SDL_SetRenderDrawColor(renderer, 38, 38, 38, 255);
    for (int x = 2; x < SEC_W-1; x++) {
        SDL_Rect wgrid = {x * CELL,1*CELL,1,SEC_H*CELL - 2*CELL};
        SDL_RenderFillRect(renderer, &wgrid);
    }
    for (int x = 2; x < SEC_H-1; x++) {
        SDL_Rect hgrid = {1 * CELL,x*CELL,SEC_W*CELL - 2*CELL,1};
        SDL_RenderFillRect(renderer, &hgrid);
    }

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (auto i: sec_apple) {
        SDL_Rect appleRect = {i.x * CELL,i.y * CELL,CELL,CELL};
        SDL_RenderFillRect(renderer, &appleRect);
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_Rect headRect = {snake[0].x * CELL,snake[0].y * CELL,CELL,CELL};
    SDL_RenderFillRect(renderer, &headRect);

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    for (size_t i = 1; i < snake.size(); i++) {
        SDL_Rect rect = {snake[i].x * CELL,snake[i].y * CELL,CELL,CELL};
        SDL_RenderFillRect(renderer, &rect);
    }

    std::string score_txt = portal ? "Size: " + std::to_string(score) + " | PORTAL CLOSING: " + std::to_string(close_t): "Size: " + std::to_string(score);
    SDL_Surface* txt = TTF_RenderText_Blended(font, score_txt.c_str(), {255,255,255,nosizeboard ? (Uint8) 100 : (Uint8) 255});
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, txt);
    SDL_Rect txt_rect = {30,20,txt->w,txt->h};
    SDL_FreeSurface(txt);
    SDL_RenderCopy(renderer, texture, nullptr, &txt_rect);
    SDL_DestroyTexture(texture);
}

bool if_spawn(std::vector<food> apples, std::vector<seg> snake, food pos) {
    for (auto i: apples) {
        if (pos.x == i.x && pos.y == i.y) return false;
    }
    for (auto i: snake) {
        if (pos.x == i.x && pos.y == i.y) return false;
    }
    return true;
}
