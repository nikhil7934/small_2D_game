#include "floor.h"
#include "main.h"

Floor::Floor(float x, float y, color_t color) {
    this->position = glm::vec3(x, y, 0);
    this->rotation = 0;
    speed = 0.00;
    static const GLfloat vertex_buffer_data[] = {
        -15.0, 0.5, 0, // vertex 1
        15.0,  0.5, 0, // vertex 2
        15.0,  -0.5, 0, // vertex 3

        15.0,  -0.5, 0, // vertex 3
        -15.0, -0.5, 0, // vertex 4
        -15.0, 0.5, 0 // vertex 1
    };

    this->object = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color, GL_FILL);
}

void Floor::draw(glm::mat4 VP) {
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translate = glm::translate (this->position);    // glTranslatef
    glm::mat4 rotate    = glm::rotate((float) (this->rotation * M_PI / 180.0f), glm::vec3(0, 0, 1));
    rotate          = rotate * glm::translate(glm::vec3(0, 0,0));
    Matrices.model *= (translate * rotate);
    glm::mat4 MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw3DObject(this->object);
}

void Floor::set_position(float x, float y) {
    this->position = glm::vec3(x, y, 0);
}

void Floor::tick() {
    //this->position.x -= speed;
    // this->position.y -= speed;
    return;
}

bounding_box_t Floor::bounding_box() {
    float x = this->position.x, y = this->position.y;
    bounding_box_t bbox = { x, y, 1, 1 };
    return bbox;
}
