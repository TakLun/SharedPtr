#include "SharedPtr.hpp"
#include <iostream>
#include <cassert>
#include <algorithm>
#include <time.h>

using namespace cs540;


class Random {
public:
  Random(unsigned int s = 0);
  // Generate random int in range [l,h].                                          
  int operator()(int l, int h) {
    return std::uniform_int_distribution<int>(l, h)(gen);
  }
  Random(const Random &) = delete;
  Random &operator=(const Random &) = delete;
private:
  std::default_random_engine gen;
};

Random::Random(unsigned int seed) : gen(seed) {}




time_t StartTime;
const int TABLE_SIZE = 100;


class TestObj {
public:
  TestObj() {}
  TestObj(int i) : a(i) {}
  int a;
};

struct TableEntry {
  TableEntry();
  ~TableEntry();
  pthread_mutex_t lock;
  SharedPtr<TestObj> *ptr;
} *Table;

TableEntry::TableEntry() : ptr(0) {
  int ec;
  ec = pthread_mutex_init(&lock, 0);
  assert(ec == 0);
}
TableEntry::~TableEntry() {
  int ec;
  ec = pthread_mutex_destroy(&lock);
  assert(ec == 0);
  if (ptr != 0) {
    delete ptr;
    ptr = 0;
  }
}


int main(){

  Table = new TableEntry[TABLE_SIZE];

  int RunSecs = 15;
  StartTime = time(NULL);
  printf("Running threaded test for %d seconds.\n", RunSecs);

  unsigned long vp = 3;
  Random rand((unsigned long) vp);

  pthread_t tid = pthread_self();
  int ec;
  struct Counters {
    unsigned long assignment_new, assignment, reset, new_default, new_copy, delet;
  } counters = {0, 0, 0, 0, 0, 0};



  while (time(NULL) < StartTime + RunSecs) {
    
    if (rand(0, 1) == 0) {
      int i = rand(0, TABLE_SIZE - 1);
      ec = pthread_mutex_lock(&Table[i].lock); assert(ec == 0);
      if (!Table[i].ptr) {
	Table[i].ptr = new SharedPtr<TestObj>;
	counters.new_default++;
      }
      ec = pthread_mutex_unlock(&Table[i].lock); assert(ec == 0);
      // Create with copy constructor.                                    
    } else {
      int i = rand(0, TABLE_SIZE - 1), j;
      do {
	j = rand(0, TABLE_SIZE - 1);
      } while (i == j);
      
      // Order to avoid deadlock.                                     
      if (i < j) {
	ec = pthread_mutex_lock(&Table[i].lock); assert(ec == 0);
	ec = pthread_mutex_lock(&Table[j].lock); assert(ec == 0);
      } else {
	ec = pthread_mutex_lock(&Table[j].lock); assert(ec == 0);
	ec = pthread_mutex_lock(&Table[i].lock); assert(ec == 0);
      }
      
      if (!Table[i].ptr && Table[j].ptr) {
	Table[i].ptr = new SharedPtr<TestObj>(*Table[j].ptr);
	counters.new_copy++;
      }
      
      ec = pthread_mutex_unlock(&Table[i].lock); assert(ec == 0);
      ec = pthread_mutex_unlock(&Table[j].lock); assert(ec == 0);
      printf("%d: assign %d=%d done\n", (int) tid, i, j);
     }
  }



  double rs = RunSecs;
  printf("%10lu: assignment_new=%lu ops, assignment=%lu ops, reset=%lu ops, new_default=%lu ops, new_copy=%lu ops, delete=%lu ops\n"
     "    rates: assignment_new=%.0f ops/sec, assignment=%.0f ops/sec, reset=%.0f ops/sec, new_default=%.0f ops/sec, new_copy=%.0f ops/sec, delete=%.0f ops/sec\n",
	 (unsigned long) tid,
	 counters.assignment_new, counters.assignment, counters.reset, counters.new_default, counters.new_copy, counters.delet, counters.assignment_new/rs, counters.assignment/rs, counters.reset/rs, counters.new_default/rs, counters.new_copy/rs, counters.delet/rs);


  delete [] Table;
}
