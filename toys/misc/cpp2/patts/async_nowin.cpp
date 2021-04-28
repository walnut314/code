/*++
Abstract:
    Base class for creating/managing a worker thread. Classes derived from this base class should
    implement the ThreadWork() method for work that should be executed by a dedicated worker thread.
    If StartWorkerThread() has been called, the derived class should call WaitForWorkerThreadToComplete()
    before exiting its destuctor to avoid potential access violations from accessing deallocated member data.
--*/

#include <thread>
#include <iostream>
#include <condition_variable>
#include <mutex>

class CAsyncWorker
{
public:
    CAsyncWorker() {}

    virtual ~CAsyncWorker() {
        //Derived class should signal its work to terminate and call WaitForThreadWorkToComplete().
        if (m_WorkerThread.joinable())
        {
            //assert(FALSE);
        }
    }

    // This method creates a new thread to execute work defined in ThreadWork().
    void StartWorkerThread() {
        m_WorkerThread = std::thread([this] { WorkerThread(); });
    }

protected:

    virtual void ThreadWork() = 0;

    // This method waits for the worker thread to complete.
    void WaitForWorkerThreadToComplete() {
        if (m_WorkerThread.joinable()) {
            m_WorkerThread.join();
        }
    }

private:

    void WorkerThread() {
        ThreadWork();
    }

    std::thread m_WorkerThread;
};

class MyService : public CAsyncWorker
{
public:

    MyService(char* pszServiceName) {
        m_ServiceName = pszServiceName;
        m_TimeToStopEvent = false;
    }
    
    virtual ~MyService(void) {
        printf("destroy...\n");
        if (m_TimeToStopEvent)
        {
            m_TimeToStopEvent = false;
        }
    }

//protected:
    void ThreadWork(void) {
        std::this_thread::sleep_for (std::chrono::seconds(1));
        printf("working...\n");
        if (m_TimeToStopEvent)
        {
            std::unique_lock<std::mutex> lk(mut);
            data_cond.wait(lk);
        }    
        printf("done working...\n");
    }
    /*virtual*/ void OnStart() {
        printf("on start...\n");
        StartWorkerThread();
    }
    
    /*virtual*/ void OnStop() {
        printf("on stop...\n");
        m_TimeToStopEvent = true;
        data_cond.notify_one();
        WaitForWorkerThreadToComplete();
    }

private:
    char*   m_ServiceName;
    bool m_TimeToStopEvent = false;
    std::mutex mut;
    std::condition_variable data_cond;

};

int main()
{
    char name[] = "wusup";
    MyService service(name);
    service.OnStart();
    int counter = 8;
    while (1) {
        std::this_thread::sleep_for (std::chrono::seconds(1));        
        if (--counter < 0) {
            break;
        }
        printf("count: %d\n", counter);
    }
    service.OnStop();
    return 0;
}

