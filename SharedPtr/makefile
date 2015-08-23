all:
	g++ -std=c++14 SharedPtr_test.cpp -o SharedPtr -pthread

warnings:
	g++ -Wall -Wextra -pedantic -g -std=c++14 SharedPtr_test.cpp -o SharedPtr -pthread

sample_warnings:
	g++ -Wall -Wextra -pedantic -g -std=c++14 SharedPtr_test_lite.cpp -o SharedPtr -pthread

sample_threads:
	g++ -Wall -Wextra -pedantic -g -std=c++14 SharedPtr_test_threads.cpp -o SharedPtr -pthread


clean:
	rm -f *.o *~ SharedPtr