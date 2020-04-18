#pragma once

#include <Thread.h>

class PagingScope{
public:
	PagingScope(Thread&);
	~PagingScope();
private:
	PTE* old_pdir { nullptr };
};
