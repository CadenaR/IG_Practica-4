#include <iostream>
#include <math.h> 
#include <ogl/glew.h>
#include <ogl/freeglut.h>

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
void funSpecial(int key, int x, int y);
void drawOcean (glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawSubmarineAux(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawSubmarine(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawBody(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawHead(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawPeriscope (glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawFlap(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawPropellers(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawHelices(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawPeriscopeCylinders(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawPeriscopeSphere(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawPeriscopeGlass(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawSphere (glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawSphereMat(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void timer(int angle);
void keyboard(unsigned char key, int x, int y);
void zoom(int button, int state, int x, int y);
void mouse(int x, int y);

void drawObjectMat(Model *object, Material material, glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawObjectTex(Model *object, Textures textures, glm::mat4 P, glm::mat4 V, glm::mat4 M);
void setLights(glm::mat4 P, glm::mat4 V);

float angle = 0.0f;
float desPeris = 0.0f;
float moveX = 0.0f;
float moveY = 0.0f;
float moveZ = 0.0f;
float angleD = 0.0f;
float speed = 0.0f;
float nextSpeed = 0.0f;
float acceleration = 0.001f;
float nextPosition = 0.0f;
float flapAngle = 0.0f;
float hflapAngle = 0.0f;
float angleB = 0.0f;
float angleC = 0.0f;
float angleA = 0.0f;
float lX = 0.78f*cos(-angleA*0.0175f+0.25f)*sin((angleD-90)*0.0175f);
float lY = 0.78f*sin(-angleA*0.0175f+0.25f);
float lZ = 0.78f*cos(-angleA*0.0175f+0.25f)*cos((angleD-90)*0.0175f);
float nextLX = 0.0f;
float nextLY = 0.0f;
float nextLZ = 0.0f;
bool started = false;
float alphaX = 0;
float alphaY = 0;
float newX = 0;
float newY = 0;
float fovy = 30.0f;
float rotp = 90.0f;
float textureFlap = 2;

// Luces
   #define  NLD 1
   #define  NLP 1
   #define  NLF 1
   Light    lightG;
   Light    lightD[NLD];
   Light    lightP[NLP];
   Light    lightF[NLF];
   
// Materiales
   Material matEmeraldTr;
   Material matPerisGlass;
   
// Texturas
   Texture  *texNoEmissive;
   Textures texSuelo;
   Textures texGreenSquares;
   Textures texLuces;
   Textures texBodySub;
   Textures texSphereSub;
   Textures texOcean;
   Textures texFlap;
   Textures texFlap2;
   Textures texPeriscop;
   Textures texTopSphere;

// Shaders
   Shaders* shaders;
   
// Modelos
   Model* plane;
   Model* cylinder;
   Model* sphere;
   Model* cylinderNoTop;

// Viewport
   int w = 600;
   int h = 600;
   
// Animaciones
   float desZ = 0.0f;
   float rotZ = 0.0f;
   
int main(int argc, char** argv) {

 // Inicializamos GLUT
    glutInit(&argc, argv);
    glutInitContextVersion(3,3);   
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE); 
    glutInitContextProfile(GLUT_CORE_PROFILE); 
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600,600);
    glutInitWindowPosition(50,50);
    glutCreateWindow("Pratica 4");
    
 // Inicializamos GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if(GLEW_OK != err) {
        std::cout << "Error: " << glewGetErrorString(err) << std::endl;
        return false;
    }
    std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
    const GLubyte* oglVersion = glGetString(GL_VERSION);
    std::cout <<"This system supports OpenGL Version: " << oglVersion << std::endl;
    
 // Inicializaciones específicas
    funInit();
    
 // Configuración CallBacks
    glutReshapeFunc(funReshape);
    glutDisplayFunc(funDisplay);
    glutTimerFunc(30, timer, 0);
    glutMouseFunc(zoom);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(funSpecial);
    glutMotionFunc(mouse);
         
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
   // glPolygonOffset(1.0f,1.0f);
    
 // Shaders
    shaders = new Shaders("resources/shaders/vshader.glsl","resources/shaders/fshader.glsl");

 // Modelos
    plane    = new Model("resources/models/plane.obj");
    cylinder = new Model("resources/models/cylinder.obj");
    sphere = new Model("resources/models/sphere.obj");  
    cylinderNoTop = new Model("resources/models/cylinderNoTop.obj");

    
    // Luz ambiental global
    lightG.ambient      = glm::vec3(0.001f, 0.001f, 0.001f);
     
 // Luces direccionales
    lightD[0].position  = glm::vec3(0.0f, 4.0f, 0.0f);
    lightD[0].direction = glm::vec3(0.0f, 0.0f, 0.0f);
    lightD[0].ambient   = glm::vec3(0.1f, 0.1f, 0.1f);
    lightD[0].diffuse   = glm::vec3(0.7f, 0.7f, 0.7f);
    lightD[0].specular  = glm::vec3(0.7f, 0.7f, 0.7f);
    

 // Luces posicionales
    lightP[0].position  = glm::vec3(lX,lY, lZ);
    lightP[0].ambient   = glm::vec3 (0.0f, 0.0f, 0.0f);
    lightP[0].diffuse   = glm::vec3 (0.5f, 0.5f, 0.5f);
    lightP[0].specular  = glm::vec3 (0.5, 0.5f, 0.5f);
    lightP[0].c0        = 1.000f;
    lightP[0].c1        = 0.220f;
    lightP[0].c2        = 0.200f;
    
 // Luces focales
    lightF[0].position    = glm::vec3(-0.775f, 0.2f, 0.0f);
    lightF[0].direction   = glm::vec3(-0.775f, 0.0f, 0.0f);
    lightF[0].ambient     = glm::vec3(0.5f, 0.5f, 0.5f);
    lightF[0].diffuse     = glm::vec3(0.9f, 0.9f, 0.9f);
    lightF[0].specular    = glm::vec3(0.9f, 0.9f, 0.9f);
    lightF[0].innerCutOff = 0.0f;
    lightF[0].outerCutOff = 0.0f;
    lightF[0].c0          = 1.000f;
    lightF[0].c1          = 0.090f;
    lightF[0].c2          = 0.032f;
    
    
 // Materiales
    matPerisGlass.ambient = glm::vec4(0.1f,0.1f,0.1f,0.8f);
    matPerisGlass.diffuse = glm::vec4(0.1f,0.1f,0.1f,0.8f);
    matPerisGlass.specular = glm::vec4(0.1f,0.1f,0.1f,0.8f);
    matPerisGlass.emissive = glm::vec4(0.0f);
    matPerisGlass.shininess = 76.8f;
    
    matEmeraldTr.ambient  = glm::vec4(0.0215f, 0.1745f, 0.0215f, 0.75f);
    matEmeraldTr.diffuse  = glm::vec4(0.07568f, 0.61424f, 0.07568f, 0.75f);
    matEmeraldTr.specular = glm::vec4(0.633f, 0.727811f, 0.633f, 0.75f);
    matEmeraldTr.emissive = glm::vec4(0.0f);
    matEmeraldTr.shininess= 76.8f;

 // Texturas
    texNoEmissive      = new Texture("resources/textures/imgNoEmissive.png");
           
    texGreenSquares.diffuse   = new Texture("resources/textures/imgBrazoDiffuse.png");   
    texGreenSquares.specular  = texGreenSquares.diffuse;
    texGreenSquares.emissive  = texNoEmissive;
    texGreenSquares.normal    = new Texture("resources/textures/imgBrazoNormal.png");
    texGreenSquares.shininess = 50.0f;
    
    
    texLuces.diffuse   = new Texture("resources/textures/imgLuces.png");   
    texLuces.specular  = texLuces.diffuse;
    texLuces.emissive  = texNoEmissive;
    texLuces.normal    = NULL;  
    texLuces.shininess = 50.0f;
   
    texBodySub.diffuse   = new Texture("resources/textures/body.png");   
    texBodySub.specular  = texNoEmissive;
    texBodySub.emissive  = texNoEmissive;
    texBodySub.normal    = NULL;  
    texBodySub.shininess = 50.0f;
    
    texFlap.diffuse   = new Texture("resources/textures/flaptexture.jpg");   
    texFlap.specular  = texNoEmissive;
    texFlap.emissive  = texNoEmissive;
    texFlap.normal    = NULL;  
    texFlap.shininess = 50.0f;
    
    texFlap2.diffuse   = new Texture("resources/textures/metaltexture.png");   
    texFlap2.specular  = texNoEmissive;
    texFlap2.emissive  = texNoEmissive;
    texFlap2.normal    = NULL;  
    texFlap2.shininess = 50.0f;
    
    
    texSphereSub.diffuse   = new Texture("resources/textures/metalTexture.png");   
    texSphereSub.specular  = texNoEmissive;
    texSphereSub.emissive  = texNoEmissive;
    texSphereSub.normal    = NULL;  
    texSphereSub.shininess = 50.0f;
    
    texOcean.diffuse   = new Texture("resources/textures/ocean.png");   
    texOcean.specular  = texOcean.diffuse;
    texOcean.emissive  = texNoEmissive;
    texOcean.normal    = NULL;  
    texOcean.shininess = 50.0f;
    
    texPeriscop.diffuse   = new Texture("resources/textures/metalTexture.png");   
    texPeriscop.specular  = texPeriscop.diffuse;
    texPeriscop.emissive  = texNoEmissive;
    texPeriscop.normal    = NULL;  
    texPeriscop.shininess = 50.0f;
    
    texTopSphere.diffuse   = new Texture("resources/textures/topsphere.png");   
    texTopSphere.specular  = texLuces.diffuse;
    texTopSphere.emissive  = texNoEmissive;
    texTopSphere.normal    = NULL;  
    texTopSphere.shininess = 50.0f;
}

void funDestroy() {
      
   delete shaders;
    
    delete sphere, plane, cylinder, cylinderNoTop;  
    
    delete texNoEmissive;
    
    delete texGreenSquares.diffuse, texGreenSquares.specular, texGreenSquares.emissive;
    delete texLuces.diffuse, texLuces.specular, texLuces.emissive;
    
    
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
    //float fovy   = 30.0f;
    float nplane =  0.1f;
    float fplane = 20.0f;
    float aspectRatio = (float)w/(float)h;
    glm::mat4 P = glm::perspective(glm::radians(fovy), aspectRatio, nplane, fplane); 

 // Matriz de Vista V (Cámara)
    //glm::vec3 pos   (-2.3f, 0.2f, 1.2f);
    //glm::vec3 pos   (-2.3f, 0.0f, 0.0f);
    //glm::vec3 lookat(0.0f, 0.0f,  1.2f);
    
    
    float x = 5.0f*glm::cos(alphaY)*glm::sin(alphaX);
    float y = 5.0f*glm::sin(alphaY);
    float z = 5.0f*glm::cos(alphaY)*glm::cos(alphaX);
    glm::vec3 pos(x,y,z);
    //glm::vec3 pos   (-3.0f, 2.0f, 3.0f);
    glm::vec3 lookat(0.0f, 0.0f,  0.0f);
    glm::vec3 up    (0.0f, 1.0f,  0.0f);
    glm::mat4 V = glm::lookAt(pos, lookat, up);   
    
 // Dibujamos la escena
    drawOcean(P,V,I);
    drawSubmarineAux(P,V,I);
    
 // Fijamos las luces
    setLights(P,V);
    
 // Intercambiamos los buffers
    glutSwapBuffers();    
}


void drawOcean(glm::mat4 P, glm::mat4 V, glm::mat4 M) {

    glm::mat4 S = glm::scale(I, glm::vec3(2.5f,2.5f,2.5f));
    drawObjectTex(sphere,texOcean,P,V,M*S);
}

void drawSubmarineAux(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    // Se crean variables de rotación y traslació para todo el submarino
    glm::mat4 T = glm::translate(I, glm::vec3(moveX, moveY, moveZ));
    glm::mat4 RM = glm::rotate(I, glm::radians(angleD), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 RM2 = glm::rotate(I, glm::radians(angleA), glm::vec3(0.0f, 0.0f, 1.0f));

    drawSubmarine(P,V,M*T*RM*RM2);
}

void drawSubmarine(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
   
    glm::mat4 S = glm::scale(I, glm::vec3(1.0f,1.0f,0.8f));
    
    glm::mat4 T = glm::translate(I, glm::vec3(-0.25f, 0.435f, 0.0f));//Posición de periscopio
    

    drawFlap(P,V,M);
    drawBody(P,V,M*S);
    drawHead(P,V,M*S);
    drawPeriscope(P,V,M*T);
    drawPropellers(P,V,M);
}

void drawBody(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    glm::mat4 T = glm::translate(I, glm::vec3(0.0f, 0.2f, 0.0f));
    glm::mat4 T2 = glm::translate(I, glm::vec3((-1.2/2.0), 0.0f, 0.0f)); 
    glm::mat4 T3 = glm::translate(I, glm::vec3((1.2/2.0), 0.0f, 0.0f));
    glm::mat4 R = glm::rotate(I, -1.5707963267f, glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 S = glm::scale(I, glm::vec3(0.2,(1.2/2.0),0.2));

    drawObjectTex(cylinder,texBodySub,P,V,M*T*R*S);
    drawSphere(P,V,M*T2*T);
    drawSphereMat(P,V,M*T3*T);
}

void drawFlap(glm::mat4 P, glm::mat4 V, glm::mat4 M){
    
    glm::mat4 T = glm::translate(I, glm::vec3((1.3/2.0), 0.2f, 0.0f)); 
    glm::mat4 R = glm::rotate(I, -1.5707963267f, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 S = glm::scale(I, glm::vec3(0.05f,0.3f,0.1f));
    glm::mat4 S2 = glm::scale(I, glm::vec3(0.05f,0.3f,0.2f));
    glm::mat4 RM = glm::rotate(I, glm::radians(flapAngle), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 RMH = glm::rotate(I, glm::radians(hflapAngle), glm::vec3(0.0f, 0.0f, 1.0f));
    
    //se cargan las texturas segun el caso
    if(textureFlap == 0){
    drawObjectTex(plane,texFlap,P,V,M*T*S2*RMH);
    drawObjectTex(plane,texFlap,P,V,M*T*S*R*RM);

    }
    if(textureFlap == 1){
    drawObjectTex(plane,texFlap2,P,V,M*T*S2*RMH);
    drawObjectTex(plane,texFlap,P,V,M*T*S*R*RM);

    }
    if(textureFlap == 2){
    drawObjectTex(plane,texFlap,P,V,M*T*S2*RMH);
    drawObjectTex(plane,texGreenSquares,P,V,M*T*S*R*RM);
    }
}

void drawHead(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    
    glm::mat4 T = glm::translate(I, glm::vec3(-0.2f, 0.5f, 0.0f)); 
    glm::mat4 S2 = glm::scale(I, glm::vec3(0.1f,(0.2/2.0),0.1f));
    drawObjectTex(cylinder,texTopSphere,P,V,M*T*S2); 
}

void drawPeriscope(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    
    glm::mat4 T = glm::translate(I, glm::vec3(0.0f, desPeris, 0.0f));
    
    drawPeriscopeCylinders(P,V,M*T);
    
}

void drawPeriscopeCylinders(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    
    glm::mat4 S = glm::scale(I, glm::vec3(0.01f,0.1f,0.01f));
    glm::mat4 T = glm::translate(I, glm::vec3(0.0f, 0.1f, 0.0f));
    glm::mat4 RM = glm::rotate(I, glm::radians(angleB), glm::vec3(0.0f, 1.0f, 0.0f));
    drawObjectTex(cylinder,texPeriscop,P,V,M*RM*T*S);//cilinfro base
    glm::mat4 S2 = glm::scale(I, glm::vec3(0.009f,0.025f,0.009f));
    glm::mat4 T2 = glm::translate(I, glm::vec3(0.0f, 0.1f, 0.0f));
    glm::mat4 T3 = glm::translate(I, glm::vec3(0.0f, 0.025f, 0.0f));
    glm::mat4 R = glm::rotate(I, glm::radians(angleC), glm::vec3(0.0f, 0.0f, 1.0f));
    drawObjectTex(cylinder,texPeriscop,P,V,M*T2*T*RM*R*T3*S2); //cilindro articulado
    drawPeriscopeSphere(P,V,M*T2*T*RM*R);
    drawPeriscopeGlass(P,V,M*T2*T*RM*R);
}

void drawPeriscopeSphere(glm::mat4 P, glm::mat4 V, glm::mat4 M){
    glm::mat4 S = glm::scale(I, glm::vec3(0.006f,0.006f,0.006f));
    drawObjectTex(sphere,texPeriscop,P,V,M*S);
}

void drawPeriscopeGlass(glm::mat4 P, glm::mat4 V, glm::mat4 M){
    glm::mat4 S = glm::scale(I, glm::vec3(0.004f,0.004f,0.004f));
    glm::mat4 T = glm::translate(I, glm::vec3(0.0f, 0.05f, 0.0f));
    drawObjectMat(sphere,matPerisGlass,P,V,M*T*S);
}

void drawPropellers(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    
    glm::mat4 T = glm::translate(I, glm::vec3(0.3f, 0.1f, 0.2f)); 
    glm::mat4 T2 = glm::translate(I, glm::vec3(0.3f, 0.1f, -0.2f)); 
    glm::mat4 S = glm::scale(I, glm::vec3(0.1f,0.1f,0.1f));
    glm::mat4 R = glm::rotate(I, 1.5707963267f*0.2f, glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 R2 = glm::rotate(I, -1.5707963267f*0.05f, glm::vec3(0.0f, 1.0f, 0.0f));

    drawObjectTex(cylinderNoTop,texPeriscop,P,V,M*T*S*R2*R);
    drawObjectTex(cylinderNoTop,texPeriscop,P,V,M*T2*S*R2*R);
    drawHelices(P,V,M*T*S*R);
    drawHelices(P,V,M*T2*S*R);
}



void drawHelices(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    
    glm::mat4 S = glm::scale(I, glm::vec3(0.1f,0.5f,0.5f));
    glm::mat4 T = glm::translate(I, glm::vec3(0.0f, -0.1f, -0.05f)); 
    glm::mat4 S2 = glm::scale(I, glm::vec3(0.1f,0.1f,0.1f));
    glm::mat4 RM = glm::rotate(I, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));

    drawObjectTex(plane,texPeriscop,P,V,M*T*S*RM);
    drawObjectTex(sphere,texPeriscop,P,V,M*T*S2);

}

void drawSphere(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    
    glm::mat4 S2 = glm::scale(I, glm::vec3((0.2/2.0),(0.2/2.0),(0.2/2.0)));
    drawObjectTex(sphere,texSphereSub,P,V,M*S2); 
}

void drawSphereMat(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    
    glm::mat4 S2 = glm::scale(I, glm::vec3((0.2/2.0),(0.2/2.0),(0.2/2.0)));
    drawObjectMat(sphere,matEmeraldTr,P,V,M*S2); 
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
    
    //Se calcula la posición con respecto a la cola del submarino
    lX = -0.775f*cos(-angleA*0.0175f-0.25f)*sin((angleD-90)*0.0175f);
    lY = -0.775f*sin(-angleA*0.0175f-0.25f);
    lZ = -0.775f*cos(-angleA*0.0175f-0.25f)*cos((angleD-90)*0.0175f);
    
    shaders->setLight("ulightG",lightG);
    for(int i=0; i<NLD; i++) shaders->setLight("ulightD["+toString(i)+"]",lightD[i]);
    for(int i=0; i<NLP; i++) shaders->setLight("ulightP["+toString(i)+"]",lightP[i]);    
    for(int i=0; i<NLF; i++) shaders->setLight("ulightF["+toString(i)+"]",lightF[i]);
    
    glm::mat4 M = glm::scale(glm::translate(I,lightP[0].position),glm::vec3(0.025f));
    lightP[0].position  = glm::vec3(lX+moveX, lY+moveY, lZ+moveZ);
    drawObjectTex(sphere,texLuces,P,V,M);
    
    
}

void zoom(int button, int state, int x, int y){
    if (button == 3)//
  { 
        if(fovy>10){
           fovy -= 1;
        }
    }
    else if (button ==4)
    {
        if(fovy<60){
           fovy += 1;
        }
    }
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y){
    switch(key){
        //Rotación de todo el periscopio
        case 'B':
            angleB -= 5.0f;
            break;
        case 'b':
            angleB += 5.0f;
            break;
        // rotación articulada de segundo cilindro
        case 'o':
            if(angleC < 120)
                angleC += 5.0f;
            break;
        case 'O':
            if(angleC > -120)
                angleC -= 5.0f;
            break;
        // subir submarino
        case 'a':
            hflapAngle = 12;
            if(angleA>-30.0){
                angleA -= 0.8f;
            }
            break;
        // bajar submarino
        case 'z':
            hflapAngle = -12;
            if(angleA<30.0){
                angleA += 0.8f;
            }
            break;
        // cambiar textura
        case 'T':
            if(textureFlap<2)
            textureFlap +=1;
            else textureFlap = 0;
            break;
        // Sacar periscopio
        case 'P':
            if(desPeris > 0.0){
                desPeris -= 0.005;
            }
            break;
        //meter periscopio
        case 'p':
            if(desPeris<0.165){
                desPeris += 0.005;
            }
            break;
        //encender submarino    
        case 's':
            started = true;
            break;
        //apagar submarino
        case 'S':
            started = false;
            break;
        //aumentar luz
        case 'l':
            if(lightP[0].ambient.x < 1.0f){
                lightP[0].ambient.x += 0.1f;
                lightP[0].ambient.y += 0.1f;
                lightP[0].ambient.z += 0.1f;
            }
            break;
        //disminuir luz
        case 'L':
            
            if(lightP[0].ambient.x > 0.0f){
                lightP[0].ambient.x -= 0.1f;
                lightP[0].ambient.y -= 0.1f;
                lightP[0].ambient.z -= 0.1f;
            }
            break;
    }
        glutPostRedisplay();
        //glutKeyboardFunc(keyboard);
}

void mouse(int x, int y){

    newX = -(w/2-x);
    newY = h/2-y;
    
    alphaX = newX*90/(w/2);
    alphaY = newY*90/(h/2);
    
    if(alphaX < -179){
        alphaX = -179;
    }
    else if(alphaX > 179){
        alphaX = 179;
    }
    
    
    if(alphaY < -89){
        alphaY = -89;
    }
    else if(alphaY > 89){
        alphaY = 89;
    }

    
    alphaX = glm::radians(alphaX);
    alphaY = glm::radians(alphaY);
    
}


void timer(int ignore)
{
    angle += 7.0f;
    
    if(flapAngle>0){
    flapAngle -= 1.0f;
    }
    if(flapAngle<0){
    flapAngle += 1.0f;
    }
    
     if(hflapAngle>0){
    hflapAngle -= 1.0f;
    }
    if(hflapAngle<0){
    hflapAngle += 1.0f;
    }
    
    if(started){
        nextSpeed = speed + acceleration;
        if(abs(nextSpeed) < 0.015){
            speed += acceleration;
        }
        
    } else {
        if(speed > 0.0f){
            speed -= 0.0001;
        } else if (speed < 0.0f) {
            speed += 0.0001;
        }
    }
    
    //se revisa que la próxima posición no avance mas del límite
    nextPosition = sqrt(
                pow(moveX + speed*sin(glm::radians(angleD-90)),2.0f) 
              + pow(moveY + speed*sin(glm::radians(-angleA)),2.0f) 
              + pow(moveZ + speed*cos(glm::radians(angleD-90)) ,2.0f));
    
    if(nextPosition < 4){
        moveX += speed*sin(glm::radians(angleD-90));
        moveY += speed*sin(glm::radians(-angleA));
        moveZ += speed*cos(glm::radians(angleD-90));

    }
    
    
    glutPostRedisplay();
    glutTimerFunc(30, timer, 0);
}


void funSpecial(int key, int x, int y) {
    
    switch(key) {
        // movimiento del submarino en la dirección que mira
        case GLUT_KEY_UP: 
            if(acceleration < 0){
                acceleration *= -1.0f;
            }
            break;
        //movimiento del submarino en la dirección contraria a la que mira
        case GLUT_KEY_DOWN: 
            if(acceleration > 0){
                acceleration *= -1.0f;
            }
            break;
        //girar izquierda
        case GLUT_KEY_LEFT: 
            flapAngle = -12.0f;
            angleD += 0.5f;   
            break;
        //girar derecha
        case GLUT_KEY_RIGHT: 
            flapAngle = 12.0f;
            angleD -= 0.5f;   
            break;
    }
           
    glutPostRedisplay();
}