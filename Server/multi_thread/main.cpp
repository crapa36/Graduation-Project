#include <thread>
#include <iostream>
#include <vector>
#include <chrono>
using namespace std;

void ThreadFunc()
{
	
}

volatile  int sum{};

int main()
{
	vector <thread> my_thread;
	//시간측정
	auto t = chrono::high_resolution_clock::now();
	for (auto i = 0; i < 50000000; ++i) sum = sum + 2;
	auto d = chrono::high_resolution_clock::now() - t;
	cout << "Sum = " << sum <<  " duration = ";
	cout << chrono::duration_cast<chrono::milliseconds>(d).count() << " msecs\n";
	for (auto& t : my_thread) t.join();
}
