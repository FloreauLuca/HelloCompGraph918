#pragma once

#include <vector>

#include "SDL.h"
#include "SDL_main.h"
#include "render_program.h"

class Engine
{
public:
    using seconds = std::chrono::duration<float, std::ratio<1>>;
    void Init();
    void StartLoop();
    void AddProgram(RenderProgram* renderProgram);
private:

    void Update(seconds dt);
    void UpdateUi(seconds dt);
    void Destroy();
    void OnEvent(const SDL_Event& event);
    SDL_Window * window = nullptr;
    std::vector<RenderProgram*> programs;
    int windowWidth = 800;
    int windowHeight = 800;
    bool isRunning = true;
    SDL_GLContext glRenderContext;
};
