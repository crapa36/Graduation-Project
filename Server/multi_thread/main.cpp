#include <thread>
#include <iostream>
#include <vector>
#include <chrono>
using namespace std;

void ThreadFunc(int threadid)
{
	cout << threadid << endl;
}

int main()
{
	vector <thread> my_thread;
	//시간측정
	auto t = chrono::high_resolution_clock::now();
	for (int i = 0; i < 10; ++i)
		my_thread.emplace_back(ThreadFunc, i);
	auto d = chrono::high_resolution_clock::now() - t;
	cout << chrono::duration_cast<chrono::milliseconds>(d).count() << " msecs\n";
	for (auto& t : my_thread) t.join();
}
