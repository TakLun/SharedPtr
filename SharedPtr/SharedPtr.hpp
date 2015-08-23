#ifndef __SHARED_PTR_HPP
#define __SHARED_PTR_HPP

#include <functional>
#include <mutex>

namespace cs540{
  
  class Ref_Cnt{
  private:
    int reference_count;
    std::mutex mtx;
  public:
    Ref_Cnt() : 
      reference_count(1)
    {}

    int inc_ref(){
      std::unique_lock<std::mutex> lock(mtx);
      ++reference_count;
      return reference_count;
    }

    int dec_ref(){
      std::unique_lock<std::mutex> lock(mtx);
      if(reference_count > 0){
	--reference_count;
      }
      return reference_count;
    }
  };

  template <typename T>
  class SharedPtr{

    template <typename U>
    friend class SharedPtr;

  public:
    T *object;

  private:
    Ref_Cnt *ref;
    std::function<void()> deleteObject;

  public:

    /* Constructors */
    //Default Constructors
    SharedPtr() : 
      object(nullptr),
      ref(nullptr),
      deleteObject(nullptr)
    { }

    template <typename U> explicit SharedPtr(U *o) :
      object(o),
      ref(new Ref_Cnt),
      deleteObject([o]()->void{ if(o != nullptr) delete o; })
    { }
   
    // Copy constructors
    SharedPtr(const SharedPtr &p) :
      object(p.object),
      ref(p.ref),
      deleteObject(p.deleteObject)
    { 
      if(ref == nullptr){
	ref = new Ref_Cnt;
      }else{
	ref->inc_ref();
      }
    }

    template <typename U> SharedPtr(const SharedPtr<U> &p) :
      object(static_cast<T *>(p.object)),
      ref(p.ref),
      deleteObject(p.deleteObject)
    { 
      if(ref == nullptr){
	ref = new Ref_Cnt;
      }else{
	ref->inc_ref();
      }
    }

    // Dynamic Pointer Cast Helper Constructor
    template <typename U> SharedPtr(const SharedPtr<U> &p, U* o) :
      object(dynamic_cast<T *>(o)),
      ref(p.ref),
      deleteObject(p.deleteObject)
    { 
      if(ref == nullptr){
	ref = new Ref_Cnt;
      }else{
	ref->inc_ref();
      }
    }

    // Move Constructors
    SharedPtr(SharedPtr &&p) : 
      object(p.object),
      ref(p.ref),
      deleteObject(p.deleteObject)
    {
      p.object = nullptr;
      p.ref = nullptr;
      p.deleteObject = nullptr;
    }

    template <typename U> SharedPtr(SharedPtr<U> &&p) :
      object(p.object),
      ref(p.ref),
      deleteObject(p.deleteObject)
    {
      p.object = nullptr;
      p.ref = nullptr;
      p.deleteObject = nullptr;
    }

    // Destructor
    ~SharedPtr(){
      if(ref != nullptr && ref->dec_ref() == 0){
	delete ref;
	if(deleteObject != nullptr){
	  deleteObject();
	}
      }
    }

    /* Member Function */
    T *get() const{ return object; }

    void reset(){
      if(ref != nullptr && ref->dec_ref() == 0){
	delete ref;
	if(deleteObject != nullptr){
	  deleteObject();
	}
      }
      ref = nullptr;
      object = nullptr;
      deleteObject = nullptr;
    }

    template <typename U> void reset(U *o){
      if(ref != nullptr && ref->dec_ref() == 0){
	delete ref;
	if(deleteObject != nullptr){
	  deleteObject();
	}
      }
      object = o;
      ref = new Ref_Cnt();
      deleteObject = [o]()->void{ delete o; };
    }

    /* Operator */
    // Assignment Operators
    SharedPtr &operator=(const SharedPtr &p){
      if(*this != p){
	if(ref != nullptr && ref->dec_ref() == 0){
	  delete ref;
	  if(deleteObject != nullptr){
	    deleteObject();
	  }
	}
	object = p.object;
	ref = p.ref;
	if(ref == nullptr){
	  ref = new Ref_Cnt;
	}else{
	  ref->inc_ref();
	}
	deleteObject = p.deleteObject;
      }
      return *this;
    }

    template <typename U>
    SharedPtr<T> &operator=(const SharedPtr<U> &p){
      if(*this != p){
	if(ref != nullptr && ref->dec_ref() == 0){
	  delete ref;
	  if(deleteObject != nullptr){
	    deleteObject();
	  }
	}
	object = static_cast<T *>(p.object);
	ref = p.ref;
	if(ref == nullptr){
	  ref = new Ref_Cnt;
	}else{
	  ref->inc_ref();
	}
	deleteObject = p.deleteObject;
      }
      return *this;
    }

    // Move Assignment Operators
    SharedPtr &operator=(SharedPtr &&p){
      if(*this != p){
	if(ref != nullptr && ref->dec_ref() == 0){
	  delete ref;
	  if(deleteObject != nullptr){
	    deleteObject();
	  }
	}
	object = p.object;
	ref = p.ref;
	deleteObject = p.deleteObject;

	p.object = nullptr;
	p.ref = nullptr;
	p.deleteObject = nullptr;
      }
      return *this;
    }

    template <typename U>
    SharedPtr<T> &operator=(SharedPtr<U> &&p){
      if(*this != p){
	if(ref != nullptr && ref->dec_ref() == 0){
	  delete ref;
	  if(deleteObject != nullptr){
	    deleteObject();
	  }
	}
	object = static_cast<T *>(p.object);
	ref = p.ref;
	deleteObject = p.deleteObject;

	p.object = nullptr;
	p.ref = nullptr;
	p.deleteObject = nullptr;
      }
      return *this;
    }

    T &operator*() const{ return *object; }

    T *operator->() const{ return object; }

    explicit operator bool() const{ return *this != nullptr; }

  };

  /* Non-member (Free) Functions */
  template <typename T1, typename T2>
  bool operator==(const SharedPtr<T1> &p1, const SharedPtr<T2> &p2){
    return p1.object == p2.object;
  }

  template <typename T>
  bool operator==(const SharedPtr<T> &p, std::nullptr_t nullpt){
    return p.object == nullpt;
  }

  template <typename T>
  bool operator==(std::nullptr_t nullpt, const SharedPtr<T> &p){
    return nullpt == p.object;
  }


  template <typename T1, typename T2>
  bool operator!=(const SharedPtr<T1> &p1, const SharedPtr<T2> &p2){
    return p1.object != p2.object;
  }

  template <typename T>
  bool operator!=(const SharedPtr<T> &p, std::nullptr_t nullpt){
    return p.object != nullpt;
  }

  template <typename T>
  bool operator!=(std::nullptr_t nullpt, const SharedPtr<T> &p){
    return nullpt != p.object;
  }


  template <typename T, typename U>
  SharedPtr<T> static_pointer_cast(const SharedPtr<U> &sp){
    return SharedPtr<T>(sp);
  }

  template <typename T, typename U>
  SharedPtr<T> dynamic_pointer_cast(const SharedPtr<U> &sp){
    return SharedPtr<T>(sp, sp.object);
  }
  
}


#endif
