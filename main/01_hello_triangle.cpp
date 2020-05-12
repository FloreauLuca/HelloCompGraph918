//
// Created by unite on 12/05/2020.
//

#include <render_program.h>
#include <engine.h>

class HelloTriangleProgram : public RenderProgram
{
public:
    void Init() override
    {

    }

    void Update(seconds dt) override
    {

    }

    void Destroy() override
    {

    }

    void OnEvent(const SDL_Event &event) override
    {

    }

};

int main(int argc, char** argv)
{
    Engine engine;
    HelloTriangleProgram renderProgram;
    engine.AddProgram(&renderProgram);
    engine.Init();
    engine.StartLoop();
    return EXIT_SUCCESS;
}