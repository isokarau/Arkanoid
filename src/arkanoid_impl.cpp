#include "arkanoid_impl.h"

#include <GLFW/glfw3.h>

#ifdef USE_ARKANOID_IMPL
Arkanoid* create_arkanoid()
{
    return new ArkanoidImpl();
}
#endif

void ArkanoidImpl::reset(const ArkanoidSettings &settings)
{
    // TODO:
    // Implement your game world, bricks and
    // carriage initialization
    // ...

    // TODO:
    // remove demo code
    world_size.x = settings.world_size[0];
    world_size.y = settings.world_size[1];

    ball_position = world_size * 0.5f;
    ball_initial_speed = settings.ball_speed;
    ball_radius = settings.ball_radius;
    ball_velocity = Vect(ball_initial_speed);

    carriage_position.x = world_size.x * 0.5f;
    carriage_position.y = world_size.y;
    carriage_width = settings.carriage_width;
}

void ArkanoidImpl::update(ImGuiIO& io, ArkanoidDebugData& debug_data, float elapsed)
{
    world_to_screen = Vect(io.DisplaySize.x / world_size.x, io.DisplaySize.y / world_size.y);

    // process user input
    if (io.KeysDown[GLFW_KEY_A])
        ball_velocity.x -= 1.0f;

    if (io.KeysDown[GLFW_KEY_D])
        ball_velocity.x += 1.0f;

    if (io.KeysDown[GLFW_KEY_W])
        ball_velocity.y -= 1.0f;

    if (io.KeysDown[GLFW_KEY_S])
        ball_velocity.y += 1.0f;

    if (io.KeysDown[GLFW_KEY_ESCAPE])
        ball_velocity = Vect(ball_initial_speed);

    // update ball position according
    // its velocity and elapsed time
    ball_position += ball_velocity * elapsed;

    if (ball_position.x < ball_radius)
    {
        ball_position.x += (ball_radius - ball_position.x) * 2.0f;
        ball_velocity.x *= -1.0f;

        add_debug_hit(debug_data, Vect(0, ball_position.y), Vect(1, 0));
    }
    else if (ball_position.x > (world_size.x - ball_radius))
    {
        ball_position.x -= (ball_position.x - (world_size.x - ball_radius)) * 2.0f;
        ball_velocity.x *= -1.0f;

        add_debug_hit(debug_data, Vect(world_size.x, ball_position.y), Vect(-1, 0));
    }

    if (ball_position.y < ball_radius)
    {
        ball_position.y += (ball_radius - ball_position.y) * 2.0f;
        ball_velocity.y *= -1.0f;

        add_debug_hit(debug_data, Vect(ball_position.x, 0), Vect(0, 1));
    }
    else if (ball_position.y > (world_size.y - ball_radius))
    {
        ball_position.y -= (ball_position.y - (world_size.y - ball_radius)) * 2.0f;
        ball_velocity.y *= -1.0f;

        add_debug_hit(debug_data, Vect(ball_position.x, world_size.y), Vect(0, -1));
    }
    // TODO:
    // Implement you Arkanoid user input handling
    // and game logic.
    // ...
    
    // TODO:
    // remove demo code
}

void ArkanoidImpl::draw(ImGuiIO& io, ImDrawList &draw_list)
{
    Vect ball_screen_pos = ball_position * world_to_screen;
    float ball_screen_radius = ball_radius * world_to_screen.x;
    draw_list.AddCircleFilled(ball_screen_pos, ball_screen_radius, ImColor(100, 255, 100));

    // TODO:
    // Implement you Arkanoid drawing
    // ...

    // TODO:
    // remove demo code
}

void ArkanoidImpl::add_debug_hit(ArkanoidDebugData& debug_data, const Vect& world_pos, const Vect& normal)
{
    ArkanoidDebugData::Hit hit;
    hit.screen_pos = world_pos * world_to_screen;
    hit.normal = normal;
    debug_data.hits.push_back(std::move(hit));
}

