#pragma once

class SharedInternal{
private:
	int references { 0 };

public:
	void dec() { references--; }
	void inc() { references++; }
	bool should_destruct() { return references == 0; }
};

template <typename T>
class Shared{
private:
	SharedInternal* state { nullptr };
	T* data { nullptr };

public:
	Shared(){}

	Shared(T* t) : data(t) {
		state = new SharedInternal();
		state->inc();
	}

	Shared(T&& t){
		data = new T(t);
		state = new SharedInternal();
		state->inc();
	}

	Shared(Shared&& s){
		data = s.data;
		state = s.state;
		state->inc();
	}

	Shared(const Shared& s){
		data = s.data;
		state = s.state;
		state->inc();
	}

	~Shared(){
		state->dec();
		if(state->should_destruct()){
			delete state;
			delete data;
		}
		state = nullptr;
		data = nullptr;
	}

	Shared& operator=(const Shared& s){
		if(this == &s) return *this;
		this->~Shared();
		this->state = s.state;
		this->data = s.data;
		this->state->inc();
		return *this;
	}

	Shared& operator=(Shared&& s){
		if(this == s) return this;
		this->~Shared();
		this->state = s.state;
		this->data = s.data;
		this->state->inc();
		return this;
	}

 	T* operator->() const { return data; }
 	T& operator*()  const { return *data; }

};
