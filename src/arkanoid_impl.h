#pragma once

#include "arkanoid.h"

#include <vector>

#define USE_ARKANOID_IMPL

class Brick {
public:
    Brick();
    Brick(Vect _upper_left_corner, Vect _bottom_right_corner);

    Vect upper_left_corner;
    Vect bottom_right_corner;
    bool alive;

    bool IsAlive();
    void Destroy();
    void SetCorners(Vect upper_left_corner, Vect bottom_right_corner);
};

class ArkanoidImpl : public Arkanoid
{
public:
    void reset(const ArkanoidSettings& settings) override;
    void update(ImGuiIO& io, ArkanoidDebugData& debug_data, float elapsed) override;
    void draw(ImGuiIO& io, ImDrawList& draw_list) override;

private:
    void add_debug_hit(ArkanoidDebugData& debug_data, const Vect& pos, const Vect& normal);
    
    Vect world_size = Vect(0.0f);
    Vect world_to_screen = Vect(0.0f);

    Vect ball_position = Vect(0.0f);
    Vect ball_velocity = Vect(0.0f);
    
    Vect carriage_position = Vect(0.0f);

    int bricks_columns_count = 15;
    int bricks_rows_count = 7;

    std::vector<Brick> bricks;

    float brick_width;
    float brick_height = 20;

    float bricks_columns_padding = 5.0f;
    float bricks_rows_padding = 5.0f;

    float ball_radius = 0.0f;
    float ball_initial_speed = 0.0f;

    float carriage_width = 0.0f;
    const float carriage_height = 20;
    const float carriage_shift = 20;
};
