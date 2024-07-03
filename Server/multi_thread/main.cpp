#include <thread>
#include <iostream>
#include <vector>
#include <chrono>
using namespace std;
const auto  MAX_THREADS = 64;

volatile int sum{};

void ThreadFunc(int num_of_thread)
{
	for (auto i = 0; i < 50000000 / num_of_thread; ++i) {
		sum = sum + 2;
	}
}

int main()
{
	vector<thread> threads;
	//�ð�����
	for (auto i = 1; i <= MAX_THREADS; i *= 2) {
		sum = 0;
		threads.clear();
		auto start = chrono::high_resolution_clock::now();
		for (auto j = 0; j < i; ++j) threads.push_back(thread{ ThreadFunc, i });
		for (auto& tmp : threads) tmp.join();
		auto duration = chrono::high_resolution_clock::now() - start;
		cout << i << " Threads,   Sum = " << sum;
		cout << "  Duration = " << chrono::duration_cast<chrono::milliseconds>(duration).count() << " milliseconds\n";
	}
	
}
