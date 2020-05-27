//
// Created by unite on 12/05/2020.
//

#include <fstream>

#include <render_program.h>
#include <engine.h>
#include <iostream>
#include <GL/glew.h>
#include <sstream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <string_view>

template<typename T>
struct Vec2
{
    union
    {
        struct
        {
            T x; ///< X coordinate of the vector
            T y; ///< Y coordinate of the vector

        };
        T coord[2];
    };

    const static Vec2 zero;
    const static Vec2 one;
    const static Vec2 up;
    const static Vec2 down;
    const static Vec2 left;
    const static Vec2 right;

    //-----------------------------------------------------------------------------
    // Constructors
    //-----------------------------------------------------------------------------
    Vec2()
        : x(0), y(0)
    {
    }

    explicit Vec2(T same)
        : x(same), y(same)
    {
    }

    Vec2(T X, T Y) noexcept
        : x(X), y(Y)
    {
    }

    template<typename U>
    explicit Vec2(const Vec2<U>& vector)
        : x(static_cast<T>(vector.x)),
        y(static_cast<T>(vector.y))
    {
    }


    //-----------------------------------------------------------------------------
    // Operators
    //-----------------------------------------------------------------------------
    Vec2<T> operator+(const Vec2<T>& rhs) const
    {
        return Vec2<T>(x + rhs.x, y + rhs.y);
    }

    Vec2<T>& operator+=(const Vec2<T>& rhs)
    {
        this->x += rhs.x;
        this->y += rhs.y;
        return *this;
    }

    Vec2<T> operator-(const Vec2<T>& rhs) const
    {
        return Vec2<T>(x - rhs.x, y - rhs.y);
    }

    Vec2<T>& operator-=(const Vec2<T>& rhs)
    {
        this->x -= rhs.x;
        this->y -= rhs.y;
        return *this;
    }

    Vec2<T> operator*(T rhs) const
    {
        return Vec2<T>(x * rhs, y * rhs);
    }

    Vec2<T> operator*(const Vec2<T>& rhs) const
    {
        return Vec2<T>(x * rhs.x, y * rhs.y);
    }

    Vec2<T>& operator*=(T rhs)
    {
        this->x *= rhs;
        this->y *= rhs;
        return *this;
    }

    Vec2<T> operator/(T rhs) const
    {
        return (*this) * (1.0f / rhs);
    }

    Vec2<T>& operator/=(T rhs)
    {
        this->x /= rhs;
        this->y /= rhs;
        return *this;
    }

    bool operator==(const Vec2<T>& right)
    {
        return x == right.x && y == right.y;
    }

    bool operator!=(const Vec2<T>& right)
    {
        return !(*this == right);
    }

    const T& operator[](size_t p_axis) const
    {

        return coord[p_axis];
    }

    T& operator[](size_t p_axis)
    {

        return coord[p_axis];
    }
};

using Vec2f = Vec2<float>;
using TextureId = GLuint;

class Quad
{
public:
    void Init()
    {
        Vec2f vertices[4] = {
                Vec2f(0.5f, 0.5f) * size_ + Vec2f(offset_),  // top right
                Vec2f(0.5f, -0.5f) * size_ + Vec2f(offset_),  // bottom right
                Vec2f(-0.5f, -0.5f) * size_ + Vec2f(offset_),  // bottom left
                Vec2f(-0.5f, 0.5f) * size_ + Vec2f(offset_)   // top left
        };

        float texCoords[8] = {
                1.0f, 1.0f,      // top right
                1.0f, 0.0f,   // bottom right
                0.0f, 0.0f,   // bottom left
                0.0f, 1.0f,   // bottom left
        };

        unsigned int indices[6] = {
            // note that we start from 0!
            0, 1, 3,   // first triangle
            1, 2, 3    // second triangle
        };

        //Initialize the EBO program
        glGenBuffers(2, &VBO[0]);
        glGenBuffers(1, &EBO);
        glGenVertexArrays(1, &VAO);
        // 1. bind Vertex Array Object
        glBindVertexArray(VAO);
        // 2. copy our vertices array in a buffer for OpenGL to use
        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2f), (void*)0);
        glEnableVertexAttribArray(0);
        //bind texture coords data
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    }

    void Draw() const
    {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    void Destroy()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(2, &VBO[0]);
        glDeleteBuffers(2, &EBO);

    }
private:
    unsigned int VAO = 0;
    unsigned int EBO = 0;
    unsigned int VBO[2]{};
    Vec2f size_;
    Vec2f offset_;

    unsigned shaderProgram;
};
class TextureProgram : public RenderProgram
{
public:
    unsigned stbCreateTexture(const std::string filename)
    {
        int width, height, nrChannels;

        int reqComponents = 0;
        reqComponents = 4;

        unsigned char* data = stbi_load("data/12_hello_texture/texture.png", &width, &height, &nrChannels, 0);
        if (data == nullptr)
        {
            std::cout << "[Error] Texture: cannot load " << filename << "\n";
            return 0;
        }
        unsigned int texture;
        glGenTextures(1, &texture);

        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
        return texture;
    }

    void Init() override
    {
        quad_.Init();

        const auto texturePath = "data/12_hello_texture/texture.png";
        textureId_ = stbCreateTexture(texturePath);

        glEnable(GL_DEPTH_TEST);
    }
    void Destroy() override
    {
        quad_.Destroy();
        glDeleteTextures(1, &textureId_);
        textureId_ = 0;
    }

    void Update(seconds dt) override
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureId_);
        quad_.Draw();
    }
    void OnEvent(const SDL_Event& event) override
    {

    }

    void UpdateUi(seconds dt) override {

    }
private:
    Quad quad_;
    TextureId textureId_ = 0u;
};


int main(int argc, char** argv)
{
    Engine engine;
    TextureProgram renderProgram;
    engine.AddProgram(&renderProgram);
    engine.Init();
    engine.StartLoop();
    return EXIT_SUCCESS;
}