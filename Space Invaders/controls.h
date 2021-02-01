#include <GL/glew.h>
#include <GLFW/glfw3.h>

bool game_running = false;
int move_dir = 0;
int move_ydir = 0;
bool fire_pressed = 0;
bool fire2_pressed = 0;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    switch (key) {
    case GLFW_KEY_ESCAPE:
        if (action == GLFW_PRESS) game_running = false;
        break;
    case GLFW_KEY_RIGHT:
        if (action == GLFW_PRESS) move_dir += 1;
        else if (action == GLFW_RELEASE) move_dir -= 1;
        break;
    case GLFW_KEY_LEFT:
        if (action == GLFW_PRESS) move_dir -= 1;
        else if (action == GLFW_RELEASE) move_dir += 1;
        break;
    case GLFW_KEY_UP:
        if (action == GLFW_PRESS) move_ydir += 1;
        else if (action == GLFW_RELEASE) move_ydir -= 1;
        break;
    case GLFW_KEY_DOWN:
        if (action == GLFW_PRESS) move_ydir -= 1;
        else if (action == GLFW_RELEASE) move_ydir += 1;
        break;
    case GLFW_KEY_SPACE:
        if (action == GLFW_RELEASE) fire_pressed = true;
        break;
    case GLFW_KEY_A:
        if (action == GLFW_RELEASE) fire2_pressed = true;
        break;
    default:
        break;
    }
}