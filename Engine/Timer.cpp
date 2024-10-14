#include "pch.h"
#include "Timer.h"

void Timer::Init(){
    
    _prevTime = Clock::now();
    _firstTime = Clock::now();
}

void Timer::Update() {
    TimePoint currentTime = Clock::now();
    std::chrono::duration<float> elapsedTime = currentTime - _prevTime;
    std::chrono::duration<float> totalTime = currentTime - _firstTime;
    _deltaTime = elapsedTime.count();
    _totalTime = totalTime.count();
    _prevTime = currentTime;

    _frameCount++;
    _elapsedTime += _deltaTime;

    if (_elapsedTime >= 1.0f) {
        _fps = _frameCount;
        _frameCount = 0;
        _elapsedTime = 0.0f;
    }
}