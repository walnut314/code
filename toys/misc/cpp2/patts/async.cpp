/*++
Abstract:
    Base class for creating/managing a worker thread. Classes derived from this base class should
    implement the ThreadWork() method for work that should be executed by a dedicated worker thread.
    If StartWorkerThread() has been called, the derived class should call WaitForWorkerThreadToComplete()
    before exiting its destuctor to avoid potential access violations from accessing deallocated member data.
--*/

#include <thread>
#include <iostream>
#include <windows.h>

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
        m_TimeToStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    }
    
    virtual ~MyService(void) {
        printf("destroy...\n");
        if (m_TimeToStopEvent)
        {
            CloseHandle(m_TimeToStopEvent);
            m_TimeToStopEvent = NULL;
        }
    }

//protected:
    void ThreadWork(void) {
        Sleep(1000);
        printf("working...\n");
        if (m_TimeToStopEvent)
        {
            ::WaitForSingleObject(m_TimeToStopEvent, INFINITE);
        }    
        printf("done working...\n");
    }
    /*virtual*/ void OnStart() {
        printf("on start...\n");
        StartWorkerThread();
    }
    
    /*virtual*/ void OnStop() {
        printf("on stop...\n");
        ::SetEvent(m_TimeToStopEvent);
        WaitForWorkerThreadToComplete();
    }

private:
    char*   m_ServiceName;
    HANDLE  m_TimeToStopEvent;

};

int main()
{
    char name[] = "wusup";
    MyService service(name);
    service.OnStart();
    int counter = 8;
    while (1) {
        Sleep(1000);
        if (--counter < 0) {
            break;
        }
        printf("count: %d\n", counter);
    }
    service.OnStop();
    return 0;
}

