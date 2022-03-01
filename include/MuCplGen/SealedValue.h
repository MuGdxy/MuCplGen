#pragma once
#include <any>

struct Sealed
{
	std::any o;
	virtual ~Sealed() {}
};

template<typename T>
struct SealedValue :public Sealed
{
	template<class ...Args>
	SealedValue(Args&&... args)
	{
		o = std::make_any<T>(::std::forward<Args>(args)...);
	}
	virtual ~SealedValue() 
	{ }
};


//struct Sealed
//{
//	void* ptr = nullptr;
//	virtual ~Sealed() {}
//};
//
//template<typename T>
//struct SealedValue :public Sealed
//{
//	SealedValue(T val) :ptr(new T(val)) {}
//	SealedValue(T* obj_ptr) :ptr(obj_ptr) {}
//	virtual ~SealedValue()
//	{
//		delete (T*)ptr;
//	}
//};
