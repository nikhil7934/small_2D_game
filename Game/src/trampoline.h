#include "main.h"

#ifndef TRPOLINE_H
#define TRPOLINE_H


class Tram {
public:
    Tram() {}
    Tram(float x, float y, color_t color,GLfloat vertex_buffer_data[],int u);
    glm::vec3 position;
    float rotation;
    void draw(glm::mat4 VP);
    void set_position(float x, float y);
    void tick();
    double speed;
    bounding_box_t bounding_box();
private:
    VAO *object;
};

#endif // TRPOLINE_H
