#include "main.h"

#ifndef BALL_H
#define BALL_H


class Ball {
public:
    Ball() {}
    Ball(float x, float y, float w, float h, GLfloat vertex_buffer_data[],int u, color_t color,float sp,int plank,float xplank, float yplank);
    glm::vec3 position;
    float rotation;
    float width;
    float height;
    void draw(glm::mat4 VP);
    void set_position(float x, float y);
    void set_speed(float x, float y);
    void tick();
    int plank;
    float radius;
    float xplank;
    float yplank;
    double speed;
    double upvel;
    double upvel1;
    color_t ballcolor;
    double uptime;
    bounding_box_t bounding_box();
private:
    VAO *object[2];
};

#endif // BALL_H
