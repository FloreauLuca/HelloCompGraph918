#pragma once

#include <chrono>
#include <SDL2/SDL_events.h>


class RenderProgram
{
public:
    using seconds = std::chrono::duration<float, std::ratio<1>>;

    virtual void Init() = 0;
    virtual void Update(seconds dt) = 0;
    virtual void Destroy() = 0;
    virtual void UpdateUi(seconds dt) = 0;
    virtual void OnEvent(const SDL_Event& event) = 0;
};