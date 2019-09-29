//
//  entity.h
//  hw_2
//
//  Created by Gordon Lei on 9/25/19.
//  Copyright © 2019 ctg. All rights reserved.
//

#ifndef entity_h
#define entity_h
#pragma once
#define GL_SILENCE_DEPRECATION

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"


class Entity {
public:
    
    glm::vec3 position;
    glm::vec3 movement;
    
    
    float speed;
    float height;
    float width;
    
    GLuint textureID;
    
    Entity();
    
    void Update(float deltaTime);
    void Render(ShaderProgram *program);
};


#endif /* entity_h */
