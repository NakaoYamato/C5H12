#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <string>
#include <atomic>
#include "../ImGui/ImGuiProfiler.h"

class JobSystem
{
private:
    JobSystem() {}
    ~JobSystem() {}

    // コピー禁止
    JobSystem(const JobSystem&) = delete;
    JobSystem(JobSystem&&) noexcept = delete;
    JobSystem& operator=(const JobSystem&) = delete;
    JobSystem& operator=(const JobSystem&&) noexcept = delete;
public:
    // シングルトンのインスタンス取得
    static JobSystem& Instance() {
        static JobSystem instance;
        return instance;
    }

    // 初期化（スレッドプールを作成）
    void Initialize(size_t threadCount = std::thread::hardware_concurrency());

    // 終了処理（スレッドプールを停止）
    void Finalize();

    void DrawGui();

    //スレッドの数を取得
    const size_t GetNumThreads()const { return numThreads; }

    // ジョブを追加
    template<typename Func, typename... Args>
    auto EnqueueJob(Func&& func, Args&&... args) -> std::future<typename std::invoke_result<Func, Args...>::type>;

    // ジョブを追加 デバッグでタスクの処理時間を確認するよう
    template<typename Func, typename... Args>
    auto EnqueueJob(const char* TaskName, ImGuiControl::Profiler::Color uiColor, Func&& func, Args&&... args)
        -> std::future<typename std::invoke_result<Func, Args...>::type>;

    // ジョブの計算を待機
    void WaitCalculationJob();

    std::vector<std::future<void>> jobResults;

#pragma region アクセサ
	bool UseMultiThread() const { return _useMultiThread; }
#pragma endregion


private:
    // 内部的な処理
    void WorkerThread(size_t threadIndex);

    struct Job
    {
        std::function<void()> task;
        const char* name = "No Name";                                         //タスクネーム
        ImGuiControl::Profiler::Color color = ImGuiControl::Profiler::Dark;    //GUIで表示する際の色
    };

private:
    std::vector<std::thread> workers;                // スレッドプール
    std::queue<Job> jobQueue;                        // ジョブキュー
    std::mutex queueMutex;                           // ジョブキューの保護
    std::condition_variable condition;               // ジョブキューの通知
    bool stop = false;                               // 停止フラグ
    size_t numThreads = {};                          // スレッドの数  

    //std::mutex completeJobMutex;                     //
    //std::atomic<int> jobCount = 0;                     // ジョブの数
    //std::condition_variable completeJobs;            //

    std::atomic<int> jobSumCount;

#pragma region デバッグ用
	bool _drawGui = false; // GUIを描画するか
    bool _useMultiThread = false;
#pragma endregion

};

template<typename Func, typename ...Args>
inline auto JobSystem::EnqueueJob(Func&& func, Args && ...args) -> std::future<typename std::invoke_result<Func, Args ...>::type>
{
    return JobSystem::EnqueueJob("Worker", ImGuiControl::Profiler::Dark, func, args);
}

// デバッグ用
template<typename Func, typename ...Args>
inline auto JobSystem::EnqueueJob(const char* TaskName, ImGuiControl::Profiler::Color uiColor, Func&& func, Args&&... args)
-> std::future<typename std::invoke_result<Func, Args...>::type>
{
    using ReturnType = typename std::invoke_result<Func, Args...>::type;

    // パッケージ化されたジョブを作成
    auto task = std::make_shared<std::packaged_task<ReturnType()>>(
        std::bind(std::forward<Func>(func), std::forward<Args>(args)...)
    );

    std::future<ReturnType> result = task->get_future();

    {
        std::lock_guard<std::mutex> lock(queueMutex);
        jobQueue.emplace(Job{ [task]() { (*task)(); }, TaskName, uiColor });
    }

    condition.notify_one();

	jobSumCount.fetch_add(1);
    //jobCount.fetch_add(1);

    return result;
}
