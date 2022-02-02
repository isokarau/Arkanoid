#include "arkanoid_impl.h"

#include <GLFW/glfw3.h>

#include <algorithm>
#include <iostream> // for debug

#ifdef USE_ARKANOID_IMPL
Arkanoid* create_arkanoid()
{
    return new ArkanoidImpl();
}
#endif

Brick::Brick() : alive(true) {}

Brick::Brick(Vect _upper_left_corner, Vect _bottom_right_corner) :
    upper_left_corner(_upper_left_corner),
    bottom_right_corner(_bottom_right_corner),
    alive(true)
{}

bool Brick::IsAlive() {
    return alive;
}

void Brick::Destroy() {
    alive = false;
}

void Brick::SetCorners(Vect _upper_left_corner, Vect _bottom_right_corner) {
    upper_left_corner = _upper_left_corner;
    bottom_right_corner = _bottom_right_corner;
}

std::vector<Brick> BuildBricks(const ArkanoidSettings& settings, float brick_width, float brick_height) {
    std::vector<Brick> bricks(settings.bricks_rows_count * settings.bricks_columns_count);

    for (size_t i = 0; i < settings.bricks_rows_count; ++i) {
        for (size_t j = 0; j < settings.bricks_columns_count; ++j) {
            Vect upper_left_corner = { j * (settings.bricks_columns_padding + brick_width),
                i * (settings.bricks_rows_padding + brick_height)};
            Vect bottom_right_corner = { upper_left_corner.x + brick_width,
                upper_left_corner.y + brick_height };
            bricks[i * settings.bricks_columns_count + j].SetCorners(upper_left_corner, bottom_right_corner);
        }
    }
    return bricks;
}

void ArkanoidImpl::reset(const ArkanoidSettings &settings)
{
    world_size.x = settings.world_size[0];
    world_size.y = settings.world_size[1];

    bricks_columns_count = settings.bricks_columns_count;
    bricks_rows_count = settings.bricks_rows_count;
    bricks_columns_padding = settings.bricks_columns_padding;
    bricks_rows_padding = settings.bricks_rows_padding;
    brick_width = (world_size.x - (bricks_columns_count - 1) * bricks_columns_padding) / bricks_columns_count;

    bricks = BuildBricks(settings, brick_width, brick_height);

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
    if (io.KeysDown[GLFW_KEY_A]) {
        ball_velocity.x -= 1.0f;
        carriage_position.x -= std::min(carriage_shift, carriage_position.x - carriage_width * 0.5f);
    }

    if (io.KeysDown[GLFW_KEY_D]) {
        ball_velocity.x += 1.0f;
        carriage_position.x += std::min(carriage_shift, world_size.x - (carriage_position.x + carriage_width * 0.5f));
    }

    if (io.KeysDown[GLFW_KEY_W])
        ball_velocity.y -= 1.0f;

    if (io.KeysDown[GLFW_KEY_S])
        ball_velocity.y += 1.0f;

    if (io.KeysDown[GLFW_KEY_ESCAPE])
        ball_velocity = Vect(ball_initial_speed);

    // update ball position according
    // its velocity and elapsed time
    ball_position += ball_velocity * elapsed;

    if (ball_position.y > (carriage_position.y - carriage_height - ball_radius) &&
        ball_position.x >= carriage_position.x - carriage_width * 0.5 &&
        ball_position.x <= carriage_position.x) // if the ball hits to the left side of carriage
    {
        std::cout << "Hit to the left side" << std::endl;
        ball_position.y -= (ball_position.y - (carriage_position.y - carriage_height - ball_radius)) * 2.0f;
        ball_velocity.y *= -1.0f;
        ball_velocity.x = -abs(ball_velocity.x);
    }
    else if (ball_position.y > (carriage_position.y - carriage_height - ball_radius) &&
             ball_position.x <= carriage_position.x + carriage_width * 0.5 && 
             ball_position.x > carriage_position.x)  // if the ball hits to the right side of carriage
    {
        std::cout << "Hit to the right side" << std::endl;
        ball_position.y -= (ball_position.y - (carriage_position.y - carriage_height - ball_radius)) * 2.0f;
        ball_velocity.y *= -1.0f;
        ball_velocity.x = abs(ball_velocity.x);
    } 
    
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
    for (auto& brick : bricks) {
        if (brick.alive) {
            if (ball_position.x >= brick.upper_left_corner.x && ball_position.x <= brick.bottom_right_corner.x &&
                ball_position.y < brick.bottom_right_corner.y + ball_radius &&
                ball_position.y >= brick.bottom_right_corner.y)
            {
                ball_position.y += (brick.bottom_right_corner.y + ball_radius - ball_position.y) * 2.0f;
                ball_velocity.y *= -1.0f;
                brick.alive = false;
                add_debug_hit(debug_data, Vect(ball_position.x, brick.bottom_right_corner.y), Vect(0, 1));
            }
            else if (ball_position.x >= brick.upper_left_corner.x && ball_position.x <= brick.bottom_right_corner.x &&
                ball_position.y > brick.upper_left_corner.y - ball_radius &&
                ball_position.y <= brick.upper_left_corner.y)
            {
                ball_position.y -= ball_position.y - (brick.upper_left_corner.y - ball_radius) * 2.0f;
                ball_velocity.y *= -1.0f;
                brick.alive = false;
                add_debug_hit(debug_data, Vect(ball_position.x, brick.upper_left_corner.y), Vect(0, -1));
            }
            
            if (ball_position.y >= brick.upper_left_corner.y && ball_position.y <= brick.bottom_right_corner.y &&
                ball_position.x < brick.bottom_right_corner.x + ball_radius &&
                ball_position.x >= brick.bottom_right_corner.x) 
            {
                ball_position.x += (brick.bottom_right_corner.x + ball_radius - ball_position.x) * 2.0f;
                ball_velocity.x *= -1.0f;
                brick.alive = false;
                add_debug_hit(debug_data, Vect(brick.bottom_right_corner.x, ball_position.y), Vect(1, 0));
            } else if (ball_position.y >= brick.upper_left_corner.y && ball_position.y <= brick.bottom_right_corner.y &&
                ball_position.x > brick.upper_left_corner.x - ball_radius &&
                ball_position.x <= brick.upper_left_corner.x)
            {
                ball_position.x -= (ball_position.x - (brick.upper_left_corner.x - ball_radius)) * 2.0f;
                ball_velocity.x *= -1.0f;
                brick.alive = false;
                add_debug_hit(debug_data, Vect(brick.upper_left_corner.x, ball_position.y), Vect(-1, 0));
            }
        }
    }
}

void ArkanoidImpl::draw(ImGuiIO& io, ImDrawList &draw_list)
{
    Vect ball_screen_pos = ball_position * world_to_screen;
    float ball_screen_radius = ball_radius * world_to_screen.x;
    draw_list.AddCircleFilled(ball_screen_pos, ball_screen_radius, ImColor(100, 255, 100)); // drawing ball

    Vect carriage_screen_pos = carriage_position * world_to_screen;
    float carriage_screen_width = carriage_width * world_to_screen.x;
    float carriage_screen_height = carriage_height * world_to_screen.y;
    draw_list.AddRectFilled( // drawing carriage
            {carriage_screen_pos.x - carriage_screen_width * 0.5f, carriage_screen_pos.y - carriage_screen_height}, 
            {carriage_screen_pos.x + carriage_screen_width * 0.5f, carriage_screen_pos.y}, 
            ImColor(221, 160, 221));

    for (const auto& brick : bricks) { //drawing bricks
        if (brick.alive) {
            Vect block_screen_pos_upper_left = brick.upper_left_corner * world_to_screen;
            Vect block_screen_pos_bottom_right = brick.bottom_right_corner * world_to_screen;
            draw_list.AddRectFilled(
                block_screen_pos_upper_left, block_screen_pos_bottom_right, ImColor(255, 160, 122));

        }
    }
}

void ArkanoidImpl::add_debug_hit(ArkanoidDebugData& debug_data, const Vect& world_pos, const Vect& normal)
{
    ArkanoidDebugData::Hit hit;
    hit.screen_pos = world_pos * world_to_screen;
    hit.normal = normal;
    debug_data.hits.push_back(std::move(hit));
}

