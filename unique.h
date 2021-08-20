/************************************************************
■スマートポインター
	https://cpp.rainy.me/040-smart-pointer.html#unique-ptr
************************************************************/
template < typename T >
class unique_ptr
{
private:
	T * ptr = nullptr ;
	
public :
	unique_ptr() { }
	explicit unique_ptr( T * _ptr ): ptr( _ptr ) { }
	
	~unique_ptr() { delete ptr ; }

	// コピーは禁止
	unique_ptr( const unique_ptr & ) = delete ;
	unique_ptr & operator =( const unique_ptr & ) = delete ;

	// ムーブ
	unique_ptr( unique_ptr && r ) : ptr( r.ptr ) { r.ptr = nullptr ; }
	unique_ptr & operator = ( unique_ptr && r )
	{
		delete ptr ;
		ptr = r.ptr ;
		r.ptr = nullptr ;
	}

	T & operator * () noexcept { return *ptr ; }
	T * operator ->() noexcept { return ptr ; } 
	T * get() noexcept { return ptr ; }
} ;


