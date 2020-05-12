#pragma once

#include <chrono>
#include <SDL2/SDL_events.h>


class RenderProgram
{
public:
    using seconds = std::chrono::seconds;

    virtual void Init() = 0;
    virtual void Update(seconds dt) = 0;
    virtual void Destroy() = 0;

    virtual void OnEvent(const SDL_Event& event) = 0;
};