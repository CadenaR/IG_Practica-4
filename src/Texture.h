#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>
#include <ogl/glew.h>
#include <freeimage/freeImage.h>

class Texture {
    
    public:
        
        Texture(const char *textureFile);
        
        unsigned int texture;  

        virtual ~Texture();
           
    private:
       
        unsigned char *loadTexture(const char *textureFile, unsigned int &w, unsigned int &h);
        
};

#endif /* TEXTURE_H */
