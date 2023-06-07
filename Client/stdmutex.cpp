#include <iostream>
#include <thread>
#include <mutex>

std::mutex mtx;
int number = 0;

void ThreadIncrement(const std::string& thread_name)
{
    for (int i = 0; i < 5; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        std::lock_guard<std::mutex> lock(mtx);
        ++number;
        std::cout << "실행 : " << thread_name << ", number : " << number << std::endl;
    }
}

int main()
{
    std::string thread1 = "A Thread";
    std::string thread2 = "B Thread";

    std::thread t1(ThreadIncrement, thread1);
    std::thread t2(ThreadIncrement, thread2);

    t1.join();
    t2.join();

    std::cout << "최종 number : " << number << std::endl;

    return 0;
}
