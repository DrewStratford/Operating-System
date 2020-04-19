#include <memory/PagingScope.h>
#include <memory/Paging.h>

PagingScope::PagingScope(Thread& thread){
	PTE* pdir = thread.get_pdir();
	old_pdir = Thread::get_current()->get_pdir();
	regions = get_user_regions();
	Thread::get_current()->set_pdir(pdir);
	set_user_regions(&thread.m_user_regions);
	load_cr3(v_to_p((uintptr_t)pdir));
}

PagingScope::~PagingScope(){
	Thread::get_current()->set_pdir(old_pdir);
	set_user_regions(regions);
	load_cr3(v_to_p((uintptr_t)old_pdir));
}

