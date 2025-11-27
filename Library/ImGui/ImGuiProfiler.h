#pragma once

#include "../Math/HighPrecisionTimer.h"
#include <vector>

#define ProfileInitialize(isPaused, commandHandler,maxThreads) ImGuiControl::globalInstance.Initialize(isPaused, commandHandler,maxThreads)
#define ProfileShutdown()                           ImGuiControl::globalInstance.Shutdown()
#define ProfileNewFrame()                           ImGuiControl::globalInstance.NewFrame()
#define ProfileDrawUI()                             ImGuiControl::globalInstance.DrawUI()

#define ProfilePushSection_1(x)     ImGuiControl::globalInstance.PushSectionInternal( #x, 0x00000000, __FILE__, __LINE__ )
#define ProfilePushSection_2(x,y)   ImGuiControl::globalInstance.PushSectionInternal( #x, y, __FILE__, __LINE__ )
#define ProfilePushSection_X(x,y,z,...) z

#define ProfilePushSection_FUNC_RECOMPOSER(argsWithParentheses) ProfilePushSection_X argsWithParentheses
#define ProfilePushSection_CHOOSE_FROM_ARG_COUNT(...) ProfilePushSection_FUNC_RECOMPOSER((__VA_ARGS__, ProfilePushSection_2, ProfilePushSection_1, ))
#define ProfilePushSection_MACRO_CHOOSER(...) ProfilePushSection_CHOOSE_FROM_ARG_COUNT(__VA_ARGS__ ())
#define ProfilePushSection(...) ProfilePushSection_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

#define ProfilePopSection() gProf.PopSectionInternal()

#define ProfileScopedSection_1(th, x)   ImGuiControl::ProfileScope S##x__LINE__(th, #x, 0x00000000, __FILE__, __LINE__ )
#define ProfileScopedSection_2(th, x,y) ImGuiControl::ProfileScope S##x__LINE__(th, #x, y, __FILE__, __LINE__ )
//#define ProfileScopedSection_3(th, x,y) ImGuiControl::ProfileScope(th, const char* x,ImGuiControl::Profiler::Color y, __FILE__, __LINE__ )
#define ProfileScopedSection_3(th, name, color) ImGuiControl::ProfileScope S##__LINE__(th, name, color, __FILE__, __LINE__)

#define ProfileScopedSection_X(x,y,z,...) z
#define ProfileScopedSection_FUNC_RECOMPOSER(argsWithParentheses) ProfileScopedSection_X argsWithParentheses
#define ProfileScopedSection_CHOOSE_FROM_ARG_COUNT(...) ProfileScopedSection_FUNC_RECOMPOSER((__VA_ARGS__, ProfileScopedSection_2, ProfileScopedSection_1, ))
#define ProfileScopedSection_MACRO_CHOOSER(...) ProfileScopedSection_CHOOSE_FROM_ARG_COUNT(__VA_ARGS__ ())
#define ProfileScopedSection(...) ProfileScopedSection_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

#define ProfileFunction0(th)      ImGuiControl::ProfileScope S##__LINE__(th, __FUNCTION__, 0x00000000, __FILE__, __LINE__ )
#define ProfileFunction1(th, x)     ImGuiControl::ProfileScope S##__LINE__(th, __FUNCTION__, x, __FILE__, __LINE__ )
#define ProfileFunction2(th, x, y)  ImGuiControl::ProfileScope S##__LINE__(th, x, y, __FILE__, __LINE__ )

#define ProfileThreadName(th, thread_name) ImGuiControl::globalInstance._threads[th].name = thread_name

namespace ImGuiControl
{
    /// <summary>
    /// ImGuiで各スレッドの処理時間を可視化するクラス
    /// </summary>
    class Profiler
    {
    public:
        static constexpr int DefaultMaxThreads = 9; // デフォルトの固定スレッド数
        int MaxThreads = DefaultMaxThreads;         // 実行時に設定可能な値

        static const int MaxFrames = 600;
        static const int MaxSections = 2000;

        enum Color : unsigned int
        {
            Yellow = 0xFF3594B7,
            Red = 0xFF3945D5,
            Blue = 0xFFC55344,
            Green = 0xFF4C8F00,
            Purple = 0xFF8B377A,
            Dark = 0xFF222222,
        };

        void Initialize(bool* isPaused, void (*setPause)(bool), int maxThreads);
        void Shutdown();
        void NewFrame();
        void PushSectionInternal(int threadIndex, const char* sectionName, unsigned int color, const char* fileName, int line);
        void PopSectionInternal(int threadIndex);

        void DrawUI();
        inline void LockCriticalSection2() {}
        inline void UnLockCriticalSection2() {}

        inline bool IsPaused() { return _isPausedPtr != nullptr && *_isPausedPtr; }
        inline void SetPause(bool value) { if (_setPause != nullptr) { _setPause(value); } }

        struct Section
        {
            const char* name;
            const char* fileName;
            int             line;
            unsigned int    color;
            int             callStackDepth;
            double          startTime;
            double          endTime;
            int             parentSectionIndex;
        };

        struct Frame
        {
            int         index       = 0;
            double      startTime   = 0.0;
            double      endTime     = 0.0;
        };

        struct ThreadInfo
        {
            double     minTime          = 0.0;
            double     maxTime          = 0.0;
            int       maxCallStackDepth = 0;
        };

        struct Thread
        {
            const char* name = nullptr;
            bool        initialized     = false;
            int         callStackDepth  = 0;
            int         sectionsCount   = 0;
            int         sectionIndex    = 0;
            Section     sections[MaxSections] = {};
            int         activeSectionIndex  = 0;
            ThreadInfo	threadInfo = {};
        };

        HighPrecisionTimer      _timer;
        Frame                   _frames[MaxFrames] = {};
        int                     _frameCount = 0;
        int                     _frameIndex = 0;
        std::vector<Thread>      _threads;
        bool*                   _isPausedPtr = nullptr;
        void                    (*_setPause)(bool) = nullptr;

        // Ui
        double      _timeOffset = 0.0;
        double      _timeDuration = 0.0;
        bool        _isWindowOpen = false;
        float       _frameAreaMaxDuration = 0.0f;
        int         _frameSelectionStart = 0;
        int         _frameSelectionEnd = 0;
        double      _sectionAreaDurationWhenZoomStarted = 0.0;

    private:
        void RefreshFrameSelection(double recordsMaxTime);
    };

    extern Profiler globalInstance;

    struct ProfileScope
    {
        ProfileScope(int threadIndex, const char* name, unsigned int color, const char* fileName, int line, Profiler* target = nullptr)
            : _threadIndex(threadIndex)
        {
			if (target == nullptr)
                _target = &globalInstance;
            else
				_target = target;

            if (_target)
                _target->PushSectionInternal(_threadIndex, name, color, fileName, line);
        }

        ~ProfileScope()
        {
            if (_target)
                _target->PopSectionInternal(_threadIndex);
        }
    private:
        Profiler* _target = nullptr;
        int _threadIndex;
    };
}
