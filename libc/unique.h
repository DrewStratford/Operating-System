template <typename T>
class Unique {
public:
	Unique(){}
	Unique(T* t) : ptr(t){
	}

	Unique(T&& t){
		if(ptr) delete ptr;
		ptr = new T(t);
	}

	Unique(Unique&& u){
		if(ptr) delete ptr;
		ptr = u.release();
	}

	Unique(Unique& u){
		if(ptr) delete ptr;
		ptr = u.release();
	}

	Unique& operator=(Unique&& u){
		if(this == u) return this;
		if(ptr) delete ptr;
		ptr = u.release();
		return this;
	}

	T* release(){
		T* t = ptr;
		ptr = nullptr;
		return t;
	}

	T* get(){ return ptr; }
	
 	T* operator->() const { return ptr; }
 	T& operator*()  const { return *ptr; }

	~Unique(){
		if(ptr)
			delete ptr;
	}
private:
	T* ptr { nullptr };
};
