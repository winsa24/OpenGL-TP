// ----------------------------------------------------------------------------
// main.cpp
//
//  Created on: 24 Jul 2020
//      Author: Kiwon Um
//        Mail: kiwon.um@telecom-paris.fr
//
// Description: IGR201 Practical; OpenGL and Shaders (DO NOT distribute!)
//
// Copyright 2020 Kiwon Um
//
// The copyright to the computer program(s) herein is the property of Kiwon Um,
// Telecom Paris, France. The program(s) may be used and/or copied only with
// the written permission of Kiwon Um or in accordance with the terms and
// conditions stipulated in the agreement/contract under which the program(s)
// have been supplied.
// ----------------------------------------------------------------------------

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#define _USE_MATH_DEFINES
#include <cmath>
#include <memory>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Window parameters
GLFWwindow *g_window = nullptr;

// GPU objects
GLuint g_program = 0; // A GPU program contains at least a vertex shader and a fragment shader

// OpenGL identifiers
GLuint g_vao = 0;
GLuint g_posVbo = 0;
GLuint g_colorVbo = 0;
GLuint g_ibo = 0;

// All vertex positions packed in one array [x0, y0, z0, x1, y1, z1, ...]
std::vector<float> g_vertexPositions;
// All triangle indices packed in one array [v00, v01, v02, v10, v11, v12, ...] with vij the index of j-th vertex of the i-th triangle
std::vector<unsigned int> g_triangleIndices;
std::vector<float> g_vertexColors;

// main.cpp ...
class Mesh {
public:
    void init(); // should properly set up the geometry buffer
    void render(); // should be called in the main rendering loop
    //static std::shared_ptr<Mesh> genSphere(const size_t resolution=16); // should generate a unit sphere
    void genSphere(){
        float x, y, z, xy;                              // vertex position
        float radius = 1.0f;
        float PI = 3.14159265;
        float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
    //    float s, t;                                     // vertex texCoord

        int sectorCount = 16;
        int stackCount = 16;
        float sectorStep = 2 * PI / sectorCount;
        float stackStep = PI / stackCount;
        float sectorAngle, stackAngle;

        for(int i = 0; i <= stackCount; ++i)
        {
            stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
            xy = radius * cosf(stackAngle);             // r * cos(u)
            z = radius * sinf(stackAngle);              // r * sin(u)

            // add (sectorCount+1) vertices per stack
            // the first and last vertices have same position and normal, but different tex coords
            for(int j = 0; j <= sectorCount; ++j)
            {
                sectorAngle = j * sectorStep;           // starting from 0 to 2pi

                // vertex position (x, y, z)
                x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
                y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
                m_vertexPositions.push_back(x);
                m_vertexPositions.push_back(y);
                m_vertexPositions.push_back(z);



                //normalized vertex normal (nx, ny, nz)
                nx = x * lengthInv;
                ny = y * lengthInv;
                nz = z * lengthInv;
                m_vertexNormals.push_back(nx);
                m_vertexNormals.push_back(ny);
                m_vertexNormals.push_back(nz);

            }
        }
        int k1, k2;
        for(int i = 0; i < stackCount; ++i)
        {
            k1 = i * (sectorCount + 1);     // beginning of current stack
            k2 = k1 + sectorCount + 1;      // beginning of next stack

            for(int j = 0; j < sectorCount; ++j, ++k1, ++k2)
            {
                // 2 triangles per sector excluding first and last stacks
                // k1 => k2 => k1+1
                if(i != 0)
                {
                    m_triangleIndices.push_back(k1);
                    m_triangleIndices.push_back(k2);
                    m_triangleIndices.push_back(k1 + 1);
                }

                // k1+1 => k2 => k2+1
                if(i != (stackCount-1))
                {
                    m_triangleIndices.push_back(k1 + 1);
                    m_triangleIndices.push_back(k2);
                    m_triangleIndices.push_back(k2 + 1);
                }
            }
        }

    };
// ...
private:
    std::vector<float> m_vertexPositions;
    std::vector<float> m_vertexNormals;
    std::vector<unsigned int> m_triangleIndices;
    GLuint m_vao = 0;
    GLuint m_posVbo = 0;
    GLuint m_normalVbo = 0;
    GLuint m_ibo = 0;
// ...

};

//std::shared_ptr<Mesh> Mesh:: genSphere(){


// Basic camera model
class Camera {
public:
  inline float getFov() const { return m_fov; }
  inline void setFoV(const float f) { m_fov = f; }
  inline float getAspectRatio() const { return m_aspectRatio; }
  inline void setAspectRatio(const float a) { m_aspectRatio = a; }
  inline float getNear() const { return m_near; }
  inline void setNear(const float n) { m_near = n; }
  inline float getFar() const { return m_far; }
  inline void setFar(const float n) { m_far = n; }
  inline void setPosition(const glm::vec3 &p) { m_pos = p; }
  inline glm::vec3 getPosition() { return m_pos; }

  inline glm::mat4 computeViewMatrix() const {
    return glm::lookAt(m_pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
  }

  // Returns the projection matrix stemming from the camera intrinsic parameter.
  inline glm::mat4 computeProjectionMatrix() const {
    return glm::perspective(glm::radians(m_fov), m_aspectRatio, m_near, m_far);
  }

private:
  glm::vec3 m_pos = glm::vec3(0, 0, 0);
  float m_fov = 45.f;        // Field of view, in degrees
  float m_aspectRatio = 1.f; // Ratio between the width and the height of the image
  float m_near = 0.1f; // Distance before which geometry is excluded fromt he rasterization process
  float m_far = 10.f; // Distance after which the geometry is excluded fromt he rasterization process
};
Camera g_camera;

GLuint loadTextureFromFileToGPU(const std::string &filename) {
  int width, height, numComponents;
  // Loading the image in CPU memory using stbd_image
  unsigned char *data = stbi_load(
    filename.c_str(),
    &width, &height,
    &numComponents, // 1 for a 8 bit greyscale image, 3 for 24bits RGB image, 4 for 32bits RGBA image
    0);

  GLuint texID;
  // TODO:

  // Freeing the now useless CPU memory
  stbi_image_free(data);
  glBindTexture(GL_TEXTURE_2D, 0); // unbind the texture

  return texID;
}

// Executed each time the window is resized. Adjust the aspect ratio and the rendering viewport to the current window.
void windowSizeCallback(GLFWwindow* window, int width, int height) {
  g_camera.setAspectRatio(static_cast<float>(width)/static_cast<float>(height));
  glViewport(0, 0, (GLint)width, (GLint)height); // Dimension of the rendering region in the window
}

// Executed each time a key is entered.
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if(action == GLFW_PRESS && key == GLFW_KEY_W) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else if(action == GLFW_PRESS && key == GLFW_KEY_F) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  } else if(action == GLFW_PRESS && (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)) {
    glfwSetWindowShouldClose(window, true); // Closes the application if the escape key is pressed
  }
}

void errorCallback(int error, const char *desc) {
  std::cout <<  "Error " << error << ": " << desc << std::endl;
}

void initGLFW() {
  glfwSetErrorCallback(errorCallback);

  // Initialize GLFW, the library responsible for window management
  if(!glfwInit()) {
    std::cerr << "ERROR: Failed to init GLFW" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  // Before creating the window, set some option flags
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

  // Create the window
  g_window = glfwCreateWindow(
    1024, 768,
    "Interactive 3D Applications (OpenGL) - Simple Solar System",
    nullptr, nullptr);
  if(!g_window) {
    std::cerr << "ERROR: Failed to open window" << std::endl;
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }

  // Load the OpenGL context in the GLFW window using GLAD OpenGL wrangler
  glfwMakeContextCurrent(g_window);
  glfwSetWindowSizeCallback(g_window, windowSizeCallback);
  glfwSetKeyCallback(g_window, keyCallback);
}

void initOpenGL() {
  // Load extensions for modern OpenGL
  if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "ERROR: Failed to initialize OpenGL context" << std::endl;
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }

  glCullFace(GL_BACK); // Specifies the faces to cull (here the ones pointing away from the camera)
  glEnable(GL_CULL_FACE); // Enables face culling (based on the orientation defined by the CW/CCW enumeration).
  glDepthFunc(GL_LESS);   // Specify the depth test for the z-buffer
  glEnable(GL_DEPTH_TEST);      // Enable the z-buffer test in the rasterization
  glClearColor(0.7f, 0.7f, 0.7f, 1.0f); // specify the background color, used any time the framebuffer is cleared
}

// Loads the content of an ASCII file in a standard C++ string
std::string file2String(const std::string &filename) {
  std::ifstream t(filename.c_str());
  std::stringstream buffer;
  buffer << t.rdbuf();
  return buffer.str();
}

// Loads and compile a shader, before attaching it to a program
void loadShader(GLuint program, GLenum type, const std::string &shaderFilename) {
  GLuint shader = glCreateShader(type); // Create the shader, e.g., a vertex shader to be applied to every single vertex of a mesh
  std::string shaderSourceString = file2String(shaderFilename); // Loads the shader source from a file to a C++ string
  const GLchar *shaderSource = (const GLchar *)shaderSourceString.c_str(); // Interface the C++ string through a C pointer
  glShaderSource(shader, 1, &shaderSource, NULL); // load the vertex shader code
  glCompileShader(shader);
  glAttachShader(program, shader);
  glDeleteShader(shader);
}

void initGPUprogram() {
  g_program = glCreateProgram(); // Create a GPU program, i.e., two central shaders of the graphics pipeline
  loadShader(g_program, GL_VERTEX_SHADER, "vertexShader.glsl");
  loadShader(g_program, GL_FRAGMENT_SHADER, "fragmentShader.glsl");
  glLinkProgram(g_program); // The main GPU program is ready to be handle streams of polygons

  glUseProgram(g_program);
}

void task1(){

    //    g_vertexPositions.push_back(0);
    //    g_vertexPositions.push_back(0);
    //    g_vertexPositions.push_back(0);
    //    g_vertexPositions.push_back(1.0);
    //    g_vertexPositions.push_back(0);
    //    g_vertexPositions.push_back(0);
    //    g_vertexPositions.push_back(0);
    //    g_vertexPositions.push_back(1.0);
    //    g_vertexPositions.push_back(0);
    //    g_vertexPositions = { // the array of vertex positions [x0, y0, z0, x1, y1, z1, ...]
    //        0.f, 0.f, 0.f,
    //        1.f, 0.f, 0.f,
    //        0.f, 1.f, 0.f
    //    };
    //    g_triangleIndices.push_back(0);
    //    g_triangleIndices.push_back(1);
    //    g_triangleIndices.push_back(2);
        //g_triangleIndices = { 0, 1, 2 }; // indices just for one triangle

    //    g_vertexColors.push_back(1.0);
    //    g_vertexColors.push_back(1.0);
    //    g_vertexColors.push_back(0.0);

    //    g_vertexColors.push_back(0.0);
    //    g_vertexColors.push_back(1.0);
    //    g_vertexColors.push_back(0.0);

    //    g_vertexColors.push_back(0.0);
    //    g_vertexColors.push_back(0.0);
    //    g_vertexColors.push_back(1.0);


    //    g_vertexColors = { // the array of vertex colors [r0, g0, b0, r1, g1, b1, ...]
    //    1.f, 0.f, 0.f,
    //    0.f, 1.f, 0.f,
    //    0.f, 0.f, 1.f
    //    };
}
void task2(){
    float x, y, z, xy;                              // vertex position
    float radius = 1.0f;
    float PI = 3.14159265;
//    float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
//    float s, t;                                     // vertex texCoord

    int sectorCount = 16;
    int stackCount = 16;
    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;

    for(int i = 0; i <= stackCount; ++i)
    {
        stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for(int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            g_vertexPositions.push_back(x);
            g_vertexPositions.push_back(y);
            g_vertexPositions.push_back(z);
//            g_vertexColors.push_back(x);
//            g_vertexColors.push_back(y);
//            g_vertexColors.push_back(z);
            g_vertexColors.push_back(0);
            g_vertexColors.push_back(0);
            g_vertexColors.push_back(0);
//            vertices.push_back(x);
//            vertices.push_back(y);
//            vertices.push_back(z);

            // normalized vertex normal (nx, ny, nz)
//            nx = x * lengthInv;
//            ny = y * lengthInv;
//            nz = z * lengthInv;
//            normals.push_back(nx);
//            normals.push_back(ny);
//            normals.push_back(nz);

//            // vertex tex coord (s, t) range between [0, 1]
//            s = (float)j / sectorCount;
//            t = (float)i / stackCount;
//            texCoords.push_back(s);
//            texCoords.push_back(t);

        }
    }
    int k1, k2;
    for(int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack

        for(int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if(i != 0)
            {
                g_triangleIndices.push_back(k1);
                g_triangleIndices.push_back(k2);
                g_triangleIndices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if(i != (stackCount-1))
            {
                g_triangleIndices.push_back(k1 + 1);
                g_triangleIndices.push_back(k2);
                g_triangleIndices.push_back(k2 + 1);
            }
        }
    }


}
// Define your mesh(es) in the CPU memory
void initCPUgeometry() {
  // TODO:

    Mesh sphere;
    sphere.genSphere();


}

void initGPUgeometry() {
  // Create a single handle that joins together attributes (vertex positions,
  // normals) and connectivity (triangles indices)
  //glCreateVertexArrays(1, &g_vao);

  glGenVertexArrays(1, &g_vao); // If your system doesn't support OpenGL 4.5, you should use this instead of glCreateVertexArrays.
  glBindVertexArray(g_vao);

  // Generate a GPU buffer to store the positions of the vertices
  size_t vertexBufferSize = sizeof(float)*g_vertexPositions.size(); // Gather the size of the buffer from the CPU-side vector
//  glCreateBuffers(1, &g_posVbo);
//  glNamedBufferStorage(g_posVbo, vertexBufferSize, NULL, GL_DYNAMIC_STORAGE_BIT); // Create a data storage on the GPU
//  glNamedBufferSubData(g_posVbo, 0, vertexBufferSize, g_vertexPositions.data()); // Fill the data storage from a CPU array
//  glBindBuffer(GL_ARRAY_BUFFER, g_posVbo);
//  glEnableVertexAttribArray(0);
//  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
  

  // If your system doesn't support OpenGL 4.5, you should replace the upper code block with this.
  glGenBuffers(1, &g_posVbo);
  glBindBuffer(GL_ARRAY_BUFFER, g_posVbo);
  glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, g_vertexPositions.data(), GL_DYNAMIC_READ);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);



  size_t colorBufferSize = sizeof(float)*g_vertexColors.size();
  glGenBuffers(1, &g_colorVbo);
  glBindBuffer(GL_ARRAY_BUFFER, g_colorVbo);
  glBufferData(GL_ARRAY_BUFFER, colorBufferSize, g_vertexColors.data(), GL_DYNAMIC_READ);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);

  glBindVertexArray(0); // deactivate the VAO for now, will be activated at rendering time

  //glBindVertexArray(1); // deactivate the VAO for now, will be activated at rendering time


  // Same for the index buffer that stores the list of indices of the
  // triangles forming the mesh
  size_t indexBufferSize = sizeof(unsigned int)*g_triangleIndices.size();
//  glCreateBuffers(1, &g_ibo);
//  glNamedBufferStorage(g_ibo, indexBufferSize, NULL, GL_DYNAMIC_STORAGE_BIT);
//  glNamedBufferSubData(g_ibo, 0, indexBufferSize, g_triangleIndices.data());

  // As before, for older OpenGL, use this.
  glGenBuffers(1, &g_ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, g_triangleIndices.data(), GL_DYNAMIC_READ);
}

void initCamera() {
  //std::cout << "qwr5" << std::endl;

  int width, height;
  glfwGetWindowSize(g_window, &width, &height);
  g_camera.setAspectRatio(static_cast<float>(width)/static_cast<float>(height));

  g_camera.setPosition(glm::vec3(0.0, 0.0, 3.0));
  g_camera.setNear(0.1);
  g_camera.setFar(80.1);
  //std::cout << "qwr4" << std::endl;
}

void init() {
  initGLFW();
  initOpenGL();
  initCPUgeometry();
  initGPUprogram();
  initGPUgeometry();
  initCamera();
}

void clear() {
  glDeleteProgram(g_program);

  glfwDestroyWindow(g_window);
  glfwTerminate();
}

// The main rendering call
void render() {

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Erase the color and z buffers.

  const glm::mat4 viewMatrix = g_camera.computeViewMatrix();
  const glm::mat4 projMatrix = g_camera.computeProjectionMatrix();

  glUniformMatrix4fv(glGetUniformLocation(g_program, "viewMat"), 1, GL_FALSE, glm::value_ptr(viewMatrix)); // compute the view matrix of the camera and pass it to the GPU program
  glUniformMatrix4fv(glGetUniformLocation(g_program, "projMat"), 1, GL_FALSE, glm::value_ptr(projMatrix)); // compute the projection matrix of the camera and pass it to the GPU program

  glBindVertexArray(g_vao);     // bind the VAO storing geometry data
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ibo); // bind the IBO storing geometry data
  glDrawElements(GL_TRIANGLES, g_triangleIndices.size(), GL_UNSIGNED_INT, 0); // Call for rendering: stream the current GPU geometry through the current GPU program
}

// Update any accessible variable based on the current time
void update(const float currentTimeInSec) {
//   std::cout << currentTimeInSec << std::endl;

}

int main(int argc, char ** argv) {
  init(); // Your initialization code (user interface, OpenGL states, scene with geometry, material, lights, etc)
  while(!glfwWindowShouldClose(g_window)) {
    update(static_cast<float>(glfwGetTime()));
    render();
    glfwSwapBuffers(g_window);
    glfwPollEvents();
  }
  clear();
  return EXIT_SUCCESS;
}
