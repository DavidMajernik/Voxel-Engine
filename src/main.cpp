#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "C:\Users\dpmaj\LearnC++\SlimeCraft\LinkerStuff\dependencies\stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Shader.h"
#include "Texture.h"
#include "PlayerController.h"

#include <iostream>
#include "World/Chunk.h"
#include "World/World.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void renderUnitCube();


// settings
const unsigned int SCR_WIDTH = 2400;
const unsigned int SCR_HEIGHT = 1800;

//camera
Camera camera(glm::vec3(0.0f, 250.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true; // if the mouse is moved, this will be set to false, so that the camera can be moved with the mouse

//timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f; // time of last frame

//break and place
PlayerController playerController;
std::unique_ptr<World> world;
glm::vec3 hitBlockPos = glm::vec3(-1);
bool buttonPress = false;
bool place = false;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Hide and capture the cursor for first-person camera movement


    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
	glad_glFrontFace(GL_CW); // Set the front face to clockwise
	glEnable(GL_CULL_FACE); // Enable face culling
	glCullFace(GL_BACK); // Cull back faces (the default is GL_BACK, but it's good to be explicit)

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("shaders/6.3.coordinate_systems.vs", "shaders/6.3.coordinate_systems.fs");


    //make chunk
    //Chunk chunk = Chunk(glm::vec3(0.0f, 0.0f, 0.0f)); // Create a chunk at the origin
    //chunk.buildChunk();
    world = std::make_unique<World>();

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        
        //per-frame logic
		float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.6f, 0.9f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

        // activate shader
        ourShader.use();

        //set fog stuff
        ourShader.setVec4("FogColor", glm::vec4(0.6f, 0.9f, 1.0f, 1.0f));
        ourShader.setFloat("FogMaxDist", (renderDistance-0.5) * 32);
        ourShader.setFloat("FogMinDist", (renderDistance-3) * 32);
        ourShader.setVec3("CameraPos", camera.Position);


        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
        ourShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        ourShader.setMat4("model", model);

        //chunk.render(ourShader); // Render the chunk using the shader
        world->updateChunks(camera.Position);
		world->renderChunks(ourShader); // Render all chunks in the world

        playerController.RayCast(camera.Position, camera.Front, world, place, hitBlockPos, buttonPress);
        buttonPress = false;

		//render the hit block if it exists
        if (hitBlockPos != glm::vec3(-1)) { // Use a sentinel value for "no hit"
            // Set up a simple color shader for the outline, or reuse your existing shader
            ourShader.use();
            glm::mat4 outlineModel = glm::translate(glm::mat4(1.0f), hitBlockPos);
            outlineModel = glm::scale(outlineModel, glm::vec3(1.01f)); // Slightly larger to avoid z-fighting
            ourShader.setMat4("model", outlineModel);

            // Set a uniform for outline color if your shader supports it
            // ourShader.setVec4("OutlineColor", glm::vec4(1, 1, 0, 1)); // yellow

            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
            glDisable(GL_CULL_FACE); // So all edges are visible
            renderUnitCube(); // You need a function that draws a cube from (0,0,0) to (1,1,1)

            glEnable(GL_CULL_FACE);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Restore fill mode
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    //clean up resources

    //chunk.Delete();
	world->Delete(); // Clean up all chunks in the world

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void renderUnitCube() {
    static GLuint vao = 0, vbo = 0;
    if (vao == 0) {
        float vertices[] = {
            // ... 8 cube vertices ...
            -0.5f,-0.5f,-0.5f, 0.5f,-0.5f,-0.5f, 0.5f,0.5f,-0.5f, -0.5f,0.5f,-0.5f,
            -0.5f,-0.5f,0.5f, 0.5f,-0.5f,0.5f, 0.5f,0.5f,0.5f, -0.5f,0.5f,0.5f
        };
        unsigned int indices[] = {
            0,1, 1,2, 2,3, 3,0, // bottom
            4,5, 5,6, 6,7, 7,4, // top
            0,4, 1,5, 2,6, 3,7  // sides
        };
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        GLuint ebo;
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }
    glBindVertexArray(vao);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    int speed = 8;

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		speed *= 4; // Double the speed if left shift is pressed

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime * speed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime * speed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime * speed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime * speed);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        //break block so the bool place is false.
        place = false;
        buttonPress = true;
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        //place block so the bool place is true. 
        place = true;
		buttonPress = true;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
