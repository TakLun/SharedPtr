
#include "SharedPtr.hpp"
#include <iostream>
#include <cassert>
#include <pthread.h>

using namespace cs540;

class Base{
public:
  int base_val;
public:
  Base() :
    base_val(30)
  {}

  Base(int v) :
    base_val(v)
  {}

  int &operator*(){ return base_val; }

};

std::ostream &operator<<(std::ostream &stream, Base b){
  stream << b.base_val;
  return stream;
}


class Derived : public Base{
public:
  int  derived_val1;
  char derived_val2;

public:
  Derived() : 
    derived_val1(1),
    derived_val2('A')
  {}

  Derived(int v, char c) : 
    derived_val1(v),
    derived_val2(c)
  {}
};

std::ostream &operator<<(std::ostream &stream, Derived d){
  stream << "\n\tderived's base val: " << d.Base::base_val << "\n\tderived val 1: " << d.derived_val1 << "\n\tderived val 2: " << d.derived_val2;
  return stream;
}

int main(){
  printf("Constructors:\n");
  {
    SharedPtr<Base> b1;
    SharedPtr<Derived> d1(new Derived());
    SharedPtr<Base> b_to_d(new Derived);
    
    (nullptr == b1) ? std::cout << "b1 is null\n" : std::cout << "b1: " << b1->base_val << std::endl;
    
    std::cout << "d1: " << *d1 << std::endl;
    std::cout << "b_to_d: " << *b_to_d << std::endl;
  }

  printf("\n\n");

  // move base to base
  printf("Move Assignments:\n");
  {
    SharedPtr<Base> mvb1(new Base(20));
    SharedPtr<Base> mvb2(new Base(800));
    std::cout << "mvb1 befor move: " << *mvb1 << std::endl;
    std::cout << "mvb2 befor move: " << *mvb2 << std::endl;
    mvb1 = std::move(mvb2);
    std::cout << "mvb1 after move: " << *mvb1 << std::endl;
    //assert(mvb2 == nullptr);
    (mvb2 == nullptr) ? std::cout<< "mvb2 is null\n" : std::cout << "mvb2 after move: " << *mvb2 << std::endl;
    
    printf("\n\n");
    
    // move derived to base
    SharedPtr<Base> mvb_d1(new Base(300));
    SharedPtr<Derived> mvb_d2(new Derived(10, 'E'));
    std::cout << "mvb_d1 befor move: " << *mvb_d1 << std::endl;
    std::cout << "mvb_d2 befor move: " << *mvb_d2 << std::endl;
    mvb_d1 = std::move(mvb_d2);
    std::cout << "mvb_d1 after move: " << *mvb_d1 << std::endl;
    //assert(mvb2 == nullptr);
    (mvb_d2 == nullptr) ? std::cout<< "mvb_d2 is null\n" : std::cout << "mvb_d2 after move: " << *mvb_d2 << std::endl;
  }

  printf("\n\n");

  // move ctor
  printf("Move Constructors:\n");
  {
    SharedPtr<Base> mvbc1(new Base(20));
    std::cout << "mvbc1 befor move: " << *mvbc1 << std::endl;
    SharedPtr<Base> mvbc2(std::move(mvbc1));
    (mvbc1 == nullptr) ? std::cout<< "mvbc1 is null\n" : std::cout << "mvbc1 after move: " << *mvbc1 << std::endl;
    (mvbc2 == nullptr) ? std::cout<< "mvbc2 is null\n" :std::cout << "mvbc2 befor move: " << *mvbc2 << std::endl;
  }

  printf("\n\n");
  
  //Copy constructor
  printf("Copy Constructors:\n");
  {
    {
      SharedPtr<Base> cb1(new Derived);
      SharedPtr<Base> cb2(cb1);
      SharedPtr<Base> cb3(cb2);
      
      std::cout << "cb1: " << *cb1 << std::endl;
      std::cout << "cb2: " << *cb2 << std::endl;
      std::cout << "cb3: " << *cb3 << std::endl;
    }
  }

  printf("\n\n");

  //Assignment operator
  printf("Assignment Operators:\n");
  {
    SharedPtr<Base> cb2;
    SharedPtr<Derived> cb1(new Derived);
    SharedPtr<Base> cb3(cb1);

    cb2 = cb1;
    cb2 = cb2;
    
    std::cout << "= cb1: " << *cb1 << std::endl;
    std::cout << "= cb2: " << *cb2 << std::endl;
    std::cout << "= cb3: " << *cb3 << std::endl;
  }


  
  printf("\n\n");
}
