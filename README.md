# SharedPtr
Custom Smart / Shared Pointer using C++

Implements a smart pointer similar to std::shared_ptr that only deletes its managed object when the associated reference count reaches 0. SharedPtr should be copyable and assignable. Such an operation should increase the reference count of the associated object, not duplicate the object pointed to.
