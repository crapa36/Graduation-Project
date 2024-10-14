#pragma once
#include <chrono>

class Timer {
    DECLARE_SINGLETON(Timer);
public:
    void Init();
    void Update();
    float GetDeltaTime() const { return _deltaTime; }
    float GetElapsedTime() const { return _elapsedTime; }
    float GetTotalTime() const { return _totalTime; }
    int32_t GetFPS() const { return _fps; }

private:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    
    TimePoint _firstTime;
    TimePoint _prevTime;
    float _deltaTime=0.0f;
    int32_t _frameCount=0;
    int32_t _fps=0;
    float _elapsedTime=0.0;
    float _totalTime = 0.0;
};