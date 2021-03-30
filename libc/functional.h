#pragma once
#include "unique.h"

using namespace std;

template <typename Ret, typename... Args>
class Func{
private:


	class ICallable{
	public:
		virtual ~ICallable() = default;
		virtual Ret invoke(Args...) = 0;
	};

	template <typename T>
	class Callable : public ICallable{
	public:
		Callable(const T& t) : t_(t){
		}

		~Callable() override = default;

		Ret invoke(Args... args){
			return t_(args...);
		}

	private:
		T t_;
	};

	Unique<ICallable> callable { nullptr };
	//ICallable* callable { nullptr };

public:

	//~Func(){
	//	if(callable)
	//		delete callable;
	//}

	Func& operator=(nullptr_t){
		callable = nullptr;
		return *this;
	}

	Func(){
		callable = nullptr;
	}
	template <typename T>
	Func(const T& t){
		callable = new Callable<T>(t);
	}

	template <typename T>
	Func& operator=(const T& t) {
		callable = new Callable<T>(t);
		return *this;
	}

	Ret operator()(Args... args) const{
		return callable->invoke(args...);
	}

};

template <typename A, typename B, typename C>
auto compose(Func<A,B>& f1, Func<B, C>& f2) {
	return [&](A a) { 
		return f2(f1(a)); 
		};
}
