/************************************************************
■smart pointer
	■C++11スマートポインタ入門
		https://qiita.com/hmito/items/db3b14917120b285112f
	
	■rvalue reference 完全解説
		https://cpplover.blogspot.com/2009/11/rvalue-reference_23.html

	■C++11 でスマートポインタを使用する - C++ プログラミング
		https://ez-net.jp/article/F8/edjTZRhO/hLtiTyeDv9QP/

	■本当は怖くないムーブセマンティクス
		https://yohhoy.hatenablog.jp/entry/2012/12/15/120839
		
	■スマートポインター
		https://cpp.rainy.me/040-smart-pointer.html#shared-ptr
		
		contents
		-	shared_ptrの実装例
	
	■C++11スマートポインタで避けるべき過ち Top10
		https://postd.cc/top-10-dumb-mistakes-avoid-c-11-smart-pointers/
		
	■C++プログラマーよ！std::make_sharedを安易に使うべからず！
		https://cflat-inc.hatenablog.com/entry/2014/01/20/214551

■operator bool()
	■c++ convert class to boolean
		https://stackoverflow.com/questions/3359954/c-convert-class-to-boolean
		
	■bool型へのexplicitユーザ定義変換
		https://yohhoy.hatenadiary.jp/entry/20121110/p1
		
	■c++,explicitの使い道について知りたい
		https://ja.stackoverflow.com/questions/48085/c-explicit%E3%81%AE%E4%BD%BF%E3%81%84%E9%81%93%E3%81%AB%E3%81%A4%E3%81%84%E3%81%A6%E7%9F%A5%E3%82%8A%E3%81%9F%E3%81%84
		
************************************************************/

#define TEST 24

/************************************************************
概要
	C++11では、unique_ptr<T> shared_ptr<T> weak_ptr<T>の3種のスマートポインタが新たに追加されました。
	これらの使い方を勉強して行きましょう。
	
	newで確保したメモリをdeleteし忘れる					: メモリ リーク
	newしていないポインタ、解放済みのしたメモリをdelete	: 未定義
	
	スマートポインタ
		メモリの動的確保で生じる多くの危険性を低減する目的で使用されるclass。
		確保したメモリを自動的に解放してくれるなど。


keyword : メモリの所有権
	「そのメモリにアクセスする権利と、解放する義務」のこと。
	「所有権を保持する」とは「そのメモリは私が管理するので、俺を通して使ってください。そして、勝手に消さないでください」という意味である。
	
	確保した生のポインタをスマートポインタに渡すと、スマートポインタは、そのポインタの指すメモリに対する所有権を得る。
	
	所有権を持つスマートポインタが全て破棄されれば、そのメモリは誰からも使用されていないことが保障できる。
	そこで、スマートポインタは ディストラクタを用いて所有権を持つメモリが不要となる瞬間を判断し、解放を自動的に行う。


auto_ptr
	C++03までは、スマートポインタとして auto_ptr<T> が用意されていた。
	しかし、 auto_ptr<T>には、
		* 内部処理でコピーを行うため、コンテナ（ std::vector<T>等）に入れることができない。
		* 配列を保持することができない。
		* deleterを指定することができない。
	と言った問題点が複数指摘されている。
	
	そのため、C++11以降では auto_ptr<T>の使用は非推奨となっており、代わりとして今回 説明するする新たな3種のスマートポインタが追加された。
	
	と、言うことで、今回は、auto_ptrの説明は省略します。


説明の流れ
	- xxx_ptrとは
	- xxx_ptrの定義(実体化)
	- xxx_ptr : copy、moveの可否
	- xxx_ptr : 解放
	- xxx_ptr : operator bool()
	- xxx_ptr : access
	- xxx_ptr : raw pointer
	- xxx_ptr : 配列
	- xxx_ptr : deleter
	- xxx_ptr : misc
************************************************************/

/************************************************************
<unique_ptr>
************************************************************/
	/******************************
	unique_ptrとは
		- あるメモリの所有権を持つ unique_ptr<T>は、 ただ一つのみ
		- copy不可。move ok
		- 通常のポインタに匹敵する処理速度。
		- 配列を扱うことができる。
		- deleter（後述）を指定することができる。
	******************************/
#if (TEST == 0)

	/******************************
	unique_ptrの定義(実体化)
	******************************/
#elif(TEST == 1)
	#include<iostream>
	#include<memory> // need this.
	
	int main(){
		std::unique_ptr<int> ptr(new int(10)); // pointerで初期化
		
		std::unique_ptr<int> ptr2;
		ptr2.reset(new int(10)); // 後から代入
		
		std::unique_ptr<int> ptr3 = std::make_unique<int>(10); // C++14以降 : make_unique関数を使うこともできる。
		
		// std::unique_ptr<int> ptr4 = new int(10); // NG.
	}
	
	/******************************
	unique_ptr : copy、moveの可否
		copy : ×
		move : ○
	******************************/
#elif(TEST == 2)
	#include<iostream>
	#include<memory> // need this.
	
	int main(){
		std::unique_ptr<int> ptr(new int(10));
		
		// コピーコンストラクタ、コピー代入演算子 : エラー
		// std::unique_ptr<int> ptr2(ptr); //===ERROR===
		
		// std::unique_ptr<int> ptr3;
		// ptr3 = ptr;   //===ERROR===
		
		
		// ムーブコンストラクタ、ムーブ代入演算子 : OK
		// ->所有権が移動する
		std::unique_ptr<int> ptr4(std::move(ptr));
		
		std::unique_ptr<int> ptr5;
		ptr5 = std::move(ptr4);
	}
	/********************
	unique_ptrの実装例
	********************/
	
	/******************************
	unique_ptr : 解放
		destructor、reset()、で所有権を放棄する。
		unique_ptrは、所有権がただ一つなので、このtimingでメモリも解放する。
	******************************/
#elif(TEST == 3)
	#include<iostream>
	#include<memory> // need this.
	
	int main(){
		{
			std::unique_ptr<int> ptr(new int(10));
		} // ptrのdestructor : メモリの自動 解放
		
		// 明示的な解放
		std::unique_ptr<int> ptr2(new int(10));
		ptr2.reset(); // ptr2.reset(nullptr); // でもok.
	}
	
	/******************************
	unique_ptr : operator bool()
	******************************/
#elif(TEST == 4)
	#include<iostream>
	#include<memory> // need this.
	
	int main(){
		std::unique_ptr<int> ptr_0(new int(10));
		std::unique_ptr<int> ptr_1;
		
		if(ptr_0) { printf("ptr_0 : I have\n"); }
		if(ptr_1) { printf("ptr_1 : I have\n"); }
		
		bool CanAccess = (bool)ptr_0; // 代入時は、明示的cast(cf.explicit)
	}
	
	/******************************
	unique_ptr : access
		operator*(), operator->()
	******************************/
#elif(TEST == 5)
	#include<iostream>
	#include<memory> // need this.
	
	int main(){
		std::unique_ptr<std::string> pStr(new std::string("test"));
		
		std::cout << *pStr << std::endl;        
		
		unsigned int StrSize = pStr->size();
		printf("size = %d\n", StrSize);
	}
	
	/******************************
	unique_ptr : raw pointer
	******************************/
#elif(TEST == 6)
	#include<iostream>
	#include<memory> // need this.
	
	int main(){
		std::unique_ptr<int> ptr(new int(10));
		
		int* pint;
		pint = ptr.get(); // ポインタの所有権はunique_ptrが保持し続ける
		printf("%d\n", (bool)ptr);
		
		pint = ptr.release(); // 所有権を放棄
		printf("%d\n", (bool)ptr);
		delete pint;
	}
	
	/******************************
	unique_ptr : 配列
	******************************/
#elif(TEST == 7)
	#include<iostream>
	#include<memory> // need this.
	
	int main(){
		std::unique_ptr<int[]> ptrArray(new int[10]);
		
		//配列型の場合operator[](size_t)を使うことができる
		for(int i = 0; i < 10; ++i){
			ptrArray[i] = i;
		}
		
	}// 配列型 : 自動的にdelete[]が呼ばれて解放される。	
	
	/******************************
	unique_ptr : deleter
	******************************/
#elif(TEST == 8)
	#include<iostream>
	#include<memory> // need this.
	
	int* malloc_int_from_storage(){
		printf("malloc\n");
		return new int(99);
	}
	void free_int_from_storage(int* ptr){
		printf("deleter\n");
		delete ptr;
	}
	
	/********************
	free_int_from_storageを使ってメモリを解放する関数オブジェクトを定義する。
		■3.3 関数オブジェクト
			https://docs.oracle.com/cd/E19205-01/820-2985/general/3_3.htm
		■関数オブジェクト | Programming Place Plus　C++編【言語解説】　第３４章
			https://programming-place.net/ppp/contents/cpp/language/034.html
	********************/
	struct deleter_for_storage{ // 
		void operator()(int* ptr_){
			free_int_from_storage(ptr_);
		}
	};
	
	int main(){
		std::unique_ptr<int, deleter_for_storage> ptr(malloc_int_from_storage());
		printf("%d\n", *ptr);
		
		ptr.reset(); //deleteではなく、free_int_from_storageがメモリ解放の際に呼ばれる。
		if(ptr)	printf("I have\n");
		else	printf("I don't have\n");
		
		return 0;
	}
	
	/******************************
	■bool型へのexplicitユーザ定義変換
		https://yohhoy.hatenadiary.jp/entry/20121110/p1
		
			以下の場合、"コンテキストに依存したbool型への変換(contextually converted to bool)”が行われると定義する。
			簡単に言うと、セマンティクス上「真偽値が要求される場所」が列挙されている。
			- 論理否定演算子!のオペランド（5.3.1/p9）
			- 論理積演算子&&の両オペランド（5.14/p1）
			- 論理和演算子||の両オペランド（5.15/p1）
			- 条件演算子? :の第1オペランド（5.16/p1）
			- if文の条件式（6.4/p4）
			- for文の条件式（6.4/p2,4）
			- while文の条件式（6.4/p2,4）
			- do文の条件式（6.5.2/p1）
			- static_assertの定数式（7/p4）
			- 関数に対するnoexpect例外指定の定数式（15.4/p1）
	******************************/
#elif(TEST == 9)
	#include<iostream>
	#include<memory> // need this.
	
	class T {
	public:
		explicit operator bool() const{ // cast : T -> bool
			return true;
		}
	};
	
	void func(bool flag){
		if(flag)	printf("true  passed\n");
		else		printf("false passed\n");
	}
	
	int main(){
		T t;
		// func(t); // NG : 暗黙変換
		func(bool(t));
		
		if(t){
			printf("this is OK\n");
		}
		
		return 0;
	}
	
	
/************************************************************
<shared_ptr>
************************************************************/
	/******************************
	shared_ptrとは
		shared_ptrは、同一のメモリの所有権を複数で共有できるようにしたスマートポインタ。
		shared_ptr<T>は、所有権を持つポインタの数を記録するカウンタを持っている。所有権を持つ shared_ptr<T>がコピーされると、内部でカウンタがインクリメントされ、ディストラクタや明示的解放時にデクリメントされる。
		全ての所有者がいなくなると、カウンタがゼロとなり、メモリが解放される。カウンタで所有者数を管理することで、複数の shared_ptr<T>が所有権を保持していても、適切なタイミングで一度だけメモリ解放が実行されるのである。

		shared_ptr<T>は、以下のような特徴を持つ。
		- あるメモリの所有権を、複数の shared_ptr<T>で 共有することができる。メモリの解放は、全ての所有権を持つ shared_ptr<T>が破棄された際に実行される。
		- コピーもムーブも可能。
		- 内部でカウンタを利用している関係上、やや通常のポインタよりメモリ確保やコピー等の処理が遅い。
		- 配列を扱うことができる。ただし、明示的にdeleterを指定する必要がある。
	******************************/
#elif(TEST == 10)
	#include<memory>
	#include<iostream>
	int main(){
		std::shared_ptr<int> ptr; // empty.
		
		{
			std::shared_ptr<int> ptr2(new int(0));
			
			ptr = ptr2; // 所有権をcopy
			
			*ptr	+= 10;
			*ptr2	+= 10;
		}	// ここで、ptr2のディストラクタが呼ばれる
			// ptrも同一のメモリに対する所有権を持っているため、まだ解放はされない
		
		//当然、ptrはまだ使用可能
		std::cout<<"ptr="<<*ptr<<std::endl;  //"ptr=20"と出力
		
	}	// ここで、ptrのディストラクタが呼ばれる
		// メモリの所有権を持つポインタがいなくなったので、解放される
		
	/********************
	shared_ptrの実装例
	********************/
 
 
	/******************************
	shared_ptrの定義(実体化)
	******************************/
#elif(TEST == 11)
	#include<memory>
	#include<iostream>
	int main(){
		// constructor, reset
		std::shared_ptr<int> ptr(new int(10));
		std::shared_ptr<int> ptr2;
		ptr2.reset(new int(10));
		
		// make_shared関数を使うと、効率よくメモリを確保できる（C++11から使える）
		// ->	shared_ptr<T>は所有するメモリだけでなく自身のカウンタも動的にメモリを確保する必要がある。
		//		make_shared<T>(args...)は、これらのメモリ確保を同時に行えるため、処理効率が良い。
		//		可能な限りこちらを使うべきである。
		std::shared_ptr<int> ptr3=std::make_shared<int>(10);
		
		// 複数の引数を持つコンストラクタも、make_sharedから呼び出せる
		typedef std::pair<int,double> int_double_t;
		std::shared_ptr<int_double_t> ptr4 = std::make_shared<int_double_t>(10, 0.4);
	}
	
	
	/******************************
	shared_ptr : copy、moveの可否
		copy : ○
		move : ○
	******************************/
#elif(TEST == 12)
	#include<memory>
	#include<iostream>
	int main(){
		{
			std::shared_ptr<int> ptr=std::make_shared<int>(10);
			std::shared_ptr<int> ptr2(ptr);	//ok : 所有権を共有
			std::shared_ptr<int> ptr3;
			ptr3=ptr;						//ok : 所有権を共有
			
			std::shared_ptr<int> ptr4(std::move(ptr));	//ok : 所有権が移動する
			std::shared_ptr<int> ptr5;
			ptr5=std::move(ptr2);   					//ok : 所有権が移動する
		}
		
		// unique_ptr<T>からムーブすることも可能
		{
			std::unique_ptr<int> uptr(new int(10));
			std::shared_ptr<int> ptr(std::move(uptr)); //所有権がuptrからptrに移動する
			
			//代入演算子で、ムーブ
			std::unique_ptr<int> uptr2(new int(10));
			std::shared_ptr<int> ptr2;
			ptr2=std::move(uptr2);
		}
	}
	
	/******************************
	shared_ptr : 解放
	******************************/
#elif(TEST == 13)
	#include<memory>
	#include<iostream>
	int main(){
		std::shared_ptr<int> ptr = std::make_shared<int>(10);
		{
			std::shared_ptr<int> ptr2(ptr);
		}	// ここでptr2のディストラクタが呼ばれ、ptr2は所有権を放棄
			// But : ptrがまだ所有権を保有しているので、メモリは解放されていない
		
		// 引数なしやnullptrを引数としてreset関数を呼んでも、明示的に所有権を放棄できる
		// ptrが所有権を放棄すると、所有権を持つポインタがなくなるので、ここでメモリ解放
		ptr.reset();
	}
	
	/******************************
	shared_ptr : operator bool()
	******************************/
#elif(TEST == 14)
	#include<memory>
	#include<iostream>
	int main(){
		// std::shared_ptr<int> ptr;
		std::shared_ptr<int> ptr = std::make_shared<int>(10);
		
		if(ptr){
			//---所有しているときの処理---
		}
		
		bool CanAccess = bool(ptr);
		
		// 所有者の数を確認
		std::cout<<"use_count="<<ptr.use_count()<<std::endl;
		
		// 所有者が唯一であることを確認
		if(ptr.unique()){
			std::cout<<"unique"<<std::endl; 
		}
	}
	
	/******************************
	shared_ptr : access
		operator*(), operator->()
	******************************/
#elif(TEST == 15)
	#include<memory>
	#include<iostream>
	int main(){
		std::shared_ptr<std::string> pStr=std::make_shared<std::string>("test");
		
		std::cout << *pStr << std::endl;        
		
		printf("size = %d\n", pStr->size());
	}
	
	/******************************
	shared_ptr : raw pointer
	******************************/
#elif(TEST == 16)
	#include<memory>
	#include<iostream>
	int main(){
		std::shared_ptr<int> ptr=std::make_shared<int>(10);
		
		// 通常のポインタがほしい時には、get関数を使う。
		// ポインタの所有権はshared_ptrが保持し続ける
		int* pint;
		pint=ptr.get();
	}
	/********************
	releaseは用意されていない。
		reset	:	所有権を放棄し、他に所有権を持つObjectがなければメモリを解放。
					所有権を持つshared_ptr Objectがいれば、そのshared_ptrに管理を任せる。
		release	:	所有権を放棄し、raw pointerを返す。
					メモリの管理は、raw pointerに任せる。
					But :	shared_ptrは、複数のObjectでメモリをshareできるので、
							他に所有権を持つObjectがいるかもしれない。
							「shared_ptrとraw pointerでメモリを共有する」ことになり、
							管理が 非常に困難になってしまう。
	********************/
	
	/******************************
	shared_ptr : 配列
		配列を扱うこともできるが、shared_ptr<T[]>でない点に注意。
		また、 operator[](size_t)は用意されていないのでaccessの仕方にも注意。
		さらに、 deleterを明示的に指定する必要がある。なお、deleterを明示的に指定する際には、 make_shared<T>(args...)は使えない。
	******************************/
#elif(TEST == 17)
	#include<memory>
	#include<iostream>
	int main(){
		// []型名をテンプレート引数に指定することで、配列も扱える
		// 第2引数で、配列用にdeleterを指定
		// deleterを明示的に指定する際には、make_sharedは使えない
		std::shared_ptr<int> ptrArray(new int[10], std::default_delete<int[]>());
		
		// operator[]は使えない
		// 代わりに、get関数からアクセスはできる
		for(int i=0;i<10;++i){
			// ptrArray[i]=i;      //===ERROR=== 
			ptrArray.get()[i]=i;  //ok
		}
	}// default_delete<int[]>を指定しておけば、自動的にdelete[]が呼ばれて解放される。
	
	/******************************
	shared_ptr : 循環参照
	******************************/
#elif(TEST == 18)
	#include<memory>
	#include<iostream>
	class hoge{
	public:
		std::shared_ptr<hoge> ptr;
	};
	
	int main(){
		std::shared_ptr<hoge> pHoge1=std::make_shared<hoge>();
		std::shared_ptr<hoge> pHoge2=std::make_shared<hoge>();
		
		pHoge1->ptr=pHoge2;
		pHoge2->ptr=pHoge1;
		
		return 0;
   } //shared_ptrのディストラクタが呼ばれるのに、確保した二つのhogeが解放されない。


/************************************************************
<weak_ptr>
************************************************************/
	/******************************
	weak_ptrとは
		weak_ptr<T>は、循環参照によって生じる問題を防ぐために導入されたスマートポインタです。
		先の二つのスマートポインタと違い、 weak_ptr<T>は メモリへの所有権を持つことは、ありません。
		その代わりに、 weak_ptr<T>は shared_ptr<T>の指すメモリを参照することができます。
	******************************/
#elif(TEST == 19)
	#include<memory>
	#include<iostream>
	class hoge{
	public:
		//shared_ptrで所有権を得る代わりに、weak_ptrで参照する
		std::weak_ptr<hoge> ptr;
	};
	
	int main(){
		std::shared_ptr<hoge> pHoge1=std::make_shared<hoge>();
		std::shared_ptr<hoge> pHoge2=std::make_shared<hoge>();
		
		pHoge1->ptr=pHoge2;
		pHpge2->ptr=pHoge1;
		
	}//所有権は、pHoge1, pHoge2しかそれぞれ持っていないので、正しく解放される	
	
	/******************************
	weak_ptrの定義(実体化)
		weak_ptr<T>はshared_ptr<T>が所有権を持つメモリしか参照できない。
	******************************/
#elif(TEST == 20)
	#include<memory>
	#include<iostream>
	
	int main(){
		std::shared_ptr<int> sptr=std::make_shared<int>(10);
		std::weak_ptr<int> wptr(sptr); 
		std::weak_ptr<int> wptr2;
		wptr2=sptr;
		
		//ポインタを直接受け取ることはできない
		// std::weak_ptr<int> wptr3(new int(10)); //===ERROR===
	}
	
	/******************************
	weak_ptr : copy、moveの可否
		copy : ○
		move : ○
	******************************/
#elif(TEST == 21)
	#include<memory>
	#include<iostream>
	
	int main(){
		std::shared_ptr<int> sptr=std::make_shared<int>(10);
		std::weak_ptr<int> wptr(sptr);
		
		// copy
		std::weak_ptr<int> wptr2(wptr);
		std::weak_ptr<int> wptr3;
		wptr3=wptr;
		
		// move
		// この時、wptr2,wptr3は参照を失う
		std::weak_ptr<int> wptr4(std::move(wptr2));
		std::weak_ptr<int> wptr5;
		wptr5=std::move(wptr3);
	}
	
	
	/******************************
	weak_ptr : 解放
	******************************/
#elif(TEST == 22)
	#include<memory>
	#include<iostream>
	
	int main(){
		std::shared_ptr<int> sptr=std::make_shared<int>(10);
		{
			std::weak_ptr<int> wptr(sptr);
		} //ディストラクタで参照を解放
		
		std::weak_ptr<int> wptr2(sptr);
		
		//reset関数で明示的に解放
		wptr2.reset();
	}

	/******************************
	weak_ptr : operator bool()
		所有権はそもそも保持していないので、 operator bool()は使えない。
	******************************/
#elif(TEST == 23)
	#include<memory>
	#include<iostream>
	
	int main(){
		std::shared_ptr<int> sptr=std::make_shared<int>(10);
		std::weak_ptr<int> wptr(sptr);
		
		//参照先のメモリが解放されていないかどうかはexpired関数を使う
		if(wptr.expired()){
			std::cout<<"expired"<<std::endl;
		}
		
		//所有者の数を確認するには、use_count関数を使う
		std::cout<<"use_count="<<wptr.use_count()<<std::endl;
	}
	
	
	/******************************
	weak_ptr : access
		参照するメモリにアクセスするのに、 operator*()や operator->()は使えない。
		メモリにアクセスするためには、まず lock()によって参照先を保持する shared_ptr<T>を取得し、そこからアクセスする。
		これは、使用中に解放されてしまうのを避けるためである。
	******************************/
#elif(TEST == 24)
	#include<memory>
	#include<iostream>
	
	int main(){
		std::shared_ptr<int> sptr=std::make_shared<int>(10);
		std::weak_ptr<int> wptr(sptr);
		
		{
			//lock関数によって、参照先を保持するshared_ptrを取得する
			std::shared_ptr<int> ptr = wptr.lock();
			
			// この後、仮に同メモリを管理している他のshared_ptrが所有権を放棄しても、"ptr"が所有権を持っているので、メモリは解放されない。
			
			// しかし、wptrの参照先が、shared_ptr取得前の段階で、「すでに」解放されていたら・・・
			/*
			if (!wptr.expired()){
				std::shared_ptr<int> ptr = wptr.lock();
			}
			*/
			/********************
			↑NG例 : expiredのcheckとshared_ptrの取得の間で、別threadにて処理が入り、メモリが解放されるかもしれないので。
			********************/
			
			/********************
			正解
			********************/
			if(ptr){	// 取得したshared_ptrが指すメモリが、すでに解放済みの場合もあるので、check
				printf("do something\n");
			}else{
				printf("no memory\n");
			}
			
		}// デストラクタでptrが所有権を放棄
	}

/************************************************************
注意すべきcode
************************************************************/
#elif(TEST == 25)
	/******************************
	オブジェクト（生ポインタ）を作成してすぐにshared_ptrに割り当てない
	******************************/
	#include<memory>
	#include<iostream>
	
	int main()
	{
		int* raw_ptr = new int(10);
		
		std::shared_ptr<int> ptr_0(raw_ptr);
		std::cout << ptr_0.use_count() << std::endl;
		
		std::shared_ptr<int> ptr_1(raw_ptr);
		std::cout << ptr_1.use_count() << std::endl;
		
		return 0;
	}
	/********************
	sharedptrの作成にmakesharedを使用していないのであれば、せめてスマートポインタで管理されたオブジェクトを同じコード行に作成する。
		shared_ptr<int> ptr_0(new int(10));
	********************/
	
#elif(TEST == 26)
	/******************************
	shared_ptrで使用されている生ポインタを削除してしまう
	******************************/
	#include<memory>
	#include<iostream>
	
	int main()
	{
		std::shared_ptr<int> ptr(new int(10));
		int* raw_ptr = ptr.get();
		delete raw_ptr;
	}
	
#endif

/************************************************************
まとめ
	-	まずは、unique_ptrを使用する。もし後からリソースの共有をする必要が出たら、その時にshared_ptrに変更する。
	-	shared_ptrを使う場合は、thread safeになるよう注意
	-	循環参照が起きる所は、weak_ptrでメモリリークを避ける
************************************************************/



