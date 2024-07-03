#include <thread>
#include <iostream>
#include <vector>
using namespace std;

void ThreadFunc(int threadid)
{
	cout << threadid << endl;
}

int main()
{
	vector <thread> my_thread;
	for (int i = 0; i < 10; ++i)
		my_thread.emplace_back(ThreadFunc, i);
	for (auto& t : my_thread) t.join();
}
