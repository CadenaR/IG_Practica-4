#ifndef MODEL_H
#define MODEL_H

#include <iostream>
#include <vector>
#include <ogl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model {
    
    public:

        Model(const char *modelFile);

        void render(unsigned long mode);
         
        virtual ~Model();
         
    private:
        
        std::vector<glm::vec3>      positions;
        std::vector<glm::vec3>      normals;
        std::vector<glm::vec2>      textureCoords;
        std::vector<unsigned short> indices;
    
        unsigned int vao;
        unsigned int vboPositions;
        unsigned int vboNormals;
        unsigned int vboTextureCoords;
        unsigned int eboIndices;

};

#endif /* MODEL_H */