#include <iostream>
#include <ogl/glew.h>
#include <ogl/freeglut.h>
#include <limits>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shaders.h"
#include "Model.h"

#include <sstream>
std::string toString(const int &i) {
    std::stringstream ss;
    ss << i;
    return ss.str();
}

#define I glm::mat4(1.0f)

void funInit();
void funDestroy();
void funReshape(int w, int h);
void funDisplay();
void funTimer(int value);
void mouseButton(int button, int dir, int x, int y);
void mouseMovement(int x, int y);
void funKeyboard(unsigned char key, int x, int y);
void keyboardArrows(int key, int x, int y);

void drawSuelo (glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawRobot(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawBrazoTex(glm::mat4 P, glm::mat4 V, glm::mat4 M, Textures textures,float lon , float rad);
void drawBrazoMat(glm::mat4 P, glm::mat4 V, glm::mat4 M, Material material,float lon , float rad);
void drawArticulacionTex(glm::mat4 P, glm::mat4 V, glm::mat4 M, Textures textures, float rad);
void drawArticulacionMat(glm::mat4 P, glm::mat4 V, glm::mat4 M, Material material, float rad);
void drawBase(glm::mat4 P, glm::mat4 V, glm::mat4 M);

void drawObjectMat(Model *object, Material material, glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawObjectTex(Model *object, Textures textures, glm::mat4 P, glm::mat4 V, glm::mat4 M);
void setLights(glm::mat4 P, glm::mat4 V);

// Shaders
   Shaders *shaders;
   
// Modelos
   Model* plane;
   Model* sphere;
   Model* cylinder;
   
// Viewport
   int w = 500;
   int h = 500;
   
// Dimensiones de los modelos cilindro y esfera
   float rCyl = 1.0f;
   float aCyl = 2.0f;
   float rEsf = 2.0f;
   
//Animaciones
   GLint speed = 30;
   float cFovy = 30.0;
   bool btnIzq = false; 
   float alphaX = 0;
   float alphaY = 0;
   float intensidad = 1.0f;
   float rotLuz = 270.0f;
   
// Luces
   #define  NLD 1
   #define  NLP 1
   #define  NLF 1
   Light    lightG;
   Light    lightD[NLD];
   Light    lightP[NLP];
   Light    lightF[NLF];
   
// Materiales
   Material matRuby;
   Material matEmeraldBrazo;
   Material matEmeraldEsfera;
   
// Texturas
   Texture  *texNoEmissive;
   Textures texSuelo;
   Textures texBrazo;
   Textures texBase;
   Textures texLuz;
   
int main(int argc, char** argv) {

 // Inicializamos GLUT
    glutInit(&argc, argv);
    glutInitContextVersion(3,3);   
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE); 
    glutInitContextProfile(GLUT_CORE_PROFILE); 
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(w,h);
    glutInitWindowPosition(50,50);
    glutCreateWindow("Practica 4");
    
 // Inicializamos GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if(GLEW_OK != err) {
        std::cout << "Error: " << glewGetErrorString(err) << std::endl;
        return false;
    }
    std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
    const GLubyte *oglVersion = glGetString(GL_VERSION);
    std::cout <<"This system supports OpenGL Version: " << oglVersion << std::endl;
    
 // Inicializaciones específicas
    funInit();
    
 // Configuración CallBacks
    glutReshapeFunc(funReshape);
    glutDisplayFunc(funDisplay);
    glutTimerFunc(speed,funTimer,0);
    glutMouseFunc (mouseButton);
    glutMotionFunc(mouseMovement);
    glutKeyboardFunc(funKeyboard);
    glutSpecialFunc(keyboardArrows);
          
 // Bucle principal
    glutMainLoop();
    
 // Liberamos memoria
    funDestroy();
    
    return 0;
}

void funInit() {
   
 // Test de profundidad
    glEnable(GL_DEPTH_TEST);
    
 // Transparencias
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    
 // Shaders
    shaders = new Shaders("resources/shaders/vshader.glsl","resources/shaders/fshader.glsl");

 // Modelos
    plane    = new Model("resources/models/plane.obj");
    sphere = new Model("resources/models/sphere.obj");
    cylinder = new Model("resources/models/cylinder.obj");
      
 // Luces direccionales
    lightD[0].direction = glm::vec3(0.0f, -1.0f, 0.0f);
    lightD[0].ambient   = glm::vec3(0.1f, 0.1f, 0.1f);
    lightD[0].diffuse   = glm::vec3(0.7f, 0.7f, 0.7f);
    lightD[0].specular  = glm::vec3(0.7f, 0.7f, 0.7f);
    
 // Luces posicionales
    lightP[0].position  = glm::vec3(std::cos(glm::radians(rotLuz)) * 1.5f,0.2f, std::sin(glm::radians(rotLuz)) * 1.5f);
    lightP[0].ambient   = glm::vec3(0.1f, 0.1f, 0.1f);
    lightP[0].diffuse   = glm::vec3(0.9f, 0.9f, 0.9f);
    lightP[0].specular  = glm::vec3(0.9f, 0.9f, 0.9f);
    lightP[0].c0        = 1.000f;
    lightP[0].c1        = 0.220f;
    lightP[0].c2        = 0.200f;
    
 // Luces focales
    lightF[0].position    = glm::vec3(3.0f, 3.0f,-3.0f);
    //Para mirar el origen, la dirección debe ser el vector posición multiplicado por -1
    lightF[0].direction   = glm::vec3(-3.0f, -3.0f, 3.0f);
    lightF[0].ambient     = glm::vec3(0.2f, 0.2f, 0.2f);
    lightF[0].diffuse     = glm::vec3(0.9f, 0.9f, 0.9f);
    lightF[0].specular    = glm::vec3(0.9f, 0.9f, 0.9f);
    lightF[0].innerCutOff = 10.0f;
    lightF[0].outerCutOff = lightF[1].innerCutOff + 10.0f;
    lightF[0].c0 = 0.700f;
    lightF[0].c1 = 0.090f;
    lightF[0].c2 = 0.032f;
    
 // Materiales
    matRuby.ambient    = glm::vec4(0.174500f, 0.011750f, 0.011750f, 1.0f);
    matRuby.diffuse    = glm::vec4(0.614240f, 0.041360f, 0.041360f, 1.0f);
    matRuby.specular   = glm::vec4(0.727811f, 0.626959f, 0.626959f, 1.0f);
    matRuby.emissive   = glm::vec4(0.0f);
    matRuby.shininess  = 76.8f;
    
    matEmeraldBrazo.ambient =glm::vec4( 0.0215f, 0.1745f, 0.0215f, 0.75f );
    matEmeraldBrazo.diffuse =glm::vec4(0.07568f, 0.61424f, 0.07568f, 0.75f);
    matEmeraldBrazo.specular =glm::vec4(0.633f, 0.727811f, 0.633f, 0.75f);
    matEmeraldBrazo.emissive = glm::vec4(0.0f);
    matEmeraldBrazo.shininess = 76.8f;
    
    matEmeraldEsfera.ambient =glm::vec4( 0.0215f, 0.1745f, 0.0215f, 1.0f );
    matEmeraldEsfera.diffuse =glm::vec4(0.07568f, 0.61424f, 0.07568f, 1.0f);
    matEmeraldEsfera.specular =glm::vec4(0.633f, 0.727811f, 0.633f, 1.0f);
    matEmeraldEsfera.emissive = glm::vec4(0.0f);
    matEmeraldEsfera.shininess = 76.8f;

 // Texturas
    texNoEmissive     = new Texture("resources/textures/imgNoEmissive.png");

    texSuelo.diffuse = new Texture("resources/textures/imgSueloDiffuse.png");
    texSuelo.specular = new Texture("resources/textures/imgSueloSpecular.png");
    texSuelo.emissive = new Texture("resources/textures/imgSueloEmissive.png");
    texSuelo.normal = NULL;
    texSuelo.shininess = 50.0f;
    
    texBrazo.diffuse = new Texture("resources/textures/imgBrazoDiffuse.png");
    texBrazo.specular = texBrazo.diffuse;
    texBrazo.emissive = texNoEmissive;
    texBrazo.normal = new Texture("resources/textures/imgBrazoNormal.png");
    texBrazo.shininess = 50.0f;
    
    texBase.diffuse = new Texture("resources/textures/imgBaseDiffuse.png");
    texBase.specular = texBase.diffuse;
    texBase.emissive = texNoEmissive;
    texBase.normal = NULL;
    texBase.shininess = 50.0f;
    
    texLuz.diffuse = new Texture("resources/textures/imgLuces.png");
    texLuz.specular = texLuz.diffuse;
    texLuz.emissive = texNoEmissive;
    texLuz.normal = NULL;
    texLuz.shininess = 50.0f;
    
}


void funDestroy() {
      
    delete shaders;
    delete plane, sphere, cylinder;
    
    delete texNoEmissive;
    
    delete texSuelo.diffuse, texSuelo.specular, texSuelo.emissive;
    delete texBrazo.diffuse, texBrazo.normal;
    
    delete texBase.diffuse;
    delete texLuz.diffuse;        
}

void funReshape(int wnew, int hnew) {
    
 // Configuración del Viewport
    glViewport(0, 0, wnew, hnew);

 // Captura de w y h
    w = wnew;
    h = hnew;
    
}

void funDisplay() {
    
 // Borramos el buffer de color
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
 // Indicamos los shaders a utilizar
    shaders->use();
    
 // Matriz de Proyección P (Perspectiva)
    float fovy   = cFovy;
    float nplane =  0.1f;
    float fplane = 25.0f;
    float aspectRatio = (float)w/(float)h;
    glm::mat4 P = glm::perspective(glm::radians(fovy), aspectRatio, nplane, fplane); 

 // Matriz de Vista V (Cámara)
    float x = 10.0f*glm::cos(alphaY)*glm::sin(alphaX);
    float y = 10.0f*glm::sin(alphaY);
    float z = 10.0f*glm::cos(alphaY)*glm::cos(alphaX);
    glm::vec3 pos   (   x,    y,    z);
    glm::vec3 lookat(0.0f, 0.0f, 0.0f);
    glm::vec3 up    (0.0f, 1.0f, 0.0f);
    glm::mat4 V = glm::lookAt(pos, lookat, up);   
    shaders->setVec3 ("ucpos",pos);

 // Fijamos las luces
    setLights(P,V);
    
 // Dibujamos la escena
    
    
 // Intercambiamos los buffers
    glutSwapBuffers();
    
}

void drawObjectMat(Model* object, Material material, glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    
    shaders->setMat4("uN"  ,glm::transpose(glm::inverse(M)));
    shaders->setMat4("uM"  ,M);
    shaders->setMat4("uPVM",P*V*M);
    shaders->setBool("umaterials",true);
    shaders->setMaterial("umaterial",material);
    object->render(GL_FILL);

}


void drawObjectTex(Model* object, Textures textures, glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    
    shaders->setMat4("uN"  ,glm::transpose(glm::inverse(M)));
    shaders->setMat4("uM"  ,M);
    shaders->setMat4("uPVM",P*V*M);
    shaders->setBool("umaterials",false);
    shaders->setTextures("utextures",textures);
    object->render(GL_FILL);

}

void setLights(glm::mat4 P, glm::mat4 V) {
    
    shaders->setLight("ulightG",lightG);
    for(int i=0; i<NLD; i++) shaders->setLight("ulightD["+toString(i)+"]",lightD[i]);
    for(int i=0; i<NLP; i++) shaders->setLight("ulightP["+toString(i)+"]",lightP[i]);    
    for(int i=0; i<NLF; i++) shaders->setLight("ulightF["+toString(i)+"]",lightF[i]);
    
    for(int i=0; i<NLP; i++) {
        //Su posición se modifíca con sin y cos multiplicando por el tamaño del radio que es 1.5
        lightP[i].position  = glm::vec3(std::cos(glm::radians(rotLuz)) * 1.5f,0.2f, std::sin(glm::radians(rotLuz)) * 1.5f);
        glm::mat4 M = glm::scale(glm::translate(I,lightP[i].position),glm::vec3(0.025f));
         
        drawObjectTex(sphere,texLuz,P,V,M);
    }

    for(int i=0; i<NLF; i++) {
        glm::mat4 M = glm::scale(glm::translate(I,lightF[i].position),glm::vec3(0.025f));
        drawObjectTex(sphere,texLuz,P,V,M);
    }
    
}

void drawSuelo(glm::mat4 P, glm::mat4 V, glm::mat4 M) { // Podría servir para paredes también

    glm::mat4 S = glm::scale(I, glm::vec3(2.0f,2.0f,2.0f));
    glm::mat4 R = glm::rotate(I, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 T = glm::translate(I, glm::vec3(0.0f,-0.001f,0.0f));
    drawObjectTex(plane,texSuelo,P,V,M*S);
    //Se crea un segundo suelo boca abajo para que no refleje la luz
    drawObjectTex(plane,texSuelo,P,V,M*T*R*S);
}



//-------------- Funciones para animaciones------------//
// Obtenido de la sesión 1
void funTimer(int ignore) {
    
    
    glutPostRedisplay();
    glutTimerFunc(speed,funTimer,0);
}

void mouseButton(int button, int dir, int x, int y){
    if(button == 4 && cFovy <= 60.0f){
        cFovy += 2.0f;
    }
    else if(button == 3 && cFovy >= 10.0f){
        cFovy -= 2.0f;
    }
    glutPostRedisplay();
    
    if(button == GLUT_LEFT_BUTTON){
        btnIzq = true;
    }
    else{
        btnIzq = false;
    }
}


void mouseMovement(int x, int y)
{
  /* Se resta w/2 para "x" y h/2 para "y" porque el tamaño de la pantalla es definido
   * por esas dos variables, por lo que en el centro de la pantalla sera (0,0)*/
  if (btnIzq)
    {
      if((x - w/2)*.3 <= -179.0f){
          alphaX = glm::radians(-179.0f);
      }
      else if((x - w/2)*.3 >= 179.0f){
          alphaX = glm::radians(179.0f);
      }
      else{
          alphaX = glm::radians(float(x - w/2)*.3);
      }
      
      if((y - h/2)*.3  <= -89.0f){
          alphaY = glm::radians(89.0f);
      }
      else if((y - h/2)*.3  >= 89.0f){
          alphaY = glm::radians(-89.0f);
      }
      else{
          alphaY = glm::radians(float(h/2 - y)*.3 );
      }
      glutPostRedisplay();
    }
}

void funKeyboard(unsigned char key, int x, int y) {

    switch(key) {
        case 'A':

            break;
        
          
    }
    glutPostRedisplay();

}

void keyboardArrows (int key, int x, int y) {

    switch(key) {
        case GLUT_KEY_UP:
            
            
            break;
        case GLUT_KEY_DOWN:
            
            
            break;
        case GLUT_KEY_RIGHT:
            
            
            break;
        case GLUT_KEY_LEFT:
            
            
            break;
        
    }
    glutPostRedisplay();

}