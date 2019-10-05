#pragma once

#include <thread>
#include <mutex>
#include <memory>
#include <vector>
using namespace std;

namespace Tool
{
    // 注意事项
    // 1，std::function可以绑定到全局函数或类的静态成员函数，如果要绑定到类的非静态成员函数，则需要使用std::bind。
	// 2，对于需要多个线程访问的变量，需要加互斥锁（可以利用unique_lock来简化操作，unique_lock只传一个mutex，会自动上锁，析构时会自动解锁）
	//explicit unique_lock(_Mutex& _Mtx)
	//	: _Pmtx(&_Mtx), _Owns(false)
	//{	// construct and lock
	//	_Pmtx->lock();
	//	_Owns = true;
	//}
    
    // 互斥变量
    template <typename T>
    class MutexVariable
    {
    public:
        MutexVariable() {}
        MutexVariable(T val)
            : m_oVariable(val)
        {}
        ~MutexVariable() {}
    
    public:
		// 赋值运算
		T &operator=(T val)
		{
			std::unique_lock<std::mutex> lock(m_oMutex);
			m_oVariable = val;
			return m_oVariable;
		}
		T getValue()
		{
			std::unique_lock<std::mutex> lock(m_oMutex);
			return m_oVariable;
		}
        // 操作变量
        void operateVariable(std::function<void(T&)> operateFunc = nullptr)
        {
			std::unique_lock<std::mutex> lock(m_oMutex);
			if (operateFunc)
			{
				operateFunc(m_oVariable);
			}
        }
        
    private:
        // 变量
        T m_oVariable;
        // 互斥锁
        std::mutex m_oMutex;
    };

    class BaseThread
    {
    public:
        // Task
        typedef std::function<void()> Task;
        // 休眠条件 std::condition_variable::wait(signal, SleepCondition) 不传默认返回false(休眠阻塞)  返回true时继续执行
        typedef std::function<bool()> SleepCondition;
        
    public:
        BaseThread(SleepCondition sleepCondition = nullptr, bool autoCreate = true);
        virtual ~BaseThread();
    
    public:
        // 添加任务
        void addTask(const Task& task);
        // 唤醒线程
        void weekupThreadOnce();
        // 结束线程
        void terminateThread();
        // 是否处于激活状态
        bool getIsActive();
        
		int m_customId;
    protected:
        // 弹出任务
        Task popTask();
        // 任务数量
        int getTaskCount();
        // 任务池是否为空
        bool isTasksEmpty();
        
    private:
        // 线程函数
        void innerThreadFunc();
        
    protected:
        // 线程指针
        std::thread* m_pThread;
		// 信号互斥变量
		std::mutex m_SignalMutex;
        // 条件变量
        std::condition_variable m_oSleepCondition;
        // 任务函数
        MutexVariable<std::vector<Task>> m_oTasks;
        // 休眠条件函数
        SleepCondition m_pSleepCondition;
        // 是否初始化
        bool m_bInited;
        // 是否激活
        MutexVariable<bool> m_oIsActive;
        // 终止信号
		MutexVariable<bool> m_bReadyTerminate;
        // 是否终止
		MutexVariable<bool> m_bIsTerminate;
        // 线程id
        std::thread::id m_uThreadId;
    };
}
