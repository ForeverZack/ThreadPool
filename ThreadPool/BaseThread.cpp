#include "BaseThread.h"
#include <iostream>

namespace Tool
{
    
    BaseThread::BaseThread(SleepCondition sleepCondition/* = nullptr*/, bool autoCreate/* = true*/)
        : m_pThread(nullptr)
        , m_bInited(false)
        , m_pSleepCondition(sleepCondition)
        , m_oIsActive(false)
		, m_bIsTerminate(false)
		, m_bReadyTerminate(false)
		, m_customId(-1)
    {
        if (autoCreate)
        {
            m_pThread = new std::thread(&BaseThread::innerThreadFunc, this); //new std::thread(&BaseThread::innerThreadFunc, this);
            //m_pThread->detach();    // detach之后线程变为"非joinable"
            m_uThreadId = m_pThread->get_id();
        }

    }
    
    BaseThread::~BaseThread()
    {
        std::cout<<"=============~BaseThread======start======"<<m_customId<<endl;
		if (m_customId == -1)
		{
			int iii=0;
		}
        terminateThread();
		std::cout << "=============~BaseThread======end======" << m_customId << endl;
	}
    
    void BaseThread::weekupThreadOnce()
    {
		std::unique_lock<std::mutex> signal(m_SignalMutex);
		m_oIsActive = true;
        m_oSleepCondition.notify_one();
    }

    void BaseThread::terminateThread()
    {
         // 去销毁一个仍然可以“joinable”的C++线程对象会被认为是一种错误。为了销毁一个C++线程对象，约么join()函数需要被调用（并结束），要么detach()函数被调用。如果一个C++线程对象当销毁时仍然可以被join，异常会被抛出。
        m_bReadyTerminate = true;
        // 等待线程执行完成
        while(getIsActive());
        if (!m_bIsTerminate.getValue())
        {
            m_oTasks.operateVariable([](std::vector<Task>& tasks)->void
            {
                tasks.clear();
            });
            weekupThreadOnce();
        }
		m_pThread->join();
		delete m_pThread;
	}
    
    bool BaseThread::getIsActive()
    {
        return m_oIsActive.getValue();
    }
    
    void BaseThread::addTask(const Task& task)
    {
        m_oTasks.operateVariable([=](std::vector<Task>& tasks)->void
        {
            tasks.push_back(task);
        });
        weekupThreadOnce();
    }
    
    BaseThread::Task BaseThread::popTask()
    {
        Task task = nullptr;
        m_oTasks.operateVariable([&](std::vector<Task>& tasks)->void
        {
            if (tasks.size() > 0)
            {
                task = tasks[0];
                tasks.erase(tasks.begin());
            }
        });
        return task;
    }
    
    int BaseThread::getTaskCount()
    {
        int size = 0;
        m_oTasks.operateVariable([&](const std::vector<Task>& tasks)->void
        {
            size = tasks.size();
        });
        return size;
    }
    
    bool BaseThread::isTasksEmpty()
    {
        bool isEmpty;
        m_oTasks.operateVariable([&](const std::vector<Task>& tasks) -> void
        {
            isEmpty = tasks.empty();
        });
        return isEmpty;
    }
    
    void BaseThread::innerThreadFunc()
    {
        Task task = nullptr;

        while(true)
        {
            if (!m_bInited)
            {
                // 初始化后，不执行回调
                m_bInited = true;
				m_oIsActive = !isTasksEmpty();
				if (!m_oIsActive.getValue())
				{
					std::unique_lock<std::mutex> signal(m_SignalMutex);
					m_oSleepCondition.wait(signal);// 默认返回false
				}
            }
            
            task = popTask();
            if (task)
            {
                std::cout<<"===execute task===="<<endl;
                task();
            }
            
            
            if (m_bReadyTerminate.getValue())
            {
                // 终止
                break;
            }
            // 执行完一次，判断有没有需要继续执行的任务
			m_oIsActive = !isTasksEmpty();
			if (!m_oIsActive.getValue())
			{
				std::unique_lock<std::mutex> signal(m_SignalMutex);
				m_oSleepCondition.wait(signal); // 默认返回false
			}
        }

		m_oIsActive = false;
		m_bIsTerminate = true;
    }
    
    
    
    
}
