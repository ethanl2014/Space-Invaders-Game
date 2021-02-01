#include <cstdio>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <array>
#include <string>
#include "buffer.h"
#include "sprites.h"
#include "controls.h"
#include "assets.h"
#include "score_sprites.h"

extern "C" {
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
}

uint8_t* sprite_reader(int height, int width, int start_x, int start_y, int pixel, int rgba, unsigned char* data, uint8_t invader_array[])
{
    int counter = 0;
    for (int i = start_y; i < height; i++) {
        for (int j = start_x; j <= width; j++) {
            if (static_cast<int>(data[pixel * rgba * i + j * rgba - 1]) == 255) { invader_array[counter] = 1; }
            else { invader_array[counter] = 0; }
            counter++;
        }

    }
    return invader_array;
}

int main()
{
    glfwInit();
    const size_t buffer_width = 274, buffer_height = 306;
    GLFWwindow* window = glfwCreateWindow(2 * buffer_width, 2 * buffer_height, "Space Invader Game", NULL, NULL);
    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);

    GLenum glw = glewInit();
    glfwSwapInterval(1);
    glClearColor(1.0, 0.0, 0.0, 1.0);

    Buffer2 buffer2; //note: using 'buffer2' instead of buffer b/c I get shader error ~ google search suggests this has something to do with AMD radeon drivers
    buffer2.width = buffer_width;
    buffer2.height = buffer_height;
    buffer2.data = new uint32_t[buffer2.width * buffer2.height];
    buffer2_clear(&buffer2, 0);

    #pragma region Texture setup
    GLuint buffer2_texture;
    glGenTextures(1, &buffer2_texture);
    glBindTexture(GL_TEXTURE_2D, buffer2_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, buffer2.width, buffer2.height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, buffer2.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#pragma endregion

    GLuint fullscreen_triangle_vao;
    glGenVertexArrays(1, &fullscreen_triangle_vao);
   
    #pragma region Shader Setup
    GLuint shader_id = glCreateProgram();
    GLuint shader_vp = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shader_vp, 1, &vertex_shader, 0);
    glCompileShader(shader_vp);
    //validate_shader(shader_vp, vertex_shader);//shaders in buffer.h
    glAttachShader(shader_id, shader_vp);
    glDeleteShader(shader_vp);
    
    GLuint shader_fp = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(shader_fp, 1, &fragment_shader, 0);
    glCompileShader(shader_fp);
    //validate_shader(shader_fp, fragment_shader);
    glAttachShader(shader_id, shader_fp);
    glDeleteShader(shader_fp);
    glLinkProgram(shader_id);
    glUseProgram(shader_id);
    glUniform1i(glGetUniformLocation(shader_id, "buffer2"), 0);
#pragma endregion

    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(fullscreen_triangle_vao);

    #pragma region Sprite Creation
    Sprite spr_invaders[6];

    std::string filename = "sprite_master_32.png";
    int x = 32, y = 32, n = 4; //pixel x and y (32), n = 4 b/c rgba
    unsigned char* data = stbi_load(filename.c_str(), &x, &y, &n, 0);

    int height_a = 8, width_a = 11;
    uint8_t spritemaker_a[88];
    uint8_t spritemaker_a2[88];

    spr_invaders[2].height = height_a;
    spr_invaders[2].width = width_a;
    spr_invaders[2].data = sprite_reader(height_a, width_a, 1, 0, x, n, data, spritemaker_a);

    spr_invaders[3].height = height_a;
    spr_invaders[3].width = width_a;
    spr_invaders[3].data = sprite_reader(height_a, width_a + width_a, 1 + width_a, 0, x, n, data, spritemaker_a2);

    int height_b = 8, width_b = 8;
    uint8_t spritemaker_b[64];
    uint8_t spritemaker_b2[64];

    spr_invaders[0].width = width_b;
    spr_invaders[0].height = height_b;
    spr_invaders[0].data = sprite_reader(height_b, width_b + width_a + width_a, 1 + width_a + width_a, 0, x, n, data, spritemaker_b);

    spr_invaders[1].width = width_b;
    spr_invaders[1].height = height_b;
    spr_invaders[1].data = sprite_reader(height_b + height_a, width_b, 1, 0 + height_a, x, n, data, spritemaker_b2);

    int height_c = 8, width_c = 12;
    uint8_t spritemaker_c[96];
    uint8_t spritemaker_c2[96];

    spr_invaders[4].width = width_c;
    spr_invaders[4].height = height_c;
    spr_invaders[4].data = sprite_reader(height_b + height_a, width_c + width_b, 1 + width_b, 0 + height_a, x, n, data, spritemaker_c);

    spr_invaders[5].width = width_c;
    spr_invaders[5].height = height_c;
    spr_invaders[5].data = sprite_reader(height_b + height_a, width_c + width_b + width_c, 1 + width_b + width_c, 0 + height_a, x, n, data, spritemaker_c2);

    SpriteAnimation spr_invader_dance[3];
    for (size_t i = 0; i < 3; ++i)
    {
        spr_invader_dance[i].loop = true;
        spr_invader_dance[i].num_frames = 2;
        spr_invader_dance[i].frame_duration = 10;
        spr_invader_dance[i].time = 0;

        spr_invader_dance[i].frames = new Sprite * [2];
        spr_invader_dance[i].frames[0] = &spr_invaders[2 * i];
        spr_invader_dance[i].frames[1] = &spr_invaders[2 * i + 1];
    }

    Sprite invader_spr_explosion;
    invader_spr_explosion.width = 13;
    invader_spr_explosion.height = 7;
    uint8_t spritemaker_expl[91];
    invader_spr_explosion.data = sprite_reader(invader_spr_explosion.height + height_b + height_a, invader_spr_explosion.width, 1, 0 + height_a + height_b, x, n, data, spritemaker_expl);

    Sprite spr_text;
    spr_text.width = 5;
    spr_text.height = 7;
    uint8_t spritemaker_text[65 * 35];
    spr_text.data = get_score(spritemaker_text);

    Sprite spr_score = spr_text;
    spr_score.data += 16 * 35;

    Sprite spr_player;
    spr_player.width = 11;
    spr_player.height = 7;
    uint8_t spritemaker_mc[91];
    spr_player.data = sprite_reader(spr_player.height + height_b + height_a, spr_player.width + invader_spr_explosion.width, 1 + invader_spr_explosion.width, 0 + height_a + height_b, x, n, data, spritemaker_mc);

    Sprite spr_laser;
    spr_laser.width = 1;
    spr_laser.height = 3;
    spr_laser.data = new uint8_t[3]
    {
        1, // @
        1, // @
        1  // @
    };
#pragma endregion

    Game game;
    game.width = buffer_width;
    game.height = buffer_height;
    game.invaders = new Invader[game.num_invaders];

    game_running = true;
    bool gameoverlap = false; //checks if invaders have reached bottom
    size_t score = 0;
    int waiting_period = 8;
    bool win_checker = false;

    int player_move_dir = 0;
    int monster_move_dir = 1;
    int monster_move_ydir = 0;

    //positions invaders -- uses 5 and 11 to make 55, adjust accordingly
    for (size_t yi = 0; yi < 5; ++yi)
    {
        for (size_t xi = 0; xi < 11; ++xi)
        {
            Invader& invader = game.invaders[yi * 11 + xi];
            invader.type = (5 - yi) / 2 + 1;

            const Sprite& sprite = spr_invaders[2 * (invader.type - 1)];

            invader.x = 16 * xi + 20 + (invader_spr_explosion.width - sprite.width) / 2;
            invader.y = 17 * yi + 128;
        }
    }

    uint8_t* death_counters = new uint8_t[game.num_invaders]; //keep track of dead aliens for position loops
    for (size_t i = 0; i < game.num_invaders; ++i)
    {
        death_counters[i] = 10;
    }

    uint32_t clear_color = rgb_to_uint32(20, 20, 20); // color background

    while (!glfwWindowShouldClose(window) && game_running)
    {
        buffer2_clear(&buffer2, clear_color);

        //Game Over
        if (gameoverlap || game.player.life == 0 || win_checker == true)
        {
            if (gameoverlap || game.player.life == 0) {
                buffer_draw_text(&buffer2, spr_text, "GAME OVER", game.width / 2 - 30, game.height / 2, rgb_to_uint32(128, 128, 128));
            }
            else {
                buffer_draw_text(&buffer2, spr_text, "YOU WIN!", game.width / 2 - 30, game.height / 2, rgb_to_uint32(128, 128, 128));
                buffer_draw_text(&buffer2, spr_text, "EARTH IS SAVED!", game.width / 2 - 30, game.height / 2 - 30, rgb_to_uint32(128, 128, 128));
            }

            buffer_draw_text(&buffer2, spr_text, "SCORE", 4, game.height - spr_text.height - 7, rgb_to_uint32(128, 128, 128));
            buffer_draw_number(&buffer2, spr_score, score, 4 + 2 * spr_score.width, game.height - 2 * spr_score.height - 12, rgb_to_uint32(128, 128, 128));
            
            glTexSubImage2D(
                GL_TEXTURE_2D, 0, 0, 0,
                buffer2.width, buffer2.height,
                GL_RGBA, GL_UNSIGNED_INT_8_8_8_8,
                buffer2.data
            );
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            glfwSwapBuffers(window);
            glfwPollEvents();
            continue;
        }

        buffer_draw_text(&buffer2, spr_text, "SCORE", 4, game.height - spr_text.height - 7, rgb_to_uint32(128, 128, 128));
        buffer_draw_number(&buffer2, spr_score, score, 4 + 2 * spr_score.width, game.height - 2 * spr_score.height - 12, rgb_to_uint32(128, 128, 128));

        buffer_draw_text(&buffer2, spr_text, "LIFE", game.width - 30, game.height - spr_text.height - 7, rgb_to_uint32(128, 128, 128));
        buffer_draw_number(&buffer2, spr_score, game.player.life, game.width - spr_score.width - 10 , game.height - 2 * spr_score.height - 12, rgb_to_uint32(128, 128, 128));

        for (size_t i = 0; i < game.width; ++i){buffer2.data[game.width * 16 + i] = rgb_to_uint32(128, 0, 128);}// ground

        monster_move_ydir = 0;
        int invader_fire = rand() % game.num_invaders + 1;
        int trigger = rand() % 20 + 1;//set rate of fire

        if (trigger <= 1) {//invader fire loop
            if (game.invaders[invader_fire].type != INVADER_DEAD)
            {
                game.projectiles[game.num_projectiles].x = game.invaders[invader_fire].x + 6;
                game.projectiles[game.num_projectiles].y = game.invaders[invader_fire].y - 8 - 2;
                game.projectiles[game.num_projectiles].dir = -2;
                ++game.num_projectiles;
            }
        }

        win_checker = true;

        //invader movement and sprites
        for (size_t ai = 0; ai < game.num_invaders; ++ai) 
        {
            if (!death_counters[ai]) continue;

            const Invader& invader = game.invaders[ai];

            if (invader.type == INVADER_DEAD)
            {buffer_draw_sprite(&buffer2, invader_spr_explosion, invader.x, invader.y, rgb_to_uint32(239, 153, 29));}
            else
            {
                const SpriteAnimation& animation = spr_invader_dance[invader.type - 1];
                size_t current_frame = animation.time / animation.frame_duration;
                const Sprite& sprite = *animation.frames[current_frame];

                win_checker = false;

                if (invader.type == 1) {buffer_draw_sprite(&buffer2, sprite, invader.x, invader.y, rgb_to_uint32(63, 228, 27));}
                else if (invader.type == 2) {buffer_draw_sprite(&buffer2, sprite, invader.x, invader.y, rgb_to_uint32(228, 27, 177));}
                else if (invader.type == 3) {buffer_draw_sprite(&buffer2, sprite, invader.x, invader.y, rgb_to_uint32(224, 239, 29));}
                
                if (sprite_overlap_check(sprite, invader.x, invader.y, spr_player, game.player.x, game.player.y)) 
                {gameoverlap = true;}
            }

            if (invader.x + 12 + monster_move_dir >= game.width - 1)
            {
                game.invaders[ai].x = game.width - 12 - monster_move_dir - 1;
                monster_move_dir *= -1;
                monster_move_ydir = -1;
            }
            else if ((int)game.invaders[ai].x + monster_move_dir <= 0)
            {
                game.invaders[ai].x = 0;
                monster_move_dir *= -1;
                monster_move_ydir = -1;
            }
            else {//method to increase invader speed as there are less invaders
                for (int i = 0; i < score/350 + 0.5; i++) {
                    game.invaders[ai].x += monster_move_dir;
                }
            }
        }
        //y movement
        if (monster_move_ydir == -1) {
            for (size_t ai = 0; ai < game.num_invaders; ++ai) {
                if (!death_counters[ai]) continue;
                const Invader& invader = game.invaders[ai];
                if (invader.type != INVADER_DEAD) {
                    game.invaders[ai].y += -7;
                }
            }
        }        

        for (size_t bi = 0; bi < game.num_projectiles; ++bi)//laser sprites
        {
            const Projectile& projectile = game.projectiles[bi];
            const Sprite& sprite = spr_laser;
            if (game.projectiles[bi].dir < 0){ buffer_draw_sprite(&buffer2, sprite, projectile.x, projectile.y, rgb_to_uint32(0, 62, 255)); }
            else{ buffer_draw_sprite(&buffer2, sprite, projectile.x, projectile.y, rgb_to_uint32(194, 28, 28)); }
        }

        //player sprite
        buffer_draw_sprite(&buffer2, spr_player, game.player.x, game.player.y, rgb_to_uint32(29, 219, 239));

        for (size_t i = 0; i < 3; ++i) //invader animation
        {
            ++spr_invader_dance[i].time;
            if (spr_invader_dance[i].time == spr_invader_dance[i].num_frames * spr_invader_dance[i].frame_duration)
            {spr_invader_dance[i].time = 0;}
        }

        glTexSubImage2D(
            GL_TEXTURE_2D, 0, 0, 0,
            buffer2.width, buffer2.height,
            GL_RGBA, GL_UNSIGNED_INT_8_8_8_8,
            buffer2.data
        );
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);

        for (size_t ai = 0; ai < game.num_invaders; ++ai) //keep track of death
        {
            const Invader& invader = game.invaders[ai];
            if (invader.type == INVADER_DEAD && death_counters[ai])
            {
                --death_counters[ai];
            }
        }

        //projectile logic
        for (size_t bi = 0; bi < game.num_projectiles;)
        {
            game.projectiles[bi].y += game.projectiles[bi].dir;
            if (game.projectiles[bi].y >= game.width || game.projectiles[bi].y < spr_laser.width)
            {
                game.projectiles[bi] = game.projectiles[game.num_projectiles - 1];
                --game.num_projectiles;
                continue;
            }

            if (game.projectiles[bi].dir < 0)
            {
                bool overlap = sprite_overlap_check(
                    spr_laser, game.projectiles[bi].x, game.projectiles[bi].y,
                    spr_player, game.player.x, game.player.y
                );

                if (overlap)
                {
                    --game.player.life;
                    game.projectiles[bi] = game.projectiles[game.num_projectiles - 1];
                    --game.num_projectiles;
                    break;
                }
            }
            else {

                for (size_t bj = 0; bj < game.num_projectiles; ++bj)
                {
                    if (bi == bj) continue;

                    bool overlap = sprite_overlap_check(
                        spr_laser, game.projectiles[bi].x, game.projectiles[bi].y,
                        spr_laser, game.projectiles[bj].x, game.projectiles[bj].y
                    );

                    if (overlap)
                    {
                        if (bj == game.num_projectiles - 1)
                        {
                            game.projectiles[bi] = game.projectiles[game.num_projectiles - 2];
                        }
                        else if (bi == game.num_projectiles - 1)
                        {
                            game.projectiles[bj] = game.projectiles[game.num_projectiles - 2];
                        }
                        else
                        {
                            game.projectiles[(bi < bj) ? bi : bj] = game.projectiles[game.num_projectiles - 1];
                            game.projectiles[(bi < bj) ? bj : bi] = game.projectiles[game.num_projectiles - 2];
                        }
                        game.num_projectiles -= 2;
                        break;
                    }
                }

                for (size_t ai = 0; ai < game.num_invaders; ++ai)
                {
                    const Invader& invader = game.invaders[ai];
                    if (invader.type == INVADER_DEAD) continue;

                    const SpriteAnimation& animation = spr_invader_dance[invader.type - 1];
                    size_t current_frame = animation.time / animation.frame_duration;
                    const Sprite& invader_sprite = *animation.frames[current_frame];
                    bool overlap = sprite_overlap_check(
                        spr_laser, game.projectiles[bi].x, game.projectiles[bi].y,
                        invader_sprite, invader.x, invader.y
                    );
                    if (overlap)
                    {
                        score += 10 * (4 - game.invaders[ai].type);
                        game.invaders[ai].type = INVADER_DEAD;
                        game.invaders[ai].x -= (invader_spr_explosion.width - invader_sprite.width) / 2;
                        game.projectiles[bi] = game.projectiles[game.num_projectiles - 1];
                        --game.num_projectiles;
                        continue;
                    }
                }
            }
            ++bi;
        }

        //player movement
        player_move_dir = 2 * move_dir;
        if (player_move_dir != 0)
        {
            if (game.player.x + spr_player.width + player_move_dir >= game.width)
            {
                game.player.x = game.width - spr_player.width;
            }
            else if ((int)game.player.x + player_move_dir <= 0)
            {
                game.player.x = 0;
            }
            else game.player.x += player_move_dir;
        }

        if (fire_pressed && game.num_projectiles < GAME_MAX_PROJECTILES && waiting_period == 0)
        {
            game.projectiles[game.num_projectiles].x = game.player.x + spr_player.width / 2;
            game.projectiles[game.num_projectiles].y = game.player.y + spr_player.height;
            game.projectiles[game.num_projectiles].dir = 2;
            ++game.num_projectiles;
            waiting_period = 12; //use to change time between laser shots -- so player can't spam spacebar
        }
        else if (waiting_period == 0) { waiting_period = 0; }
        else { waiting_period--; }
        fire_pressed = false;

        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    glDeleteVertexArrays(1, &fullscreen_triangle_vao);

    for (size_t i = 0; i < 6; ++i)
    {
        spr_invaders[i].data = NULL;
        delete[] spr_invaders[i].data;
    }
    for (size_t i = 0; i < 3; ++i)
    {
        delete[] spr_invader_dance[i].frames;
    }
        
    spr_text.data, invader_spr_explosion.data = NULL;
    delete[] buffer2.data, game.invaders, death_counters, invader_spr_explosion.data, spr_text.data;

    return 0;
}