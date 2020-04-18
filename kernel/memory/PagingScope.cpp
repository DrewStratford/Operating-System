#include <memory/PagingScope.h>
#include <memory/Paging.h>

PagingScope::PagingScope(Thread& thread){
	PTE* pdir = thread.get_pdir();
	old_pdir = Thread::get_current()->get_pdir();
	Thread::get_current()->set_pdir(pdir);
	load_cr3(v_to_p((uintptr_t)pdir));
}

PagingScope::~PagingScope(){
	Thread::get_current()->set_pdir(old_pdir);
	load_cr3(v_to_p((uintptr_t)old_pdir));
}

