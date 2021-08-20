/************************************************************
■スマートポインター
	https://cpp.rainy.me/040-smart-pointer.html#unique-ptr
************************************************************/
template < typename T >
class shared_ptr
{
	T * ptr = nullptr ;
	std::size_t * count = nullptr ;

	void release(){
		if ( count == nullptr ) return ;

		--*count ;
		if ( *count == 0 ){
			delete ptr ;
			ptr = nullptr ;
			
			delete count ;
			count = nullptr ;
		}
	}
	
public :
	shared_ptr() { }
	explicit shared_ptr( T * _ptr ) : ptr(_ptr), count( new std::size_t(1) )	{ }
	~shared_ptr()
	{
		release() ;
	}

	shared_ptr( const shared_ptr & r )
	: ptr( r.ptr ), count( r.count )
	{
		++*count ;
	}
	shared_ptr & operator =( const shared_ptr & r )
	{
		if ( this == &r )
			return *this ;

		release() ;
		ptr = r.ptr ;
		count = r.count ;
		++*count ;
	}

	shared_ptr( shared_ptr && r )
	: ptr(r.ptr), count(r.count)
	{
		r.ptr = nullptr ;
		r.count = nullptr ;
	}

	shared_ptr & operator =( shared_ptr && r )
	{
		release() ;
		
		ptr = r.ptr ;
		count = r.count ;
		
		r.ptr = nullptr ;
		r.count = nullptr ;
	}

	T & operator * () noexcept { return *ptr ; }
	T * operator ->() noexcept { return ptr ; } 
	T * get() noexcept { return ptr ; }
} ;

