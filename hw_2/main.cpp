#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "entity.h"



SDL_Window* displayWindow;
bool gameIsRunning = true;
bool acceptInputs = true;


ShaderProgram tex_program, non_tex_program;
glm::mat4 viewMatrix, projectionMatrix;//, p1_matrix, p2_matrix, ball_matrix;

//Global variables
float lastTicks = 0;
//float bounces = 0.0f;
float bounce_spd_inc = 0.5f;


//Game windoe dimensions
int window_width = 640;
int window_height = 480;
float ortho_x = 5.0f;
float ortho_y = ortho_x * ((float) window_height) / ((float) window_width);
//Entity (Players + Balls)
Entity p1, p2, ball;




//Load the texture
GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(image);
    return textureID;
}


//initialize
void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Textured", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    tex_program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    non_tex_program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");
    
    
    viewMatrix = glm::mat4(1.0f);
    //p1_matrix = glm::mat4(1.0f);
    //p2_matrix = glm::mat4(1.0f);
    //ball_matrix = glm::mat4(1.0f);
    
    //10 pixel width and 7.5 height.
    //so height is 75% of 10.
    //640 * 0.75 = 480
    projectionMatrix = glm::ortho(-(ortho_x), ortho_x, -(ortho_y), ortho_y, -1.0f, 1.0f);
    
    //Add stuff for the entities
    p1.position = {-4.9,0,0};
    p1.height = 1.4;
    p1.width = 0.2;
    p1.speed = 4;
    
    p2.position = {4.9,0,0};
    p2.height = 1.4;
    p2.width = 0.2;
    p2.speed = 4;
    
    ball.position = {0,0,0};
    
    ball.movement = {1,1,0};
    ball.movement = glm::normalize(ball.movement);

    ball.height = 0.2;
    ball.width = 0.2;
    ball.speed = 1;
    
    
    //shaders if we need
    tex_program.SetProjectionMatrix(projectionMatrix);
    tex_program.SetViewMatrix(viewMatrix);
    tex_program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    non_tex_program.SetProjectionMatrix(projectionMatrix);
    non_tex_program.SetViewMatrix(viewMatrix);
    non_tex_program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    glUseProgram(tex_program.programID);
    glUseProgram(non_tex_program.programID);
    
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}


//check inputs
void ProcessInput() {
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
            /*
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        //
                        break;
                    case SDLK_RIGHT:
                        //
                        break;
                }
             */
        }
             
    }
    
    // Check for pressed/held keys below
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    /*
    if (keys[SDL_SCANCODE_A])
    {
        p1.movement[0] = -1;
    }
    else if  (keys[SDL_SCANCODE_D])
    {
        p1.movement[0] = 1;
    }
    */
    if (keys[SDL_SCANCODE_W])
    {
        if(p1.position[1] + p1.height/2 + 0.01 >= ortho_y){
            p1.movement[1] = 0;
        }
        else{
            p1.movement[1] = 1;
        }
        
    }
    else if  (keys[SDL_SCANCODE_S])
    {
        if(p1.position[1] - p1.height/2 - 0.01 <= -1 * ortho_y){
            p1.movement[1] = 0;
        }
        else{
            p1.movement[1] = -1;
        }
    }
    
    
    else if (keys[SDL_SCANCODE_UP])
    {
        if(p2.position[1] + p2.height/2 + 0.01 >= ortho_y){
            p2.movement[1] = 0;
        }
        else{
            p2.movement[1] = 1;
        }
        
    }
    else if  (keys[SDL_SCANCODE_DOWN])
    {
        if(p2.position[1] - p2.height/2 - 0.01 <= -1 * ortho_y){
            p2.movement[1] = 0;
        }
        else{
            p2.movement[1] = -1;
        }
    }
    
    
    /*
    if (glm::length(p1.movement) > 1.0f)
    {
        p1.movement = glm::normalize(p1.movement);
    }
    */
    
    
    /*
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
     */
    
    
    
}

void check_ball_collide_player(Entity* ball, Entity* p1, Entity* p2){
    //float xdist = 0.0f;
    //float ydist = 0.0f;
    
    //check if ball is colliding with p1
    float xdist = fabs(p1 -> position[0] - ball -> position[0]) - (ball -> width + p1 -> width)/2.0f;
    float ydist = fabs(p1 -> position[1] - ball -> position[1]) - (ball -> height + p1 -> height)/2.0f;
    if (xdist < 0 && ydist < 0){
        //collide, change ball movement + increase the SPEED
        if (ball -> movement[0] < 0){
            ball -> movement[0] -= bounce_spd_inc; //
            //ball -> movement = glm::normalize(ball -> movement);
        }
        else{
            ball -> movement[0] += bounce_spd_inc; //
            //ball -> movement = glm::normalize(ball -> movement);
        }
        ball -> movement[0] = -1 * ball -> movement[0];
    }
    else{
        xdist = fabs(p2 -> position[0] - ball -> position[0]) - (ball -> width + p1 -> width)/2.0f;
        ydist = fabs(p2 -> position[1] - ball -> position[1]) - (ball -> height + p1 -> height)/2.0f;
        if (xdist < 0 && ydist < 0){
            //collide, change ball movement and INCRESE THE SPEED
            if (ball -> movement[1] < 0){
                ball -> movement[1] -= bounce_spd_inc; //
                //ball -> movement = glm::normalize(ball -> movement);
            }
            else{
                ball -> movement[1] += bounce_spd_inc; //
                //ball -> movement = glm::normalize(ball -> movement);
            }
            ball -> movement[0] = -1 * ball -> movement[0];
        }
    }
}

void check_ball_collide_border(Entity* ball){
    //check if ball hits ceiling or floor.
    if((ball -> position[1] + (ball -> height)/2 + 0.01 >= ortho_y) || (ball -> position[1] - (ball -> height)/2 - 0.01 <= -1 * ortho_y)){
            ball -> movement[1] *= -1;
    }
    
    else if((ball -> position[0] + (ball -> width)/2 + 0.01 >= ortho_x) || (ball -> position[0] - (ball -> width)/2 - 0.01 <= -1 * ortho_x)){
        //gameIsRunning = false;
        ball -> movement = {0,0,0};
        acceptInputs = false;
    }

    
    
}

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    //do stuff
    //p1.position += p1.movement * deltaTime;
    p1.Update(deltaTime);
    //p2.position += p2.movement * deltaTime;
    p2.Update(deltaTime);
    check_ball_collide_player(&ball, &p1, &p2);
    check_ball_collide_border(&ball);
    ball.Update(deltaTime);
    
    
    //p1_matrix = glm::mat4(1.0f);
    //p1_matrix = glm::translate(p1_matrix, p1.position);
    //p2_matrix = glm::mat4(1.0f);
    //p2_matrix = glm::translate(p2_matrix, p2.position);
    
    
    p1.movement = glm::vec3(0);
    p2.movement = glm::vec3(0);
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    p1.Render(&non_tex_program);
    p2.Render(&non_tex_program);
    ball.Render(&non_tex_program);
    
    SDL_GL_SwapWindow(displayWindow);
    
    
}

void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        if(acceptInputs){
            ProcessInput();
        }
        Update();
        Render();
    }
    
    Shutdown();
    return 0;
}

