#include "ball.h"
#include "main.h"

Ball::Ball(float x, float y, float w, float h, GLfloat vertex_buffer_data[],int u, color_t color,float sp,\
           int plank,float xplank,float yplank) {
    this->position = glm::vec3(x, y, 0);
    this->rotation = 0;
    this->width = w;
    this->height= h;
    this->uptime = 0.01;
    //speed = 0.00;
    this->speed = -1*sp;
    this->upvel = 0.00;
    this->upvel1 = this->upvel;
    this->plank = plank;
    this->xplank = xplank;
    this->yplank = yplank;
    this->radius = w;

    if(plank==9)
    {
        this->object[0] = create3DObject(GL_TRIANGLES, u/6, vertex_buffer_data, COLOR_LGREEN, GL_FILL);
        this->object[1] = create3DObject(GL_TRIANGLES, u/6, &vertex_buffer_data[u/2], COLOR_WHITE, GL_FILL);
    }
    else if(plank==0)
    {
        this->object[0] = create3DObject(GL_TRIANGLES, u/3, vertex_buffer_data, color, GL_FILL);
        this->object[1] = create3DObject(GL_TRIANGLES, u/3, vertex_buffer_data, color, GL_FILL);
    }
    else if(plank==1)
    {
        static const GLfloat plank_buffer_data[]={
            (0.3)*sqrt(2),0.0,0.0,
            0.0,(0.3)*sqrt(2),0.0,
            (0.3)*sqrt(2)+0.075,0.075,0.0,

            (0.3)*sqrt(2)+0.075,0.075,0.0,
            0.0,(0.3)*sqrt(2),0.0,
            0.075,(0.3)*sqrt(2)+0.075,0.0,
        };
        this->object[0] = create3DObject(GL_TRIANGLES, u/3, vertex_buffer_data, color, GL_FILL);
        this->object[1] = create3DObject(GL_TRIANGLES,6,plank_buffer_data,COLOR_BROWN,GL_FILL);
    }
    else if(plank==3)
    {
        static const GLfloat mag_buffer_data[]={
            0.4,0.45 ,0.0,
            0.4,0.25 ,0.0,
            0.6,0.25 ,0.0,
            0.6,0.25,0.0,
            0.4,0.45,0.0,
            0.6,0.45,0.0,

            0.4,-0.45 ,0.0,
            0.4,-0.25 ,0.0,
            0.6,-0.25 ,0.0,
            0.6,-0.25,0.0,
            0.4,-0.45,0.0,
            0.6,-0.45,0.0,
        };
        this->object[0] = create3DObject(GL_TRIANGLES, u/3, vertex_buffer_data, color, GL_FILL);
        this->object[1] = create3DObject(GL_TRIANGLES,12,mag_buffer_data,COLOR_WHITE,GL_FILL);
    }
}

void Ball::draw(glm::mat4 VP) {
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translate = glm::translate (this->position);    // glTranslatef
    glm::mat4 rotate    = glm::rotate((float) (this->rotation * M_PI / 180.0f), glm::vec3(0, 0, 1));
    rotate          = rotate * glm::translate(glm::vec3(0, 0, 0));
    Matrices.model *= (translate * rotate);
    glm::mat4 MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw3DObject(this->object[0]);
    draw3DObject(this->object[1]);
}

void Ball::set_speed(float x,float y)
{
    this->speed = x;
    this->upvel = y;
    this->upvel1 =y;
}

void Ball::set_position(float x, float y) {
    this->position = glm::vec3(x, y, 0);
}

void Ball::tick() {
    if(this->speed)
        this->position.x -= this->speed;
    if(this->upvel)
    {
        this->upvel1 = this->upvel-(10*this->uptime);
        this->position.y += ((this->upvel)*this->uptime)-(5*(this->uptime*this->uptime));
        this->uptime = this->uptime +0.01;
    }
    else
    {
        this->uptime=0.01;
    }

}

bounding_box_t Ball::bounding_box() {
    float x = this->position.x, y = this->position.y;
    float we = this->width,he=this->height;
    bounding_box_t bbox = { x, y, 2*we, 2*he };
    return bbox;
}
