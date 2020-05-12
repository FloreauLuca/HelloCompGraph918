#pragma once

#include <vector>

#include "SDL.h"
#include "SDL_main.h"
#include "render_program.h"

class Engine
{
public:
    using seconds = std::chrono::seconds;
    void Init();
    void StartLoop();
    void AddProgram(RenderProgram* renderProgram);
private:

    void Update(seconds dt);
    void Destroy();
    void OnEvent(const SDL_Event& event);
    SDL_Window * window = nullptr;
    std::vector<RenderProgram*> programs;
    int windowWidth = 1280;
    int windowHeight = 720;
    bool isRunning = true;
    SDL_GLContext glRenderContext;
};
