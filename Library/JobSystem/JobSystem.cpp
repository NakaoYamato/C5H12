#include "JobSystem.h"
#include <imgui.h>

#include "../DebugSupporter/DebugSupporter.h"

void JobSystem::Initialize(size_t threadCount)
{
    stop = false; // 停止フラグをリセット
    numThreads = threadCount;
    for (size_t i = 0; i < numThreads; ++i)
    {
        workers.emplace_back(&JobSystem::WorkerThread, this, i);
    }
}

void JobSystem::Finalize()
{
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        stop = true;
    }

    condition.notify_all(); // 全スレッドを終了状態に

    for (std::thread& worker : workers)
    {
        if (worker.joinable())
        {
            worker.join();
        }
    }

    workers.clear(); // スレッドをクリア
}

void JobSystem::DrawGui()
{
    if (ImGui::Begin("JobSystem"))
    {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            ImGui::Text(u8"タスク数: %d", jobQueue.size());
        }
        ImGui::Text(u8"タスク総数: %d", jobSumCount.load());
		ImGui::Checkbox("UseMultiThread", &_useMultiThread);

        ProfileDrawUI();
    }
	ImGui::End();
}

// ジョブの計算を待機
void JobSystem::WaitCalculationJob()
{
    // すべてのジョブの終了を待機
 //   std::unique_lock<std::mutex> lock(completeJobMutex);
	//completeJobs.wait(lock, [this]() { return jobCount.load() == 0; });
    // すべてのジョブの終了を待機
    for (auto& result : jobResults)
    {
        result.get();
    }
}

void JobSystem::WorkerThread(size_t threadIndex)
{
	//Debug::Output::String(L"スレッド開始:");
	//Debug::Output::String((int)threadIndex + 1);
	//Debug::Output::String("\n");
    while (true)
    {
        Job job;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this]() { return stop || !jobQueue.empty(); });

            if (stop && jobQueue.empty())
                return;

            job = std::move(jobQueue.front());
            jobQueue.pop();
        }

        if (job.task)
        {
			//std::wstring str = L"タスク開始:" + std::to_wstring(threadIndex + 1) + L"\n";
   //         Debug::Output::String(str);
            ProfileScopedSection_3(static_cast<int>(threadIndex + 1), job.name, job.color);
            job.task();
            //str = L"\tタスク終了:" + std::to_wstring(threadIndex + 1) + L"\n";
            //Debug::Output::String(str);

			//jobCount.fetch_sub(1);
			//if (jobCount.load() == 0)
			//{
   //             std::unique_lock<std::mutex> lock(completeJobMutex);
			//	completeJobs.notify_all();
			//}
        }
    }
}
