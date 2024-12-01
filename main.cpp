// Autor: Nenad Dubovac
// Opis: Animacija piramida u pustinji

#define _CRT_SECURE_NO_WARNINGS

#define CRES 30 

#define MRES 100 // Rezolucija meseca

#define NUM_RAYS 30
#define NUM_CIRCLE_VERTICES 30
#define RADIUS 0.02f
#define RAY_LENGTH 0.01f
 //Biblioteke za stvari iz C++-a (unos, ispis, fajlovi itd) 
#include <iostream>
#include <fstream>
#include <sstream>


#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <map>
#define M_PI 3.14159265358979323846


#include <string>
#include <glm/vec3.hpp> 

//Biblioteke OpenGL-a


#include <glad/glad.h>
#include <GLFW/glfw3.h>//Olaksava pravljenje i otvaranje prozora (konteksta) sa OpenGL sadrzajem


#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp> 

#include "shader.h"

#include <ft2build.h>
#include <vector>
#include FT_FREETYPE_H 

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned int compileShader(GLenum type, const char* source); //Uzima kod u fajlu na putanji "source", kompajlira ga i vraca sejder tipa "type"
unsigned int createShader(const char* vsSource, const char* fsSource); //Pravi objedinjeni sejder program koji se sastoji od Vertex sejdera ciji je kod na putanji vsSource i Fragment sejdera na putanji fsSource

static unsigned loadImageToTexture(const char* filePath); //Ucitavanje teksture, izdvojeno u funkciju
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;
//unsigned int VAOtext, VBOtext;
unsigned int VBO[13];
unsigned int VAO[13];

std::vector<float> letterStartTimes;
bool startFading = false;
int messageSize;
bool showText = false;
bool shutdownStarted = false;
float shutdownStartTime;

int main(void)
{


    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------  
    GLFWwindow* window;
    unsigned int wWidth = 1920; //Podesiti u zavisnosti od rezolucije monitora
    unsigned int wHeight = 1080;
    const char wTitle[] = "[Generic title]";
    window = glfwCreateWindow(wWidth, wHeight, wTitle, glfwGetPrimaryMonitor(), NULL); //fullscreen

    //GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    

    // OpenGL state
    // ------------
    /*
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    */
    // compile and setup the shader
    // ----------------------------
    Shader shader("text.vert", "text.frag");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
    shader.use();
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // FreeType
    // --------
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

    // find path to font
    /*
    std::string font_name = FileSystem::getPath("./arial.ttf");
    if (font_name.empty())
    {
        std::cout << "ERROR::FREETYPE: Failed to load font_name" << std::endl;
        return -1;
    }
    */

    // load font as face
    FT_Face face;
    if (FT_New_Face(ft, "arial.ttf", 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    }
    else {
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, 48);

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load first 128 characters of ASCII set
        for (unsigned char c = 0; c < 128; c++)
        {
            // Load character glyph 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            Characters.insert(std::pair<char, Character>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // Shaders

    unsigned int sunShader = createShader("basic.vert", "basic.frag");
    unsigned int pyramidShader = createShader("pyramid.vert", "pyramid.frag");
    unsigned int lakeShader = createShader("lake.vert", "lake.frag");
    unsigned int midPyramidShader = createShader("mid-pyramid.vert", "mid-pyramid.frag");
    unsigned int skyShader = createShader("sky.vert", "sky.frag");
    unsigned int grassShader = createShader("grass.vert", "grass.frag");
    unsigned int fishShader = createShader("fish.vert", "fish.frag");
    unsigned int starShader = createShader("star.vert", "star.frag");
    unsigned int doorShader = createShader("door.vert", "door.frag");
    unsigned int authorShader = createShader("auth.vert", "auth.frag");
    unsigned int sandShader = createShader("sand.vert", "sand.frag");
    unsigned int doorCoverShader = createShader("doorCover.vert", "doorCover.frag");

    //ODAVDE POCINJE IZMENA ZA VAO I VBO
    glGenVertexArrays(13, VAO);
    glGenBuffers(13, VBO);

    unsigned int VAO_inner, VBO_inner;
    unsigned int VAO_outer, VBO_outer;
    glGenVertexArrays(1, &VAO_inner);
    glGenBuffers(1, &VBO_inner);
    glGenVertexArrays(1, &VAO_outer);
    glGenBuffers(1, &VBO_outer);


    // configure VAO/VBO for texture quads
    // -----------------------------------
    //glGenVertexArrays(1, &VAOtext);
    //glGenBuffers(1, &VBOtext);
    glBindVertexArray(VAO[12]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[12]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //***************** PYRAMIDS *************************
    float vertices[] = {

        0.25, -0.1,    0.0, 1.0, 1.0, 0.0, //prvo tjeme
        0.4, 0.3,   0.0, 1.0, 1.0, 0.0, //drugo tjeme
        0.55, -0.1,     0.0, 1.0, 1.0, 0.0,

        -0.25, -0.1,    0.0, 1.0, 1.0, 0.0, //prvo tjeme
        -0.4, 0.3,   0.0, 1.0, 1.0, 0.0, //drugo tjeme
        -0.55, -0.1,     0.0, 1.0, 1.0, 0.0,

        -0.2, 0.0,       1.0, 0.0, 1.0, 0.0, //prvo tjeme
        0.0, 0.5,        1.0, 0.0, 1.0, 0.0, //drugo tjeme
        0.2, 0.0,        1.0, 0.0, 1.0, 0.0 //trece tjeme
    };

    unsigned int stride = (2 + 4) * sizeof(float);

    glBindVertexArray(VAO[0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);  //koordinate temena
    glEnableVertexAttribArray(0); // Omogu?ite koordinate temena
    //glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float))); //boja
    //glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // ***************** MID PYRAMID *************************
    float midPyramid[] = {
        -0.2, 0.0,       1.0, 0.0, 1.0, 0.0, //prvo tjeme
        0.0, 0.5,        1.0, 0.0, 1.0, 0.0, //drugo tjeme
        0.2, 0.0,        1.0, 0.0, 1.0, 0.0 //trece tjeme
    };

    glBindVertexArray(VAO[4]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(midPyramid), midPyramid, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);  //koordinate temena
    glEnableVertexAttribArray(0); // Omogu?ite koordinate temena
    //glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float))); //boja
    //glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // ***************** DOORS *************************
    float doorVertices[] = {
        // Pozicije       // Boje (žuta)
        -0.025f, 0.0f,    0.6f, 0.3f, 0.1f,  // Donji levi
        0.025f, 0.0f,     0.6f, 0.3f, 0.1f,// Donji desni
        -0.025f,  0.1f,    0.6f, 0.3f, 0.1f, // Gornji desni
        0.025f,  0.1f,   0.6f, 0.3f, 0.1f,// Gornji levi
    };

    glBindVertexArray(VAO[9]); // VAO za jezero
    glBindBuffer(GL_ARRAY_BUFFER, VBO[9]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(doorVertices), doorVertices, GL_STATIC_DRAW);

    // Povezivanje atributa verteksa
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (float*)0);
    glEnableVertexAttribArray(0);

    // Postavi atribute za boje (lokacija 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // ***************** COVERS *************************
    float doorCoversVertices[] = {
        // Pozicije       // Boje (žuta)
        -0.025f, 0.0f,    0.6f, 0.3f, 0.1f,  // Donji levi
        0.025f, 0.0f,     0.6f, 0.3f, 0.1f,// Donji desni
        -0.025f,  0.1f,    0.6f, 0.3f, 0.1f, // Gornji desni
        0.025f,  0.1f,   0.6f, 0.3f, 0.1f,// Gornji levi
    };

    glBindVertexArray(VAO[11]); // VAO za jezero
    glBindBuffer(GL_ARRAY_BUFFER, VBO[11]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(doorCoversVertices), doorCoversVertices, GL_STATIC_DRAW);

    // Povezivanje atributa verteksa
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (float*)0);
    glEnableVertexAttribArray(0);

    // Postavi atribute za boje (lokacija 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // ***************** LAKE *************************
    float lake[(CRES + 2) * 2]; // Verteksi elipse
    float radiusX = 0.5; // Polupre?nik elipse po x-osi
    float radiusY = 0.3; // Polupre?nik elipse po y-osi
    float lakeOffsetX = 0.0; // Pomak na x-osi
    float lakeOffsetY = -0.4; // Pomak na y-osi (donji deo ekrana)

    // Centar elipse
    lake[0] = lakeOffsetX; // Centar X0
    lake[1] = lakeOffsetY; // Centar Y0

    // Generisanje ta?aka na obodu elipse
    for (int i = 0; i <= CRES; i++) {
        float angle = (M_PI * 2.0f) * (float)i / (float)CRES; // Ugao u radijanima
        lake[2 + 2 * i] = lakeOffsetX + radiusX * cos(angle);    // Xi (pomaknut za offsetX)
        lake[2 + 2 * i + 1] = lakeOffsetY + radiusY * sin(angle); // Yi (pomaknut za offsetY)
    }

    // Kreiranje VAO i VBO za jezero
    glBindVertexArray(VAO[2]); // VAO za jezero
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lake), lake, GL_STATIC_DRAW);

    // Povezivanje atributa verteksa
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0); // Koordinate verteksa (x, y)
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //***************** STARS *************************  

    // Broj krakova zvezde (za petokraku zvezdu)
    const int numPoints = 5;
    float radiusOuter = 0.03f; // Radijus spoljnjih ta?aka
    float radiusInner = 0.01f; // Radijus unutrašnjih ta?aka
    float centerXstar = -0.5f, centerYstar = 0.5f; // Pozicija centra

    //float starVertices[numPoints * 4 + 2]; // 5 spoljnjih + 5 unutrašnjih + povratak na po?etnu ta?ku
    float starVertices[numPoints * 5 * 2 + 5 * 5 + 2 * 5]; // Dodaj RGB za svako teme
    int index = 0;
    float angleStep = M_PI / numPoints; // Korak ugla izme?u ta?aka

    // Dodaj centar zvezde (za GL_TRIANGLE_FAN)
    starVertices[index++] = centerXstar;
    starVertices[index++] = centerYstar;

    // Postavi boju centra zvezde (opciono: može biti druga?ija)
    starVertices[index++] = 1.0f; // R
    starVertices[index++] = 0.0f; // G
    starVertices[index++] = 0.0f; // B

    // Generiši temena spoljnih i unutrašnjih ta?aka
    for (int i = 0; i <= numPoints * 2; i++) {
        float angle = i * angleStep;
        float radius = (i % 2 == 0) ? radiusOuter : radiusInner; // Naizmeni?no spolja/unutra
        starVertices[index++] = centerXstar + radius * cos(angle);
        starVertices[index++] = centerYstar + radius * sin(angle);

        // Boja za svako teme (opciono)
        starVertices[index++] = 1.0f; // R
        starVertices[index++] = 0.0f; // G
        starVertices[index++] = 0.0f; // B
    }
    glBindVertexArray(VAO[3]);
    //unsigned int stride = 6 * sizeof(float); // 2 float-a za koordinate + 4 float-a za boju

    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(starVertices), starVertices, GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, starVertices.size() * sizeof(float), starVertices.data(), GL_STATIC_DRAW);


    //glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);  //koordinate temena
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // Omogu?ite koordinate temena

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // ***************** SKY *************************
    float skyVertices[] = {
       -1.0f,  0.15f,
        1.0f,  0.15f,
       -1.0f,  1.0f,
        1.0f,  1.0f,
    };
    glBindVertexArray(VAO[5]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyVertices), skyVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);  //koordinate temena
    glEnableVertexAttribArray(0); // Omogu?ite koordinate temena

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // ***************** SAND *************************
    float sandVertices[] = {
       -1.0f,  0.15f,
        1.0f,  0.15f,
       -1.0f,  -1.0f,
        1.0f,  -1.0f,
    };
   
    glBindVertexArray(VAO[6]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[6]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sandVertices), sandVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);  //koordinate temena
    glEnableVertexAttribArray(0); // Omogu?ite koordinate temena

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // ***************** AUTHOR *************************
    float authorVertices[] = {
       0.6f,  -1.0f,   0.0f,  0.0f,
       1.0f,  -1.0f,    1.0f,  0.0f,
      0.6f,  -0.9f,    0.0f,  1.0f,
       1.0f,  -0.9f,     1.0f,  1.0f,
    };


    glBindVertexArray(VAO[10]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[10]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(authorVertices), authorVertices, GL_STATIC_DRAW);

    // Pozicije temena
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // UV koordinate
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //Tekstura Author
    unsigned authorTexture = loadImageToTexture("./res/author.png"); //Ucitavamo teksturu
    glBindTexture(GL_TEXTURE_2D, authorTexture); //Podesavamo teksturu
    //glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(authorShader);
    unsigned uAuthTexLoc = glGetUniformLocation(authorShader, "uTex");
    glUniform1i(uAuthTexLoc, 0); // Indeks teksturne jedinice (sa koje teksture ce se citati boje)
    glUseProgram(0);

    // ***************** GRASS *************************
    float grassVertices[] = {

       -0.65f,  -0.05f,   0.0f,  1.0f,  // Levo-gore
       -0.35f,  -0.05f,   1.0f,  1.0f,  // Desno-gore
       -0.65f, -0.3f,   0.0f,  0.0f,  // Levo-dole
       -0.35f, -0.3f,   1.0f,  0.0f,   // Desno-dole

        -0.75f,  -0.15f,   0.0f,  1.0f,  // Levo-gore
       -0.45f,  -0.15f,   1.0f,  1.0f,  // Desno-gore
       -0.75f, -0.5f,   0.0f,  0.0f,  // Levo-dole
       -0.45f, -0.5f,   1.0f,  0.0f,   // Desno-dole

       -0.65f,  -0.35f,   0.0f,  1.0f,  // Levo-gore
       -0.35f,  -0.35f,   1.0f,  1.0f,  // Desno-gore
       -0.65f, -0.7f,   0.0f,  0.0f,  // Levo-dole
       -0.35f, -0.7f,   1.0f,  0.0f,   // Desno-dole

        -0.45f,  -0.5f,   0.0f,  1.0f,  // Levo-gore
       -0.15f,  -0.5f,   1.0f,  1.0f,  // Desno-gore
       -0.45f, -0.8f,   0.0f,  0.0f,  // Levo-dole
       -0.15f, -0.8f,   1.0f,  0.0f,   // Desno-dole

       //desna obala
       0.65f,  -0.05f,   0.0f,  1.0f,  // Levo-gore
       0.35f,  -0.05f,   1.0f,  1.0f,  // Desno-gore
       0.65f, -0.3f,   0.0f,  0.0f,  // Levo-dole
       0.35f, -0.3f,   1.0f,  0.0f,   // Desno-dole

       0.75f,  -0.15f,   0.0f,  1.0f,  // Levo-gore
       0.45f,  -0.15f,   1.0f,  1.0f,  // Desno-gore
       0.75f, -0.5f,   0.0f,  0.0f,  // Levo-dole
       0.45f, -0.5f,   1.0f,  0.0f,   // Desno-dole

       0.65f,  -0.35f,   0.0f,  1.0f,  // Levo-gore
       0.35f,  -0.35f,   1.0f,  1.0f,  // Desno-gore
       0.65f, -0.7f,   0.0f,  0.0f,  // Levo-dole
       0.35f, -0.7f,   1.0f,  0.0f,   // Desno-dole

       0.45f,  -0.5f,   0.0f,  1.0f,  // Levo-gore
       0.15f,  -0.5f,   1.0f,  1.0f,  // Desno-gore
       0.45f, -0.8f,   0.0f,  0.0f,  // Levo-dole
       0.15f, -0.8f,   1.0f,  0.0f,   // Desno-dole

       //srednji zbun
       -0.2f,  -0.5f,   0.0f,  1.0f,  // Levo-gore
        0.2,  -0.5f,   1.0f,  1.0f,  // Desno-gore
       -0.2f, -0.8f,   0.0f,  0.0f,  // Levo-dole
        0.2, -0.8f,   1.0f,  0.0f,   // Desno-dole

    };

    glEnable(GL_BLEND);  // Omogu?ava blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // Postavljanje blending funkcije za transparenciju

    glBindVertexArray(VAO[7]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[7]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(grassVertices), grassVertices, GL_STATIC_DRAW);

    // Pozicije temena
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // UV koordinate
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //Tekstura Grass
    unsigned grassTexture = loadImageToTexture("./res/grass2.png"); //Ucitavamo teksturu
    glBindTexture(GL_TEXTURE_2D, grassTexture); //Podesavamo teksturu
    //glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(grassShader);
    unsigned uTexLoc = glGetUniformLocation(grassShader, "uTex");
    glUniform1i(uTexLoc, 0); // Indeks teksturne jedinice (sa koje teksture ce se citati boje)
    glUseProgram(0);

    // ***************** FISH *************************
    float fishVertices[] = {
   -0.1f,  -0.3f,   0.0f,  1.0f,  // Levo-gore
   0.1f,   -0.3f,   1.0f,  1.0f,  // Desno-gore
   -0.1f, -0.5f,   0.0f,  0.0f,  // Levo-dole
   0.1f, -0.5f,   1.0f,  0.0f,   // Desno-dole

    };

    glEnable(GL_BLEND);  // Omogu?ava blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // Postavljanje blending funkcije za transparenciju

    glBindVertexArray(VAO[8]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[8]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fishVertices), fishVertices, GL_STATIC_DRAW);

    // Pozicije temena
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // UV koordinate
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //Tekstura
    unsigned fishTexture = loadImageToTexture("./res/fish.png"); //Ucitavamo teksturu
    glBindTexture(GL_TEXTURE_2D, fishTexture); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(fishShader);
    unsigned uFishLoc = glGetUniformLocation(fishShader, "uTex");
    glUniform1i(uFishLoc, 0); // Indeks teksturne jedinice (sa koje teksture ce se citati boje)
    glUseProgram(0);



    // ***************** SUNCE *************************
    float circle[(CRES + 2) * 2];
    float radius = 0.2; //poluprecnik

    circle[0] = 0; //Centar X0
    circle[1] = 0; //Centar Y0
    int i;
    for (i = 0; i <= CRES; i++) {
        float angle = (M_PI * 2.0f) * (float)i / (float)CRES; // Ugao u radijanima
        circle[2 + 2 * i] = radius * cos(angle);             // Xi
        circle[2 + 2 * i + 1] = radius * sin(angle);         // Yi
    }
    glGenVertexArrays(1, &VAO[1]); // Novi VAO za sunce
    glGenBuffers(1, &VBO[1]);
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circle), circle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindVertexArray(0);

    //****************** MESEC ********************
    float centerX = 0.0f;
    float centerY = 0.0f;
    float outerRadius = 0.2f; // Spoljašnji polupre?nik
    float innerRadius = 0.15f; // Unutrašnji polupre?nik

    std::vector<float> outerCircleVertices;
    std::vector<float> innerCircleVertices;

    // Spoljašnji krug
    for (int i = 0; i <= MRES; i++) {
        float angle = (M_PI * 2.0f) * (float)i / (float)MRES;
        outerCircleVertices.push_back(centerX + outerRadius * cos(angle)); // X koordinata
        outerCircleVertices.push_back(centerY + outerRadius * sin(angle)); // Y koordinata
    }

    // Unutrašnji krug
    float offsetX = 0.1f;
    for (int i = 0; i <= MRES; i++) {
        float angle = (M_PI * 2.0f) * (float)i / (float)MRES;
        innerCircleVertices.push_back(centerX - offsetX + innerRadius * cos(angle)); // X koordinata
        innerCircleVertices.push_back(centerY + innerRadius * sin(angle)); // Y koordinata
    }

    glBindVertexArray(VAO_inner);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_inner);
    glBufferData(GL_ARRAY_BUFFER, innerCircleVertices.size() * sizeof(float), &innerCircleVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(VAO_outer);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_outer);
    glBufferData(GL_ARRAY_BUFFER, outerCircleVertices.size() * sizeof(float), &outerCircleVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);



    // ************ OVO JE SVIMA ZAJEDNICKO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    float x = 0;
    float y = 0;

    unsigned int uPosLoc = glGetUniformLocation(sunShader, "uPos");
    unsigned int uScaleLoc = glGetUniformLocation(sunShader, "uScalse");
    unsigned int xScaleLoc = glGetUniformLocation(sunShader, "xScale");
    unsigned int uMaskLoc = glGetUniformLocation(midPyramidShader, "uMask");
    unsigned int uWLoc = glGetUniformLocation(midPyramidShader, "uW");
    unsigned int uHLoc = glGetUniformLocation(midPyramidShader, "uH");
    GLint daySkyLoc = glGetUniformLocation(skyShader, "uDaySky");
    unsigned int uPosLocSky = glGetUniformLocation(skyShader, "uPosSky");
    GLuint uColorLoc = glGetUniformLocation(lakeShader, "uColor");
    GLuint showGrassLocation = glGetUniformLocation(grassShader, "showGrass");
    unsigned int uPosOrbLoc = glGetUniformLocation(starShader, "uPosOrb");

    float r = 0.75;
    float rotationSpeed = 0.3;

    bool orbMove = true;
    bool day = true;
    float orbX = centerX;
    float orbY = centerY;
    float previousFmod = 0.0f;
    float reset = false;
    float resetTime = 0.0f;
    float xMask = 196.0f;
    bool showGrass = true;
    bool isOKeyPressed = false;
    float entranceHeight[3] = { 0.0f, 0.0f, 0.0f };  // Ulaz zatvoren za sve piramide
    float entranceSpeed = 0.01f;  // Brzina otvaranja vrata
    float doorHeight = 0.0f;  // Po?etna visina vrata (zatvorena)
    float openingSpeed = 0.0005f;  // Brzina otvaranja vrata

    // render loop
    // -----------
    float elapsedTime = 0.0f;     // Proteklo vreme od poslednjeg iscrtavanja
    float delayPerLetter = 0.5f;  // Vremenski razmak izme?u prikaza svakog slova
    int currentCharacterIndex = 0;
    std::string text = "Nastavice se na 3D projektu";
    messageSize = text.size();

    float charDisplayInterval = 0.2f; // Pauza izme?u slova
    float startTime = glfwGetTime();

    for (size_t i = 0; i < text.length(); ++i) {
        //letterStartTimes.push_back(startTime + i * charDisplayInterval);
        letterStartTimes.push_back(1);
    }
    float fadeDuration = 0.01f;        // Trajanje bledenja

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        //closing logic
        if (shutdownStarted) {
            double curTime = glfwGetTime();
            double elapsedTime = curTime - shutdownStartTime;

            // Ispis odbrojavanja (za debagovanje)
            //std::cout << "Elapsed Time: " << elapsedTime << " seconds" << std::endl;

            // Ako je prošlo više od 2 sekunde, zatvori prozor
            if (shutdownStarted && elapsedTime >= 2.0) {
                std::cout << "2 seconds elapsed, closing the window!" << std::endl;
                glfwSetWindowShouldClose(window, GL_TRUE);
            }
        }

        //end closing logic

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);

        glClearColor(0.5, 0.5, 0.5, 1.0); //Podesavanje boje pozadine: RGBA (R - Crvena, G - Zelena, B - Plava, A = neprovidno; Opseg od 0 do 1, gdje je 0 crno a 1 svijetlo)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        {
            orbMove = false;
        }
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            orbMove = true;
            reset = true;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            if (xMask < 196) {
                xMask += 0.2f;
                //std::cout << "xMask: " << xMask << std::endl;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            if (xMask > -196) {
                xMask -= 0.2f;
                //std::cout << "xMask: " << xMask << std::endl;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            showGrass = false;
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            showGrass = true;
        }
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
            isOKeyPressed = true;
        }

        //Provera da li je kliknuto na vrata
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            glfwMakeContextCurrent(window);
            glfwSetCursorPosCallback(window, cursor_position_callback);
        }

        if (isOKeyPressed) {
            doorHeight += openingSpeed;  // Postepeno otvori vrata
            if (doorHeight > 0.1f) {
                doorHeight = 0.1f;  // Ako su vrata potpuno otvorena, zadrži vrednost
            }
        }

        // Izra?unavanje aspekt proporcije i skaliranja
        int width, height;
        const float epsilon = 1e-6f;
        glfwGetFramebufferSize(window, &width, &height);
        float aspect = ((float)width + epsilon) / ((float)height + epsilon);

        float xScale = aspect >= 1.0f ? 1.0f / aspect : 1.0f;
        float yScale = aspect >= 1.0f ? 1.0f : aspect;

        glUniform2f(uScaleLoc, xScale, yScale);
        glUniform1f(xScaleLoc, xScale);

        float xOffset = 0.0;
        float yOffset = 0.0;
        if (orbMove) {
            //float xOffset = 1.7 * sin(glfwGetTime() * rotationSpeed); // Horizontalno pomeranje od leve ka desnoj strani
            //float yOffset = 1.2 * cos(glfwGetTime() * rotationSpeed); // Vertikalno pomeranje (izlazak ka gore)

            if (reset) {
                resetTime = glfwGetTime(); // Resetujte vreme na trenutni trenutak
                reset = false; // Završite resetovanje
                day = false;  // postavljam stanje na noc zato sto ce sledeci if sigurno biti ispunjen
                previousFmod = 10.0f;
            }
            float elapsedTime = glfwGetTime() - resetTime;

            float currentFmod = fmod(elapsedTime, 5.0f);

            //std::cout << "currentFmod: " << currentFmod << ", previousFmod: " << previousFmod << std::endl;

            if (currentFmod < previousFmod) {
                day = !day; // Prebacivanje dana i no?i             
                //std::cout << "Switching day state: " << (day ? "Day" : "Night") << std::endl;
            }

            previousFmod = currentFmod;

            float phaseShift = M_PI / 4.0f;
            xOffset = 1.7 * sin(fmod(elapsedTime, 5.0f) * rotationSpeed - phaseShift); // Horizontalno pomeranje od leve ka desnoj strani
            yOffset = 1.2 * cos(fmod(elapsedTime, 5.0f) * rotationSpeed - phaseShift); // Vertikalno pomeranje (izlazak ka gore)

            glUniform2f(uPosLoc, xOffset, yOffset);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        //******************* Sand *********************
        glUseProgram(sandShader);
        glm::vec3 sandColor = glm::vec3(1.0f, 0.8f, 0.2f); // Zlatno-bež boja za pustinjski pesak
        GLuint uSandColorLoc = glGetUniformLocation(sandShader, "uColor");
        glUniform3f(uSandColorLoc, sandColor.x, sandColor.y, sandColor.z);

        glBindVertexArray(VAO[6]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        //************* End Sand ***************

       
        //******************* Sky *********************
        glUseProgram(skyShader);
        glUniform2f(uPosLocSky, xOffset, yOffset);
        glUniform1i(uHLoc, wHeight);
        if (day) {
            glUniform1i(daySkyLoc, GL_TRUE);
        }
        else {
            glUniform1i(daySkyLoc, GL_FALSE);
        }
        GLfloat values[2];
        glGetUniformfv(skyShader, uPosLocSky, values);  // Dobijamo vrednosti uniformnih promenljivih
        //std::cout << "uPosSky.x: " << values[0] << ", uPosSky.y: " << values[1] << std::endl;
        glBindVertexArray(VAO[5]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);  // Koristi TRIANGLE_STRIP za pravougaonik
        //************* End Sky ***************


        //************* Pyramids ***************
        glUseProgram(pyramidShader);
        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_TRIANGLES, 0, 9); //To i nacrtamo
        //************** End Pyramids *****************

        //************* Mid Pyramid ***************

        glUseProgram(midPyramidShader);
        glUniform1f(uMaskLoc, xMask);

        int uMaskLoc = glGetUniformLocation(midPyramidShader, "uMask");
        if (uMaskLoc == -1) {
            std::cerr << "Uniform 'uMask' not found or not used in shader!" << std::endl;
        }

        glUniform1i(uWLoc, wWidth);
        glUniform1i(uHLoc, wHeight);
        glBindVertexArray(VAO[4]);
        glDrawArrays(GL_TRIANGLES, 0, 3); //To i nacrtamo
        //************** End Mid Pyramid *****************

        //************** Doors *****************
        glm::vec2 pyramidPositions[3] = {
            glm::vec2(0.0f, 0.0f),  // Piramida 1
            glm::vec2(0.4f, -0.1f),  // Piramida 2
            glm::vec2(-0.4f, -0.1f)  // Piramida 3
        };
        for (int i = 0; i < 3; i++) {

            glUseProgram(doorShader);

            GLuint translationLoc = glGetUniformLocation(doorShader, "uTranslation");
            glUniform2f(translationLoc, pyramidPositions[i].x, pyramidPositions[i].y);

            glBindVertexArray(VAO[9]);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);  // 6 vrhova za 2 trokuta (3*2)
        }
        //************** End Doors *****************

        //************** Door Covers *****************
        for (int i = 0; i < 3; i++) {

            glUseProgram(doorCoverShader);

            GLuint doorHeightLoc = glGetUniformLocation(doorCoverShader, "doorHeight");
            glUniform1f(doorHeightLoc, doorHeight);

            GLuint coverTranslationLoc = glGetUniformLocation(doorCoverShader, "uTranslation");
            glUniform2f(coverTranslationLoc, pyramidPositions[i].x, pyramidPositions[i].y);

            glBindVertexArray(VAO[11]);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);  // 6 vrhova za 2 trokuta (3*2)
        }
        //******************* End Door Covers *********************

        //******************* FISH *********************
        glUseProgram(fishShader);
        glBindVertexArray(VAO[8]);

        GLuint showFishLocation = glGetUniformLocation(fishShader, "showFish");
        glUniform1i(showFishLocation, showGrass ? GL_FALSE : GL_TRUE);

        float currentTimeFish = glfwGetTime(); // Proteklo vreme
        GLuint timeLocation = glGetUniformLocation(fishShader, "time");
        glUniform1f(timeLocation, currentTimeFish);

        float movement = sin(currentTimeFish); // Trenutna pozicija ribe (-1.0 do 1.0)

        // Odre?ivanje smera kretanja
        bool movingRight = movement >= 0.0;

        // Prosle?ivanje vrednosti uniform promenljivoj
        GLuint movingRightLocation = glGetUniformLocation(fishShader, "movingRight");
        glUniform1i(movingRightLocation, movingRight ? GL_TRUE : GL_FALSE);


        glActiveTexture(GL_TEXTURE0); //tekstura koja se bind-uje nakon ovoga ce se koristiti sa SAMPLER2D uniformom u sejderu koja odgovara njenom indeksu
        glBindTexture(GL_TEXTURE_2D, fishTexture);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);  // Koristi TRIANGLE_STRIP za pravougaonik

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glUseProgram(0);
        //************* End Fish ***************


        //************* Lake ***************
        glUseProgram(lakeShader);
        glm::vec3 lakeColor = glm::vec3(0.0f, 0.4f, 0.8f); // Zlatno-bež boja za pustinjski pesak
        glUniform3f(uColorLoc, lakeColor.x, lakeColor.y, lakeColor.z);

        GLuint lakeTransparentLoc = glGetUniformLocation(lakeShader, "lakeTransparent");
        glUniform1i(lakeTransparentLoc, showGrass ? GL_TRUE : GL_FALSE);

        glBindVertexArray(VAO[2]);
        glDrawArrays(GL_TRIANGLE_FAN, 0, CRES + 2); //To i nacrtamo
        //************** End Lake *****************

        //******************* GRASS *********************
        glUseProgram(grassShader);
        glBindVertexArray(VAO[7]);

        GLuint showGrassLocation = glGetUniformLocation(grassShader, "showGrass");
        glUniform1i(showGrassLocation, showGrass ? GL_TRUE : GL_FALSE);

        glActiveTexture(GL_TEXTURE0); //tekstura koja se bind-uje nakon ovoga ce se koristiti sa SAMPLER2D uniformom u sejderu koja odgovara njenom indeksu
        glBindTexture(GL_TEXTURE_2D, grassTexture);

        for (int i = 0; i < 9; i++) {
            glDrawArrays(GL_TRIANGLE_STRIP, i * 4, 4); // Nacrtaj svaki pravougaonik
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glUseProgram(0);
        //************* End Grass ***************

         //******************* Author *********************
        glUseProgram(authorShader);
        glBindVertexArray(VAO[10]);

        glActiveTexture(GL_TEXTURE0); //tekstura koja se bind-uje nakon ovoga ce se koristiti sa SAMPLER2D uniformom u sejderu koja odgovara njenom indeksu
        glBindTexture(GL_TEXTURE_2D, authorTexture);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);  // Koristi TRIANGLE_STRIP za pravougaonik

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glUseProgram(0);
        //************* End Author ***************

        //************* Stars ***************     
        glUseProgram(starShader);

        glm::vec2 starPositions[11] = {
            {-0.3f, 0.3f}, // Gornji levi ugao
            {-0.1f, 0.2f},
            {0.2f, 0.0f},  // Sredina gore
            {1.1f, 0.4f},
            {0.4f, 0.4f},  // Gornji desni ugao
            {-0.2f, -0.2f}, // Dole levo
            {0.5f, 0.1f},   // Dole desno
            {1.3f, 0.3f},
            {1.1f, 0.4f},  // Gornji desni ugao
            {1.0f, -0.1f}, // Dole levo
            {1.2f, 0.1f}
        };

        for (int i = 0; i < 11; i++) {
            // Postavi uniform promenljivu za boju (sve zvezde zlatne)
            glm::vec3 starColor = glm::vec3(1.0f, 0.84f, 0.0f); // Zlatno-žuta boja
            glUniform3f(uColorLoc, starColor.x, starColor.y, starColor.z);

            // Postavi uniform za translaciju (pozicija zvezde)
            glm::vec2 position = starPositions[i];
            glUniform2f(glGetUniformLocation(starShader, "uTranslation"), position.x, position.y);
            glUniform2f(uPosOrbLoc, xOffset, yOffset);

            GLuint showStarsLoc = glGetUniformLocation(starShader, "showStars");
            glUniform1i(showStarsLoc, day ? GL_TRUE : GL_FALSE);

            // Crtaj zvezdu
            glBindVertexArray(VAO[3]);
            glDrawArrays(GL_TRIANGLE_FAN, 0, numPoints * 2 + 1);
        }

        //************** End Stars *****************

         // ********* SHOW TEXT *********
        // input
        
        if (showText) {
            processInput(window);

            float currentTime = glfwGetTime();
            if (currentTime - elapsedTime >= delayPerLetter)
            {
                // Pove?aj broj ispisanih slova
                currentCharacterIndex++;
                elapsedTime = currentTime;
            }

            shader.setFloat("time", glfwGetTime());
            shader.setFloat("fadeDuration", 1.0f);          // Trajanje bledenja u sekundama
            shader.setFloat("charDisplayInterval", 0.2f);   // Pauza izme?u slova
            shader.setFloat("startTime", startTime);

            RenderText(shader, text.substr(0, currentCharacterIndex), 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));

            if (currentCharacterIndex > text.size() && !startFading) {
                currentCharacterIndex = text.size(); // Restart prikaz teksta
                startFading = true;
                std::cout << "Eo poceo je fade" << std::endl;
            }

        }
        //************ End Show Text ****************

        if (day) {
            //******************* SUNCE *********************
            glUseProgram(sunShader);
            glBindVertexArray(VAO[1]);
            glDrawArrays(GL_TRIANGLE_FAN, 0, CRES + 2);
        }
        else {
            //******************* MESEC *********************
            glClear(GL_STENCIL_BUFFER_BIT); // Resetuje stencil buffer

            glEnable(GL_STENCIL_TEST);
            glUseProgram(sunShader);

            // 3. Prvi prolaz: Nacrtaj unutrašnji krug u stencil buffer
            glStencilFunc(GL_ALWAYS, 1, 0xFF);          // Piši vrednost 1 u stencil buffer
            glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);  // Postavi vrednost u buffer
            glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // Isklju?i bojenje
            glDepthMask(GL_FALSE);  // Isklju?i zapis u depth buffer

            glBindVertexArray(VAO_inner); // VAO za unutrašnji krug
            glDrawArrays(GL_TRIANGLE_FAN, 0, MRES + 1);

            // 4. Drugi prolaz: Nacrtaj spoljašnji krug, koriste?i stencil test
            glStencilFunc(GL_NOTEQUAL, 1, 0xFF);       // Nacrtaj samo gde stencil != 1
            glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);    // Ne menjaj stencil vrednost
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); // Uklju?i bojenje
            glDepthMask(GL_TRUE);  // Uklju?i zapis u depth buffer

            glBindVertexArray(VAO_outer); // VAO za spoljašnji krug
            glDrawArrays(GL_TRIANGLE_FAN, 0, MRES + 1);

            // 5. Isklju?i stencil test

            // Iskoristi VAO za polumesec (spoljašnji krug sa unutrašnjim)
            glBindVertexArray(VAO_outer);


            glDrawArrays(GL_TRIANGLE_FAN, 0, MRES + 1);

            glDisable(GL_STENCIL_TEST);
        }
        // ********************* End Sun and Moon ************************


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glDeleteBuffers(13, VBO);
    glDeleteVertexArrays(13, VAO);
    glDeleteProgram(sunShader);
    glDeleteProgram(pyramidShader);
    glDeleteProgram(lakeShader);
    glDeleteProgram(midPyramidShader);
    glDeleteProgram(skyShader);
    glDeleteProgram(grassShader);
    glDeleteProgram(fishShader);
    glDeleteProgram(starShader);
    glDeleteProgram(doorShader);
    glDeleteProgram(authorShader);
    glDeleteProgram(sandShader);
    glDeleteProgram(doorCoverShader);


    glfwTerminate();
    return 0;
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    // Dobijamo poziciju miša u koordinatama prozora
    unsigned int wWidth = 1920; //Podesiti u zavisnosti od rezolucije monitora
    unsigned int wHeight = 1080;

    float normX = (2.0f * xpos / wWidth) - 1.0f;  // Skalira u opseg [-1, 1] na X osi
    float normY = 1.0f - (2.0f * ypos / wHeight); // Skalira u opseg [-1, 1] na Y osi

    if (normX >= -0.025f && normX <= 0.025f && normY >= 0.0f && normY <= 0.1f) {
        std::cout << "Mouse clicked at: " << normX << ", " << normY << std::endl;
        showText = true;
    }

    // Druga piramida (vrata na (0.4f, -0.1f))
    if (normX >= 0.375f && normX <= 0.425f && normY >= -0.1f && normY <= 0.0f) {
        std::cout << "HEJ - Piramida 3" << std::endl;
        showText = true;
    }

    // Tre?a piramida (vrata na (-0.4f, -0.1f))
    if (normX >= -0.425f && normX <= -0.375f && normY >= -0.1f && normY <= 0.0f) {
        std::cout << "HEJ - Piramida 1" << std::endl;
        showText = true;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    //if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    //    popup_menu();
    if ((mods & GLFW_MOD_SHIFT) && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // Zatvori prozor
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

}


unsigned int compileShader(GLenum type, const char* source)
{
    //Uzima kod u fajlu na putanji "source", kompajlira ga i vraca sejder tipa "type"
    //Citanje izvornog koda iz fajla
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Uspjesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
    }
     std::string temp = ss.str();
     const char* sourceCode = temp.c_str(); //Izvorni kod sejdera koji citamo iz fajla na putanji "source"

    int shader = glCreateShader(type); //Napravimo prazan sejder odredjenog tipa (vertex ili fragment)
    
    int success; //Da li je kompajliranje bilo uspjesno (1 - da)
    char infoLog[512]; //Poruka o gresci (Objasnjava sta je puklo unutar sejdera)
    glShaderSource(shader, 1, &sourceCode, NULL); //Postavi izvorni kod sejdera
    glCompileShader(shader); //Kompajliraj sejder

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success); //Provjeri da li je sejder uspjesno kompajliran
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog); //Pribavi poruku o gresci
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" sejder ima gresku! Greska: \n");
        printf(infoLog);
    }
    return shader;
}
unsigned int createShader(const char* vsSource, const char* fsSource)
{
    //Pravi objedinjeni sejder program koji se sastoji od Vertex sejdera ciji je kod na putanji vsSource

    unsigned int program; //Objedinjeni sejder
    unsigned int vertexShader; //Verteks sejder (za prostorne podatke)
    unsigned int fragmentShader; //Fragment sejder (za boje, teksture itd)

    program = glCreateProgram(); //Napravi prazan objedinjeni sejder program

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource); //Napravi i kompajliraj vertex sejder
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource); //Napravi i kompajliraj fragment sejder

    //Zakaci verteks i fragment sejdere za objedinjeni program
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program); //Povezi ih u jedan objedinjeni sejder program
    glValidateProgram(program); //Izvrsi provjeru novopecenog programa

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success); //Slicno kao za sejdere
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }

    //Posto su kodovi sejdera u objedinjenom sejderu, oni pojedinacni programi nam ne trebaju, pa ih brisemo zarad ustede na memoriji
    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


static unsigned loadImageToTexture(const char* filePath) {
    int TextureWidth;
    int TextureHeight;
    int TextureChannels;
    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
    if (ImageData != NULL)
    {
        //Slike se osnovno ucitavaju naopako pa se moraju ispraviti da budu uspravne
        stbi__vertical_flip(ImageData, TextureWidth, TextureHeight, TextureChannels);

        // Provjerava koji je format boja ucitane slike
        GLint InternalFormat = -1;
        switch (TextureChannels) {
        case 1: InternalFormat = GL_RED; break;
        case 2: InternalFormat = GL_RG; break;
        case 3: InternalFormat = GL_RGB; break;
        case 4: InternalFormat = GL_RGBA; break;
        default: InternalFormat = GL_RGB; break;
        }

        unsigned int Texture;
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, TextureWidth, TextureHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, ImageData);
        glBindTexture(GL_TEXTURE_2D, 0);
        // oslobadjanje memorije zauzete sa stbi_load posto vise nije potrebna
        stbi_image_free(ImageData);
        return Texture;
    }
    else
    {
        std::cout << "Textura nije ucitana! Putanja texture: " << filePath << std::endl;
        stbi_image_free(ImageData);
        return 0;
    }
}


// render line of text
// -------------------
void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color)
{
    // activate corresponding render state	
    shader.use();

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO[12]);

    // iterate through all characters
    std::string::const_iterator c;
    int i = 0;
    for (c = text.begin(); c != text.end(); c++, i++)
    {

        if (startFading) {
            //std::cout << "Letter " << i << ": " << letterStartTimes[i] << std::endl;
            if (i == 0) {
                letterStartTimes[0] -= 0.01;
            }
            else if (letterStartTimes[i - 1] <= 0) {
                letterStartTimes[i] -= 0.01;
            }
            if (letterStartTimes[messageSize - 1] <= 0) {
                //poteraj ono za zatvaranje programa
                if (shutdownStarted == false) {
                    shutdownStarted = true;
                    shutdownStartTime = glfwGetTime();
                    std::cout << "Bio sam ovde EEEEJJJJJ" << std::endl;
                }
            }
            glUniform4f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z, letterStartTimes[i]);

        }
        else {
            glUniform4f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z, 1.0);
        }

        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO[12]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
