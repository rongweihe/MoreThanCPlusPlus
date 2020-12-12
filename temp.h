#ifdef __USE_MALLOC
...
typedef __malloc_alloc_template<0> malloc_alloc;
typedef malloc_alloc alloc; // 令 alloc 为第一级配置器
#else
...
typedef __default_alloc_template<__NODE_ALLOCATOR_THREADS, 0>alloc; // 令 alloc 为第二级配置器
#endif /* !__USE_MALLOC */

template<class T, class Alloc = alloc> //默认使用 alloc 为配置器
class vector {
protected:
	//专属之空间配置器 每次配置一个元素大小
    typedef simple_alloc<value_type, Alloc> data_allocator;
    void deallocate(pointer p, size_type n) {
    	if (...) {
    		data_allocator::deallocate(start, end_of_storage - start);
    	}	
    }
};
