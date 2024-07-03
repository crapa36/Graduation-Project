#include <thread>
#include <iostream>
#include <vector>
#include <chrono>
using namespace std;


volatile int sum{};

void ThreadFunc()
{
	for (auto i = 0; i < 25000000; ++i) {
		sum = sum + 2;
	}
}

int main()
{
	
	//시간측정
	auto t = chrono::high_resolution_clock::now();
	thread t1{ ThreadFunc };
	thread t2{ ThreadFunc };
	auto d = chrono::high_resolution_clock::now() - t;
	t1.join();
	t2.join();
	cout << "Sum = " << sum <<  " duration = ";
	cout << chrono::duration_cast<chrono::milliseconds>(d).count() << " msecs\n";
	
}
