#include <iostream>
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

void drawObject(Model* object, glm::vec3 color, glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawSuelo (glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawSubmarine(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawBody(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawHead(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawPeriscope (glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawFlap(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawCylinder(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawSphere (glm::mat4 P, glm::mat4 V, glm::mat4 M);
void timer(int angle);
void keyboard(unsigned char key, int x, int y);
void zoom(int button, int state, int x, int y);
void mouse(int x, int y);
void move(int key, int x, int y);

void drawObjectMat(Model *object, Material material, glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawObjectTex(Model *object, Textures textures, glm::mat4 P, glm::mat4 V, glm::mat4 M);
void setLights(glm::mat4 P, glm::mat4 V);

int angle = 0;
int angleB = 0;
int angleA = 0;
int angleV = 0;
float desR = 0;
float moveX = 0;
float moveZ = 0;
float alphaX = 0;
float alphaY = 0;
float newX = 0;
float newY = 0;
float newAngleX = 0;
float newAngleY = 0;
float fovy = 30.0f;
float rotp = 90.0f;
float focal = 1.0f;

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
   Material matLuces;
   Material matEmerald;
   Material matEmeraldTr;
   
// Texturas
   Texture  *texNoEmissive;
   Textures texBase;
   Textures texSuelo;
   Textures texBrazoAzul;
   Textures texLuces;
   Textures texCube;
   Textures texWindow;


  
// Shaders
   Shaders* shaders;
   
// Modelos
   Model* plane;
   Model* cylinder;
   Model* sphere;
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
    glutCreateWindow("Pratica 1");
    
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
    glutSpecialFunc(funSpecial);
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
    
    // Luz ambiental global
    lightG.ambient      = glm::vec3(0.5f, 0.5f, 0.5f);
     
 // Luces direccionales
    lightD[0].position  = glm::vec3(0.0f, 4.0f, 0.0f);
    lightD[0].direction = glm::vec3(0.0f, 0.0f, 0.0f);
    lightD[0].ambient   = glm::vec3(0.1f, 0.1f, 0.1f);
    lightD[0].diffuse   = glm::vec3(0.7f, 0.7f, 0.7f);
    lightD[0].specular  = glm::vec3(0.7f, 0.7f, 0.7f);
    

 // Luces posicionales
    lightP[0].position  = glm::vec3(0.0f, 0.2f, -1.5f);
    lightP[0].ambient   = glm::vec3 (0.1f, 0.1f, 0.1f);
    lightP[0].diffuse   = glm::vec3 (0.9f, 0.9f, 0.9f);
    lightP[0].specular  = glm::vec3 (0.9f, 0.9f, 0.9f);
    lightP[0].c0        = 1.000f;
    lightP[0].c1        = 0.220f;
    lightP[0].c2        = 0.200f;
    
 // Luces focales
    lightF[0].position    = glm::vec3(3.0f, 3.0f, -3.0f);
    lightF[0].direction   = glm::vec3(0.0f, 0.0f, 0.0f);
    lightF[0].ambient     = focal * glm::vec3(0.5f, 0.5f, 0.5f);
    lightF[0].diffuse     = focal * glm::vec3(0.9f, 0.9f, 0.9f);
    lightF[0].specular    = focal * glm::vec3(0.9f, 0.9f, 0.9f);
    lightF[0].innerCutOff = 10.0f;
    lightF[0].outerCutOff = lightF[0].innerCutOff + 5.0f;
    lightF[0].c0          = 1.000f;
    lightF[0].c1          = 0.090f;
    lightF[0].c2          = 0.032f;
    
    
 // Materiales
    matLuces.ambient   = glm::vec4(0.0f,0.0f,0.0f,1.0f);
    matLuces.diffuse   = glm::vec4(0.0f,0.0f,0.0f,1.0f);
    matLuces.specular  = glm::vec4(0.0f,0.0f,0.0f,1.0f);
    matLuces.emissive  = glm::vec4(lightF[0].diffuse,1.0f);
    matLuces.shininess = 10.0f;
    
    matRuby.ambient    = glm::vec4(0.174500f, 0.011750f, 0.011750f, 1.0f);
    matRuby.diffuse    = glm::vec4(0.614240f, 0.041360f, 0.041360f, 1.0f);
    matRuby.specular   = glm::vec4(0.727811f, 0.626959f, 0.626959f, 1.0f);
    matRuby.emissive   = glm::vec4(0.0f);
    matRuby.shininess  = 76.8f;
    
    matEmerald.ambient  = glm::vec4(0.0215f, 0.1745f, 0.0215f, 1.0f);
    matEmerald.diffuse  = glm::vec4(0.07568f, 0.61424f, 0.07568f, 1.0f);
    matEmerald.specular = glm::vec4(0.633f, 0.727811f, 0.633f, 1.0f);
    matEmerald.emissive = glm::vec4(0.0f);
    matEmerald.shininess= 76.8f;
    
    matEmeraldTr.ambient  = glm::vec4(0.0215f, 0.1745f, 0.0215f, 0.75f);
    matEmeraldTr.diffuse  = glm::vec4(0.07568f, 0.61424f, 0.07568f, 0.75f);
    matEmeraldTr.specular = glm::vec4(0.633f, 0.727811f, 0.633f, 0.75f);
    matEmeraldTr.emissive = glm::vec4(0.0f);
    matEmeraldTr.shininess= 76.8f;

 // Texturas
    texNoEmissive      = new Texture("resources/textures/imgNoEmissive.png");
    
    texSuelo.diffuse   = new Texture("resources/textures/imgSueloDiffuse.png");    
    texSuelo.specular  = new Texture("resources/textures/imgSueloSpecular.png");    
    texSuelo.emissive  = new Texture("resources/textures/imgSueloEmissive.png");
    texSuelo.normal    = NULL;  
    texSuelo.shininess = 50.0f;
    

    texBase.diffuse   = new Texture("resources/textures/imgBaseDiffuse.png");    
    texBase.specular  = texBase.diffuse; 
    texBase.emissive  = texNoEmissive;
    texBase.normal    = NULL;
    texBase.shininess = 50.0f;
   
    
    texBrazoAzul.diffuse   = new Texture("resources/textures/imgBrazoDiffuse.png");   
    texBrazoAzul.specular  = texBrazoAzul.diffuse;
    texBrazoAzul.emissive  = texNoEmissive;
    texBrazoAzul.normal    = new Texture("resources/textures/imgBrazoNormal.png");
    texBrazoAzul.shininess = 50.0f;
    
    
    texLuces.diffuse   = new Texture("resources/textures/imgLuces.png");   
    texLuces.specular  = texLuces.diffuse;
    texLuces.emissive  = texNoEmissive;
    texLuces.normal    = NULL;  
    texLuces.shininess = 50.0f;
   
}

void funDestroy() {
      
   delete shaders;
    
    delete sphere, plane, cylinder;  
    
    delete texNoEmissive;
    delete texBase.diffuse, texBase.specular, texBase.emissive;
    delete texSuelo.diffuse, texSuelo.specular, texSuelo.emissive;
    
    delete texBrazoAzul.diffuse, texBrazoAzul.specular, texBrazoAzul.emissive;
    delete texLuces.diffuse, texLuces.specular, texLuces.emissive;
    
    delete texCube.diffuse, texCube.specular, texCube.emissive;
    delete texWindow.diffuse, texWindow.specular, texWindow.emissive;
    
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
    drawSuelo(P,V,I);
    drawSubmarine(P,V,I);
    
 // Fijamos las luces
    setLights(P,V);
    
 // Intercambiamos los buffers
    glutSwapBuffers();    
}



void drawObject(Model* object, glm::vec3 color, glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    
    shaders->setMat4("uPVM",P*V*M);
    glEnable(GL_POLYGON_OFFSET_FILL);
        shaders->setVec3("uColor",color);
        object->render(GL_FILL);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glColor3ub(1.0f,1.0f,1.0f);
}

void drawSuelo(glm::mat4 P, glm::mat4 V, glm::mat4 M) {

    glm::mat4 S = glm::scale(I, glm::vec3(2.0f,2.0f,2.0f));
    drawObjectTex(plane,texSuelo,P,V,M*S);
}

void drawSubmarine(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
   
    glm::mat4 S = glm::scale(I, glm::vec3(1.0f,1.0f,0.8f));
    glm::mat4 S2 = glm::scale(I, glm::vec3(0.01f,0.1f,0.01f));
    glm::mat4 T = glm::translate(I, glm::vec3(-0.25f, 0.6f, 0.0f));
    drawBody(P,V,M*S);
    drawHead(P,V,M*S);
    drawFlap(P,V,M);
    drawPeriscope(P,V,M*T*S2);
  
}

void drawBody(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    glm::mat4 T = glm::translate(I, glm::vec3(0.0f, 0.2f, 0.0f));
    glm::mat4 T2 = glm::translate(I, glm::vec3((-1.2/2.0), 0.0f, 0.0f)); 
    glm::mat4 T3 = glm::translate(I, glm::vec3((1.2/2.0), 0.0f, 0.0f));
    glm::mat4 R = glm::rotate(I, -1.5707963267f, glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 S = glm::scale(I, glm::vec3(0.2,(1.2/2.0),0.2));

    drawObjectTex(cylinder,texBase,P,V,M*T*R*S);
    drawSphere(P,V,M*T2*T);
    drawSphere(P,V,M*T3*T);
}

void drawFlap(glm::mat4 P, glm::mat4 V, glm::mat4 M){
    
    glm::mat4 T = glm::translate(I, glm::vec3((1.3/2.0), 0.2f, 0.0f)); 
    glm::mat4 R = glm::rotate(I, -1.5707963267f, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 S = glm::scale(I, glm::vec3(0.05f,0.3f,0.1f));
    drawObjectTex(plane,texSuelo,P,V,M*T*S*R);
}

void drawHead(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    
    glm::mat4 T = glm::translate(I, glm::vec3(-0.2f, 0.5f, 0.0f)); 
    glm::mat4 S2 = glm::scale(I, glm::vec3(0.1f,(0.2/2.0),0.1f));
    drawObjectTex(cylinder,texBrazoAzul,P,V,M*T*S2); 
}

void drawPeriscope(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    
    drawObjectTex(cylinder,texBrazoAzul,P,V,M);
    drawCylinder(P,V,M);
}

void drawCylinder(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    
    glm::mat4 T = glm::translate(I, glm::vec3(0.0f, 3.0f, 0.0f)); 
    glm::mat4 R = glm::rotate(I, -1.5707963267f, glm::vec3(0.0f, 0.0f, 1.0f));
    drawObjectTex(cylinder,texBrazoAzul,P,V,M*T); 
}

void drawSphere(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    
    glm::mat4 S2 = glm::scale(I, glm::vec3((0.2/2.0),(0.2/2.0),(0.2/2.0)));
    drawObjectTex(sphere,texBrazoAzul,P,V,M*S2); 
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
    
    float lx = 1.5 * cos(glm::radians(rotp));
    float lz = -1.5 * sin(glm::radians(rotp));
    
    shaders->setLight("ulightG",lightG);
    for(int i=0; i<NLD; i++) shaders->setLight("ulightD["+toString(i)+"]",lightD[i]);
    for(int i=0; i<NLP; i++) shaders->setLight("ulightP["+toString(i)+"]",lightP[i]);    
    for(int i=0; i<NLF; i++) shaders->setLight("ulightF["+toString(i)+"]",lightF[i]);
    
    for(int i=0; i<NLP; i++) {
        glm::mat4 M = glm::scale(glm::translate(I,lightP[i].position),glm::vec3(0.025f));
        lightP[0].position  = glm::vec3(lx, 0.2f, lz);
        drawObjectTex(sphere,texLuces,P,V,M);
    }

    for(int i=0; i<NLF; i++) {
        glm::mat4 M = glm::scale(glm::translate(I,lightF[i].position),glm::vec3(0.025f));
        drawObjectTex(sphere,texLuces,P,V,M);
    }
    
}


void move(int key, int x, int y){
    if(key==GLUT_KEY_UP)     
        moveX += 0.05;
    else if(key == GLUT_KEY_DOWN)
        moveX -= 0.05;
    else if(key == GLUT_KEY_LEFT)
        moveZ -= 0.05;
    else if(key == GLUT_KEY_RIGHT)
        moveZ += 0.05;
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
        case 'B':
            angleB -= 5;
            break;
        case 'b':
            angleB += 5;
            break;
        case 'A':
            if(angleA>0){
            angleA -= 5;
            }
            break;
        case 'a':
            if(angleA<180){
            angleA += 5;
            }
            break;
        case 'V':
            if(angleV>-90){
            angleV -= 5;
            }
            break;
        case 'v':
            if(angleV<90){
            angleV += 5;
            }
            break;
        case 'R':
            if(desR>-0.25){
            desR -= 0.01;
            }
            break;
        case 'r':
            if(desR<0.25){
            desR += 0.01;
            }
            break;
        case 's':
            if(lightP[0].ambient.x < 1.0f){
                lightP[0].ambient.x += 0.1f;
                lightP[0].ambient.y += 0.1f;
                lightP[0].ambient.z += 0.1f;
            }
            break;
        case 'S':
            if(lightP[0].ambient.x > 0.0f){
                lightP[0].ambient.x -= 0.1f;
                lightP[0].ambient.y -= 0.1f;
                lightP[0].ambient.z -= 0.1f;
            }
            break;
        case 'p': 
            rotp += 5;
            break;
        case 'f':
            if(focal > 0.9f)
                focal = 0.0f;
            else focal = 1.0f;
        default: break;
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
    angle += 5;
    glutPostRedisplay();
    glutTimerFunc(30, timer, 0);
}


void funSpecial(int key, int x, int y) {
       
    switch(key) {
        case GLUT_KEY_UP:    moveZ += 0.05f;   break;
        case GLUT_KEY_DOWN:  moveZ -= 0.05f;   break;
        case GLUT_KEY_LEFT:  moveX += 0.05f;   break;
        case GLUT_KEY_RIGHT: moveX -= 0.05f;   break;
        default:
           moveX  = 0.0f;   
           moveZ  = 0.0f;
           break;
    }
    if(moveX>1.75)
               moveX=1.75f;
           else if(moveX<-1.75f)
               moveX=-1.75f;
           if(moveZ>1.75f)
               moveZ=1.75f;
           else if(moveZ<-1.75f)
               moveZ=-1.75f;
           
    glutPostRedisplay();
}