#include "main.h"
#include "timer.h"
#include "ball.h"
#include "floor.h"
#include "trampoline.h"

using namespace std;

GLMatrices Matrices;
GLuint     programID;
GLFWwindow *window;

/**************************
* Customizable functions *
**************************/

typedef struct initPos{
    float x,y;
} initPos;

// For Game Progess And Game Diagonistics
int Score=0,Level=1,Lives=3;
Ball player;
Floor floor1,floor2,floor3,floor4,floor5;
Ball lev1[25];
Ball porcipine;
Ball pond;
initPos inP[25];
Ball Magnet;

Tram tramp;

float screen_zoom = 1, screen_center_x = 0, screen_center_y = 0;

Timer t60(1.0 / 60);
float pan=0;
int magtime = 0;
int magptime = 10;

/* Render the scene with openGL */
/* Edit this function according to your assignment */
void draw() {
    // clear the color and depth in the frame buffer
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use the loaded shader program
    // Don't change unless you know what you are doing
    glUseProgram (programID);

    // Eye - Location of camera. Don't change unless you are sure!!
    // glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );
    // Target - Where is the camera looking at.  Don't change unless you are sure!!
    // glm::vec3 target (0, 0, 0);
    // Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
    // glm::vec3 up (0, 1, 0);

    // Compute Camera matrix (view)
    // Matrices.view = glm::lookAt( eye, target, up ); // Rotating Camera for 3D
    // Don't change unless you are sure!!
    Matrices.view = glm::lookAt(glm::vec3(pan/8, 0, 3), glm::vec3(pan/8, 0, 0), glm::vec3(0, 1, 0)); // Fixed camera for 2D (ortho) in XY plane

    // Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
    // Don't change unless you are sure!!
    glm::mat4 VP = Matrices.projection * Matrices.view;

    // Send our transformation to the currently bound shader, in the "MVP" uniform
    // For each model you render, since the MVP will be different (at least the M part)
    // Don't change unless you are sure!!
    glm::mat4 MVP;  // MVP = Projection * View * Model

    // Scene render
    floor1.draw(VP);
    floor2.draw(VP);
    floor3.draw(VP);
    floor4.draw(VP);
    floor5.draw(VP);
    for(int e=0;e<25;e++)
    {
       lev1[e].draw(VP);
    }
    pond.draw(VP);
    if(Level == 2)
    {
        tramp.draw(VP);
        if(Score>=150)
            porcipine.draw(VP);
    }
    if(Level == 3)
    {

        tramp.draw(VP);
        porcipine.draw(VP);
        Magnet.draw(VP);
    }
    player.draw(VP);
}

void give_acc()
{
    if(pond_collision())
    {
        player.upvel = 1.1f;
        player.upvel1 = 1.1f;
     }
    else
    {
        player.upvel = 1.7f;
        player.upvel1 = 1.7f;
    }
}
void tick_input(GLFWwindow *window) {
    int A  = glfwGetKey(window, GLFW_KEY_A);
    int D = glfwGetKey(window, GLFW_KEY_D);
    int panr = glfwGetKey(window, GLFW_KEY_RIGHT);
    int panl = glfwGetKey(window, GLFW_KEY_LEFT);

    if (panr)
    {
        pan=pan+1;
    }
    if (panl)
    {
        pan=pan-1;
    }
    if(A)
    {
        player.rotation +=20;
        player.position.x -= 0.05;
    }
    else if(D)
    {
        player.rotation -=20;
        player.position.x += 0.05;
    }
}

void tick_elements() {
    if(Score>=100)
        Level = 2;
    if(Score>=200)
        Level = 3;
    if(!magptime && magtime%10!=0)
    {
        Magnet.position.y=10.0;
    }

    if(magptime == 10)
        magtime++;
    else
        magptime--;
    floor1.tick();
    floor2.tick();
    floor3.tick();
    floor4.tick();
    player.tick();
    for(int er=0;er<25;er++)
        lev1[er].tick();

    // For Pond related goals
    if(pond_collision())
    {
       pond_properties();
    }

    // For collision with porcipine
    porcipine.tick();
    porci_properties();

    if(floor_collision(floor1.bounding_box(),player.bounding_box()) && !pond_collision())
    {
        player.upvel=0.0;player.set_position(player.position.x,-1.6);player.speed=0.0;
    }
    for(int ri=0;ri<25;ri++)
    {
        if(lev1[ri].position.x >=5.5)
        {
            lev1[ri].position.x=inP[ri].x;
            lev1[ri].position.y=inP[ri].y;
        }
        if((sqrt(pow(player.position.x-lev1[ri].position.x,2)+pow(player.position.y-lev1[ri].position.y,2))<=0.75)
                &&(player.position.y>lev1[ri].position.y)&&(player.upvel1<0))
        {
            if(lev1[ri].plank == 1 && player.position.x > lev1[ri].position.x)
            {
                player.speed = -0.1;
            }
            else
            {
               // cout << '\a' << flush;

                lev1[ri].position.x = inP[ri].x;
                lev1[ri].position.y = inP[ri].y;
                player.upvel = 1.5f;
                player.upvel1 = 1.5f;
                player.uptime = 0.01;
                Score += 10;
              //  PlaySound("/home/nikhilendra_atheti/4-sem/Graphics/Assgiments/1A/20161054/src/Jai-Simha-Theme-Ringtone-1.mp3", NULL, SND_ASYNC|SND_FILENAME|SND_LOOP);
            }
        }

    }
    if((Level== 2 || Level==3) &&  abs(player.position.x - tramp.position.x) < 1.07
            && floor_collision(floor1.bounding_box(),player.bounding_box()) )
    {
        player.position.x = player.position.x-0.05;
        player.position.y = player.position.y;
    }
    if((Level==2 || Level==3)&& player.position.x >=tramp.position.x-0.65
            && player.position.x <= 4+tramp.position.x
            && player.position.y <= tramp.position.y)
    {
        player.uptime = 0.01;
        player.upvel = 2.0;player.set_position(player.position.x,-1.0);
    }

    // For magnet properties
    if(Level == 3)
        magnet_properties();

}

/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL(GLFWwindow *window, int width, int height) {
    /* Objects should be created before any other gl function and shaders */
    // Create the models
    GLfloat playerdata[360*10];
    int m=0;
    GLfloat x=0,y=-0.4;
    for(int r=0;r<360;r++)
    {
       playerdata[m]=x;m++;
       playerdata[m]=y;m++;
       playerdata[m]=0.0;m++;
       playerdata[m]=0.0;m++;
       playerdata[m]=0.0;m++;
       playerdata[m]=0.0;m++;
       GLfloat q,w;
       q=x*cos(((1)*M_PI)/180.0)-y*sin(((1)*M_PI)/180.0);
       w=x*sin(((1)*M_PI)/180.0)+y*cos(((1)*M_PI)/180.0);
       playerdata[m]=q;m++;
       playerdata[m]=w;m++;
       playerdata[m]=0.0;m++;
       x=q;
       y=w;
    }
    GLfloat flyingball[360*10];
    int m1=0;
    GLfloat x1=0,y1=-0.3;
    for(int r=0;r<360;r++)
    {
       flyingball[m1]=x1;m1++;
       flyingball[m1]=y1;m1++;
       flyingball[m1]=0.0;m1++;
       flyingball[m1]=0.0;m1++;
       flyingball[m1]=0.0;m1++;
       flyingball[m1]=0.0;m1++;
       GLfloat q,w;
       q=x1*cos(((1)*M_PI)/180.0)-y1*sin(((1)*M_PI)/180.0);
       w=x1*sin(((1)*M_PI)/180.0)+y1*cos(((1)*M_PI)/180.0);
       flyingball[m1]=q;m1++;
       flyingball[m1]=w;m1++;
       flyingball[m1]=0.0;m1++;
       x1=q;
       y1=w;
    }
    player      = Ball(2,-1.6,0.4,0.4,playerdata,m,COLOR_ORANGE,0.0,9,0.0,0.0);
    player.speed = 0.0;
    player.rotation = 25.0;
    player.ballcolor = COLOR_ORANGE;
    srand(time(0));
    for(int e=0;e<25;e++)
    {
        float ry=(rand()%5)-((rand()%2)*0.2);
        float rx=(rand()%10-15)+0.85;
        int pl=(rand()%(360*3))*3;
        if(e%5==0)
            lev1[e]=Ball(rx,ry,0.25,0.25,flyingball,m1,COLOR_YELLOW,(rand()%4+1)*0.01,1,-1*flyingball[pl],flyingball[pl+1]);
        else if(e%5==1)
            lev1[e]=Ball(rx,ry,0.25,0.25,flyingball,m1,COLOR_GREY,(rand()%4+1)*0.01,0,0,0);
        else if(e%5==2)
            lev1[e]=Ball(rx,ry,0.25,0.25,flyingball,m1,COLOR_BLUE,(rand()%4+1)*0.01,0,0,0);
        else if(e%5==3)
            lev1[e]=Ball(rx,ry,0.25,0.25,flyingball,m1,COLOR_RED,(rand()%4+1)*0.01,0,0,0);
        else if(e%5==4)
            lev1[e]=Ball(rx,ry,0.25,0.25,flyingball,m1,COLOR_GREEN,(rand()%4+1)*0.01,0,0,0);
        inP[e].x=rx;
        inP[e].y=ry;

    }
    // For the Floor bottom the screen
    floor1      = Floor(0, -2.5, COLOR_GRASS);
    floor2      = Floor(0, -3, COLOR_SOIL);
    floor3      = Floor(0, -4, COLOR_SOIL);
    // For the Floor top of the screen
    floor4      = Floor(0, 6.06, COLOR_LBLUE);
    floor5      = Floor(0, -5,COLOR_SOIL);
    // For the Floor sides of the screen;

    // For Trampoline on the screen
    GLfloat trampoline[180*10];
    int mt=0;
    GLfloat xt=-0.6,yt=0.0;
    for(int r=0;r<180;r++)
    {
       trampoline[mt]=xt;mt++;
       trampoline[mt]=yt;mt++;
       trampoline[mt]=0.0;mt++;
       trampoline[mt]=0.0;mt++;
       trampoline[mt]=0.0;mt++;
       trampoline[mt]=0.0;mt++;
       GLfloat q,w;
       q=xt*cos(((1)*M_PI)/180.0)-yt*sin(((1)*M_PI)/180.0);
       w=xt*sin(((1)*M_PI)/180.0)+yt*cos(((1)*M_PI)/180.0);
       trampoline[mt]=q;mt++;
       trampoline[mt]=w;mt++;
       trampoline[mt]=0.0;mt++;
       xt=q;
       yt=w;
    }
    trampoline[mt] = -0.6;mt++;
    trampoline[mt] = 0.0;mt++;
    trampoline[mt] = 0.0;mt++;

    trampoline[mt] = -0.65;mt++;
    trampoline[mt] = 0.0;mt++;
    trampoline[mt] = 0.0;mt++;

    trampoline[mt] = -0.6;mt++;
    trampoline[mt] = -0.6;mt++;
    trampoline[mt] = 0.0;mt++;

    trampoline[mt] = -0.6;mt++;
    trampoline[mt] = -0.6;mt++;
    trampoline[mt] = 0.0;mt++;

    trampoline[mt] = -0.65;mt++;
    trampoline[mt] = -0.6;mt++;
    trampoline[mt] = 0.0;mt++;

    trampoline[mt] = -0.65;mt++;
    trampoline[mt] = 0.0;mt++;
    trampoline[mt] = 0.0;mt++;


    trampoline[mt] = 0.6;mt++;
    trampoline[mt] = 0.0;mt++;
    trampoline[mt] = 0.0;mt++;

    trampoline[mt] = 0.65;mt++;
    trampoline[mt] = 0.0;mt++;
    trampoline[mt] = 0.0;mt++;

    trampoline[mt] = 0.6;mt++;
    trampoline[mt] = -0.6;mt++;
    trampoline[mt] = 0.0;mt++;

    trampoline[mt] = 0.6;mt++;
    trampoline[mt] = -0.6;mt++;
    trampoline[mt] = 0.0;mt++;

    trampoline[mt] = 0.65;mt++;
    trampoline[mt] = -0.6;mt++;
    trampoline[mt] = 0.0;mt++;

    trampoline[mt] = 0.65;mt++;
    trampoline[mt] = 0.0;mt++;
    trampoline[mt] = 0.0;mt++;

    tramp = Tram(4,-1.40,COLOR_LRED,trampoline,mt);


    // For Porcipines on the floor

    GLfloat proci[3*9];
    proci[0] = 0.0;
    proci[1] = 0.0;
    proci[2] = 0.0;

    proci[3] = 0.0;
    proci[4] = 0.3;
    proci[5] = 0.0;

    proci[6] = -0.5;
    proci[7] = 0.0;
    proci[8] = 0.0;

    proci[9] = -0.5;
    proci[10] = 0.0;
    proci[11] = 0.0;

    proci[12] = -1.0;
    proci[13] = 0.0;
    proci[14] = 0.0;

    proci[15] = -0.75;
    proci[16] = 0.3;
    proci[17] = 0.0;

    proci[18] = -1.0;
    proci[19] = 0.0;
    proci[20] = 0.0;

    proci[21] = -1.5;
    proci[22] = 0.0;
    proci[23] = 0.0;

    proci[24] = -1.5;
    proci[25] = 0.3;
    proci[26] = 0.0;

    porcipine = Ball(-8, -2, 0.3, 0.3, proci, 27, COLOR_LBLUE, 0.015,0,0,0);


    // For Pond in the Game
    GLfloat ponddata[180*9];
    int go=0;
    GLfloat xpd=-1.2,ypd=0;
    for(int r=0;r<180;r++)
    {
       ponddata[go]=xpd;go++;
       ponddata[go]=ypd;go++;
       ponddata[go]=0.0;go++;
       ponddata[go]=0.0;go++;
       ponddata[go]=0.0;go++;
       ponddata[go]=0.0;go++;
       GLfloat q,w;
       q=xpd*cos(((1)*M_PI)/180.0)-ypd*sin(((1)*M_PI)/180.0);
       w=xpd*sin(((1)*M_PI)/180.0)+ypd*cos(((1)*M_PI)/180.0);
       ponddata[go]=q;go++;
       ponddata[go]=w;go++;
       ponddata[go]=0.0;go++;
       xpd=q;
       ypd=w;
    }
    pond = Ball(0, -2, 1.2, 1.2, ponddata, go, COLOR_LBLUE, 0.0,0,0,0);

    // For Magnet Creation and rander it

    GLfloat magnet_data[360*11];
    int mm=0;
    GLfloat xma1=0.0,yma1=0.45,xma2=0.0,yma2=0.25;
    for(int gj=0;gj<180;gj++)
    {
        magnet_data[mm]=xma1;mm++;
        magnet_data[mm]=yma1;mm++;
        magnet_data[mm]=0.0;mm++;
        magnet_data[mm]=xma2;mm++;
        magnet_data[mm]=yma2;mm++;
        magnet_data[mm]=0.0;mm++;
        GLfloat q,w;
        q=xma1*cos(((1)*M_PI)/180.0)-yma1*sin(((1)*M_PI)/180.0);
        w=xma1*sin(((1)*M_PI)/180.0)+yma1*cos(((1)*M_PI)/180.0);
        magnet_data[mm]=q;mm++;
        magnet_data[mm]=w;mm++;
        magnet_data[mm]=0.0;mm++;
        magnet_data[mm]=q;mm++;
        magnet_data[mm]=w;mm++;
        magnet_data[mm]=0.0;mm++;
        xma1=q;
        yma1=w;
        q=xma2*cos(((1)*M_PI)/180.0)-yma2*sin(((1)*M_PI)/180.0);
        w=xma2*sin(((1)*M_PI)/180.0)+yma2*cos(((1)*M_PI)/180.0);
        magnet_data[mm]=q;mm++;
        magnet_data[mm]=w;mm++;
        magnet_data[mm]=0.0;mm++;
        magnet_data[mm]=xma2;mm++;
        magnet_data[mm]=yma2;mm++;
        magnet_data[mm]=0.0;mm++;
        xma2=q;
        yma2=w;
    }
    magnet_data[mm] = 0.0;mm++; magnet_data[mm] = 0.45;mm++; magnet_data[mm] = 0.0;mm++;
    magnet_data[mm] = 0.0;mm++; magnet_data[mm] = 0.25;mm++; magnet_data[mm] = 0.0;mm++;
    magnet_data[mm] = 0.4;mm++; magnet_data[mm] = 0.25;mm++; magnet_data[mm] = 0.0;mm++;

    magnet_data[mm] = 0.0;mm++; magnet_data[mm] = 0.45;mm++; magnet_data[mm] = 0.0;mm++;
    magnet_data[mm] = 0.4;mm++; magnet_data[mm] = 0.25;mm++; magnet_data[mm] = 0.0;mm++;
    magnet_data[mm] = 0.4;mm++; magnet_data[mm] = 0.45;mm++; magnet_data[mm] = 0.0;mm++;

    magnet_data[mm] = 0.0;mm++; magnet_data[mm] = -0.45;mm++; magnet_data[mm] = 0.0;mm++;
    magnet_data[mm] = 0.0;mm++; magnet_data[mm] = -0.25;mm++; magnet_data[mm] = 0.0;mm++;
    magnet_data[mm] = 0.4;mm++; magnet_data[mm] = -0.25;mm++; magnet_data[mm] = 0.0;mm++;

    magnet_data[mm] = 0.0;mm++; magnet_data[mm] = -0.45;mm++; magnet_data[mm] = 0.0;mm++;
    magnet_data[mm] = 0.4;mm++; magnet_data[mm] = -0.25;mm++; magnet_data[mm] = 0.0;mm++;
    magnet_data[mm] = 0.4;mm++; magnet_data[mm] = -0.45;mm++; magnet_data[mm] = 0.0;mm++;

    Magnet = Ball(-3.5,1.0,0.15,0.15,magnet_data,mm,COLOR_MAGNET,0.0,3,0,0);

    // Create and compile our GLSL program from the shaders
    programID = LoadShaders("Sample_GL.vert", "Sample_GL.frag");
    // Get a handle for our "MVP" uniform
    Matrices.MatrixID = glGetUniformLocation(programID, "MVP");


    reshapeWindow (window, width, height);

    // Background color of the scene
    glClearColor (COLOR_BACKGROUND.r / 256.0, COLOR_BACKGROUND.g / 256.0, COLOR_BACKGROUND.b / 256.0, 0.0f); // R, G, B, A
    glClearDepth (1.0f);

    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_LEQUAL);

    cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
    cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
    cout << "VERSION: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}


int main(int argc, char **argv) {
    srand(time(0));
    int width  = 1000;
    int height = 1000;

    window = initGLFW(width, height);

    initGL (window, width, height);

    /* Draw in loop */
    while (!glfwWindowShouldClose(window)) {
        // Process timers

        if (t60.processTick()) {
            // 60 fps
            // OpenGL Draw commands
            draw();
            string Result;
                               stringstream convert;
                               //cout << "nonedit score:" << score << endl;
                               convert << Score;
                               Result = convert.str();

                               const char *one = "Score: ";
                               const char *two = Result.c_str();
                               const char *three = "   Level: ";
                               string Result1;
                               stringstream convert1;
                               convert1 << Level;
                               Result1 = convert1.str();
                               const char *four = Result1.c_str();
                               const char *five = " Lives: ";
                               string Result3;
                               stringstream convert2;
                               convert2 << Lives;
                               Result3 = convert2.str();
                               const char *six = Result3.c_str();
                               string total( string(one) + two + string(three) + four + string(five) + six );
                               glfwSetWindowTitle(window,total.c_str());
            // Swap Frame Buffer in double buffering
            glfwSwapBuffers(window);

            tick_elements();
            tick_input(window);
        }
        if(Lives==0)
            break;
        // Poll for Keyboard and mouse events*/
        glfwPollEvents();
    }

    quit(window);
}

bool detect_collision(bounding_box_t a, bounding_box_t b) {
    return (abs(a.x - b.x) * 2 < (a.width + b.width)) &&
           (abs(a.y - b.y) * 2 < (a.height + b.height));
}
bool floor_collision(bounding_box_t a, bounding_box_t b) {
    return (abs(a.y - b.y) * 2 <= ( a.height + b.height));
}

void reset_screen() {
    float top    = screen_center_y + 5 / screen_zoom;
    float bottom = screen_center_y - 5 / screen_zoom;
    float left   = screen_center_x - 5 / screen_zoom;
    float right  = screen_center_x + 5 / screen_zoom;
    Matrices.projection = glm::ortho(left, right, bottom, top, 0.1f, 500.0f);
}


bool pond_collision()
{
    return ((player.position.x>=0 && player.position.x<=1.2)
            || (player.position.x<=0 && player.position.x>=-1.2)) && player.position.y<=-1.6;
}


int df = 0;
void pond_properties()
{

    if(abs(player.position.x-pond.position.x)<0.001)
    {
        player.position.x = pond.position.x;
        player.position.y = -2.8;

            player.speed = 0.0;
            player.upvel = 0.0;
            player.upvel1 = 0.0;
            player.uptime = 0.01;
            df=1;
    }
    else if(player.position.x > pond.position.x)
    {
        player.position.x -=0.03 ;
        player.position.y = -sqrt(abs(pow(1.2,2)-pow(abs(player.position.x-pond.position.x),2)))-2.0+0.4;
        player.rotation +=10;

    }
    else
    {
        player.position.x +=0.03 ;
        player.position.y = -sqrt(abs(pow(1.2,2)-pow(abs(player.position.x-pond.position.x),2)))-2.0+0.4;
        player.rotation -=10;
    }
    if( df && abs(player.position.x-pond.position.x)>=0.8)
    {
        if(player.position.x > pond.position.x)
        {
            player.position.x =1.65;
            player.position.y=-1.6;
        }
        else
        {
            player.position.x =-1.65;
            player.position.y=-1.6;
        }
        df=0;
    }
    return;
}

void magnet_properties()
{
    if(player.position.y >= 0.55 && player.position.y<=1.45 && player.position.x - 0.40 >=-2.99)
    {
        player.position.x = player.position.x - 0.05;
    }
    return;
}
 void porci_properties()
 {
     if(porcipine.position.x >=-3.5 || porcipine.position.x<=-8)
     {
                porcipine.speed = -porcipine.speed;
     }
     if((player.position.x<=porcipine.position.x
         && player.position.x>=porcipine.position.x-1.5
         && abs(player.position.y-porcipine.position.y)<0.7))
            //) //abs(player.position.x-porcipine.position.x)<=1.55)
     {
         porcipine.position.x = -8;
         if(porcipine.speed>0)
                porcipine.speed = -porcipine.speed;
         Lives -=1;

     }
     if(abs(player.position.x+0.4-porcipine.position.x)<0.60 && player.position.y==-1.6)
     {
         porcipine.position.x = -8;
         if(porcipine.speed>0)
                porcipine.speed = -porcipine.speed;
        Lives -= 1;
     }

 }
void screen_zoom_in(){
    screen_zoom -= 0.1;
    reset_screen();
}
void screen_zoom_out(){
    screen_zoom += 0.1;
    reset_screen();
}
void plank_collision()
{
    player.speed = 0.5;
}
