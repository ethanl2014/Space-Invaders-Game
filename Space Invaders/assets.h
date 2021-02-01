#include <cstdint>

class Invader
{
    public:
        size_t x, y;
        uint8_t type;
};

class Projectile
{
    public:
        size_t x, y;
        int dir;
};

class Player
{
    public:
        size_t x = 107;
        size_t y = 32;
        size_t life = 3;
};

#define GAME_MAX_PROJECTILES 128

class Game
{
    public:
        size_t width, height;
        size_t num_invaders = 55;
        size_t num_projectiles = 0;
        Invader* invaders;
        Player player;
        Projectile projectiles[GAME_MAX_PROJECTILES];
};

enum InvaderType : uint8_t
{
    INVADER_DEAD = 0,
    INVADER_TYPE_A = 1,
    INVADER_TYPE_B = 2,
    INVADER_TYPE_C = 3
};