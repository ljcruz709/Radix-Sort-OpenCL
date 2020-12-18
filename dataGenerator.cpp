#include <bits/stdc++.h>

std::ofstream cout("data.txt");

int main(int argc, char const *argv[])
{
	srand(time(NULL));

	int max = 10000;

	for (unsigned long long int i = 0; i < 1000000; ++i)
	{
		cout << rand() % max << "\n";
	}
	return 0;
}