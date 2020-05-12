//
// Created by unite on 12/05/2020.
//
#include <iostream>
#include "engine.h"
#include "GL/glew.h"

void Engine::Init()
{
    SDL_Init(SDL_INIT_VIDEO);

#ifdef WIN32
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif
    SDL_GL_SetSwapInterval(1);
    auto flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
    window = SDL_CreateWindow(
            "Hello Comp Graph",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            windowWidth,
            windowHeight,
            flags
            );
    glRenderContext = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, glRenderContext);

    auto init_res = glewInit();
    if(init_res != GLEW_OK)
    {
        std::cerr << glewGetErrorString(glewInit()) << '\n';
    }
}
static std::chrono::time_point<std::chrono::system_clock> start;
void Engine::StartLoop()
{
    for(auto* program : programs)
    {
        program->Init();
    }
    start = std::chrono::system_clock::now();
    while(isRunning)
    {
        const auto current = std::chrono::system_clock::now();
        const auto dt = std::chrono::duration_cast<seconds>(current - start);
        start = current;

        Update(dt);
    }
    Destroy();
}

void Engine::OnEvent(const SDL_Event &event)
{
    if(event.type == SDL_EventType::SDL_QUIT)
    {
        isRunning = false;
    }
    for(auto* program : programs)
    {
        program->OnEvent(event);
    }
}

void Engine::Update(seconds dt)
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        OnEvent(event);
    }
    glClearColor(0.0f,0.0f,0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    for(auto* program : programs)
    {
        program->Update(dt);
    }
    SDL_GL_SwapWindow(window);
}

void Engine::AddProgram(RenderProgram *renderProgram)
{
    programs.push_back(renderProgram);
}

void Engine::Destroy()
{
    for(auto* program : programs)
    {
        program->Destroy();
    }
    SDL_GL_DeleteContext(glRenderContext);
    SDL_DestroyWindow(window);
}