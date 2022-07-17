#pragma once

class SharedInternal{
private:
	int references { 0 };
	int weak_references { 0 };

public:
	static int dec(SharedInternal* shared) {
		if(shared)
			return __atomic_fetch_sub(&shared->references, 1, __ATOMIC_ACQ_REL);
		return 0;
	}
	static int inc(SharedInternal* shared) {
		if(shared)
			return __atomic_fetch_add(&shared->references, 1, __ATOMIC_RELAXED);
		return 0;
	}
	static int inc_weak(SharedInternal* shared) {
		if(shared)
			return __atomic_fetch_add(&shared->weak_references, 1, __ATOMIC_RELAXED);
		return 0;
	}
	static int dec_weak(SharedInternal* shared) {
		if(shared)
			return __atomic_fetch_sub(&shared->weak_references, 1, __ATOMIC_ACQ_REL);
		return 0;
	}

	static int inc_weak_shared(SharedInternal* shared){
		if(!shared)
			return 0;
		while(shared->references != 0){
			int refs = shared->references;
			if(__atomic_compare_exchange_n(&shared->references, &refs, refs+1, false, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)){
				return shared->references;
			}
		}
		return 0;
	}

	bool should_destruct() { return references == 0; }
	bool should_destruct_counter() { return references == 0 && weak_references == 0; }
	bool has_references() { return references > 0; }
};

template <typename T> class Weak;

template <typename T>
class Shared{
private:
	SharedInternal* state { nullptr };
	T* data { nullptr };

public:
	Shared(){}

	Shared(T* t) : data(t) {
		state = new SharedInternal();
		SharedInternal::inc(state);
	}

	Shared(T&& t){
		data = new T(t);
		state = new SharedInternal();
		SharedInternal::inc(state);
	}

	Shared(Shared&& s){
		data = s.data;
		state = s.state;
		SharedInternal::inc(state);
	}

	Shared(const Shared& s){
		data = s.data;
		state = s.state;
		SharedInternal::inc(state);
	}

	~Shared(){
		if(!state) return;
		SharedInternal::dec(state);
		if(state->should_destruct()){
			delete data;
		}
		if(state->should_destruct_counter()){
			delete state;
		}
		state = nullptr;
		data = nullptr;
	}

	Shared& operator=(const Shared& s){
		if(this == &s) return *this;
		this->~Shared();
		this->state = s.state;
		this->data = s.data;
		SharedInternal::inc(this->state);
		return *this;
	}

	Shared& operator=(Shared&& s){
		if(this == &s) return *this;
		this->~Shared();
		this->state = s.state;
		this->data = s.data;
		SharedInternal::inc(this->state);
		return *this;
	}

	Weak<T> make_weak(){
		Weak<T> out;
		out.state = this->state;
		out.data = this->data;
		SharedInternal::inc_weak(this->state);
		return out;
	}

	operator bool() const {
		return data;
	}

 	T* operator->() const { return data; }
 	T& operator*()  const { return *data; }

	friend class Weak<T>;
};

template <typename T>
class Weak{
private:
	SharedInternal* state { nullptr };
	T* data { nullptr };

public:
	Weak(){}

	Weak(Weak&& s){
		data = s.data;
		state = s.state;
		SharedInternal::inc_weak(state);
	}

	Weak(const Weak& s){
		data = s.data;
		state = s.state;
		SharedInternal::inc_weak(state);
	}

	~Weak(){
		if(!state) return;
		SharedInternal::dec_weak(state);
		if(state->should_destruct_counter()){
			delete state;
		}
		state = nullptr;
		data = nullptr;
	}

	Weak& operator=(const Weak& s){
		if(this == &s) return *this;
		this->~Weak();
		this->state = s.state;
		this->data = s.data;
		SharedInternal::inc_weak(this->state);
		return *this;
	}

	Weak& operator=(Weak&& s){
		if(this == &s) return *this;
		this->~Weak();
		this->state = s.state;
		this->data = s.data;
		SharedInternal::inc_weak(this->state);
		return *this;
	}

	bool is_valid(){
		if(state == nullptr)
			return false;
		return state->has_references();
	}

	Shared<T> lock(){
		Shared<T> out;
		// We need to be very careful about how we increment the count
		// as another thread may be deleting the last shared pointer

		if(SharedInternal::inc_weak_shared(this->state) > 0){
			// There was already a shared pointer so we can copy the pointer and state
			out.state = this->state;
			out.data = this->data;
		}

		return out;
	}

 	T* operator->() const { return data; }
 	T& operator*()  const { return *data; }

	friend class Shared<T>;
};
