//
//  main.cpp
//  ThreaTest
//
//  Created by 祝萌 on 2019/9/20.
//  Copyright © 2019 祝萌. All rights reserved.
//

#include <iostream>

//#include "BaseThread.h"

//int aaaa = 0;
//void counterFunc()
//{
//    ++aaaa;
//    std::cout<<aaaa<<endl;
//}
//
//int main(int argc, const char * argv[]) {
//    // insert code here...
//
////    Tool::BaseThread thread([]()->bool{
////        return true;
////    });
//    Tool::BaseThread thread;
//    thread.setCallback(&counterFunc);
////    thread.weekupThreadOnce();
//
//    while(aaaa<10)
//    {
//        if (thread.getIsActive())
//        {
//            std::cout<<"thread is busy"<<endl;
//        }
//        else
//        {
//            thread.weekupThreadOnce();
//        }
//
//    }
//    std::cout << "Hello, World!\n";
//
//    return 0;
//}



#include "BaseThreadPool.h"
#include <mutex>
int aaaa = 0;
std::mutex mut;
void counterFunc(int a)
{
    mut.lock();
    std::cout<<a<<endl;
    mut.unlock();
}

int main(int argc, const char * argv[]) {
    // insert code here...
    Tool::BaseThreadPool* threadPool = new Tool::BaseThreadPool(50);
    
    
    int ccc = 0;
    while(ccc < 10209)
    {
        {
            threadPool->addTask(std::bind(&counterFunc, ccc));
        }
        ++ccc;
    }
    
//    delete threadPool;
    
    std::cout << "Hello, World!\n";
    while(threadPool->getIsActive());
    
    threadPool->test();
    delete threadPool;
    
    std::cout << "Hello, World! eee\n";
    
    

    return 0;
}
