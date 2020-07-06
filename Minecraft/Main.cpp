#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "Header Files/Shaders.h"

#include "Header Files/stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



struct Player {
public:
    const float playerWidth = 10.0f;
    float playerVertices[6];
    float playerLocation = 0.0f;

    Player(float startLocation)
        : playerVertices{
        // positions       
         startLocation, -0.2f, 0.0f,
         startLocation, 0.2f, 0.0f,
        }
    {}
};

class PlayerRenderer {
private:
    unsigned int VAO;
    unsigned int VBO;
    unsigned int playerMovementUniform;
public:
    Player player;
    Shader playerShader;

    PlayerRenderer(Player player, Shader shader)
        : player(player), playerShader(shader)
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(player.playerVertices), player.playerVertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        playerMovementUniform = glGetUniformLocation(shader.ID, "movementTransform");
    }

    void RenderPlayer()
    {
        playerShader.use();

        glUniform1f(playerMovementUniform, player.playerLocation);
        glBindVertexArray(VAO);
        glLineWidth(player.playerWidth);
        glDrawArrays(GL_LINES, 0, 2);
    }
};


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, float& alpha);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// block 1
float positionBlock1 = 0.0f;
float positionBlock2 = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

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

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader ourShader("Shaders/Vertex.vert", "Shaders/Fragment.frag");
    Shader blockShader("Shaders/Block.vert", "Shaders/Fragment.frag");
    
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    const float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    unsigned int VBO, VAO, EBO, VBOBlock1, VBOBlock2, VAOBlock1, VAOBlock2;
    glGenVertexArrays(1, &VAO);
    glGenVertexArrays(1, &VAOBlock1);
    glGenVertexArrays(1, &VAOBlock2);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &VBOBlock1);
    glGenBuffers(1, &VBOBlock2);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int textures[2];
    glGenTextures(2, textures);
    glBindTexture(GL_TEXTURE_2D, textures[0]);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load and generate the texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load("Textures/wall.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, textures[1]);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // make the image flip when loaded
    stbi_set_flip_vertically_on_load(true);

    // load and generate the texture
    data = stbi_load("Textures/awesomeface.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // no longer needed to flip the images that will load
    stbi_set_flip_vertically_on_load(false);

    Player player1 = Player(-0.98f);

    Player player2 = Player(0.98f);

    PlayerRenderer renderer1 = PlayerRenderer(player1, blockShader);

    PlayerRenderer renderer2 = PlayerRenderer(player2, blockShader);

    const float playerVertices[] = {
        // positions       
         -0.98f, -0.2f, 0.0f,
         -0.98f, 0.2f, 0.0f,
    };

    glBindVertexArray(VAOBlock1);
    glBindBuffer(GL_ARRAY_BUFFER, VBOBlock1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(player1.playerVertices), player1.playerVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(VAOBlock2);

    glBindBuffer(GL_ARRAY_BUFFER, VBOBlock2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(player2.playerVertices), player2.playerVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //ourShader.use();

    ourShader.setInt("texture1", 0);
    ourShader.setInt("texture2", 1);

    float alpha = 0.2f;

    unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
    unsigned int blockMovementLoc = glGetUniformLocation(blockShader.ID, "movementTransform");

    float translationOffset = 0.5f;

    bool rightDirection = false;
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window, alpha);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures[1]);

        ourShader.use();

        ourShader.setFloat("alpha", alpha);

        glm::mat4 trans = glm::mat4(1.0f);


        if (rightDirection)
        {
            translationOffset += 0.01f;
        }
        else
        {
            translationOffset -= 0.01f;
        }

        if (translationOffset <= -0.5f)
        {
            rightDirection = true;
        }
        else if (translationOffset >= 0.5f)
        {
            rightDirection = false;
        }

        
        trans = glm::translate(trans, glm::vec3(translationOffset, 0.0f, 0.0f));

        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

        ourShader.use();
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
        
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


        renderer1.RenderPlayer();
        renderer2.RenderPlayer();
        

        //glUniform1f(blockMovementLoc, positionBlock1);
        //glBindVertexArray(VAOBlock1);
        //glLineWidth(10.0f);
        //glDrawArrays(GL_LINES, 0, 2);

        //glUniform1f(blockMovementLoc, positionBlock2);
        //glBindVertexArray(VAOBlock2);
        //glLineWidth(10.0f);
        //glDrawArrays(GL_LINES, 0, 2);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window, float& alpha)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE)== GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        alpha += 0.01f;
        positionBlock1 += 0.01f;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        alpha -= 0.01f;
        positionBlock1 -= 0.01f;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        positionBlock2 += 0.01f;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        positionBlock2 -= 0.01f;
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




