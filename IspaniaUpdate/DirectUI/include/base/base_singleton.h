
#ifndef CARD3D_BASE_SINGLETON_H
#define CARD3D_BASE_SINGLETON_H
// Turn off warnings generated by this Singleton implementation
#   pragma warning (disable : 4311)
#   pragma warning (disable : 4312)
#include <base\base_base.h>
#include <assert.h>
namespace base
{
	/*
	* �]�pSingleton�򥻤���
	*  
	*  ��@���ϰ쪫��
	*/

     template <typename T> class  Singleton
    {
     protected:
 
         static T* ms_singleton;
 
     public:
         Singleton( void )
         {
             assert( !ms_singleton );
#if defined( _MSC_VER ) && _MSC_VER < 1200   
             int offset = (int)(T*)1 - (int)(Singleton <T>*)(T*)1;
             ms_singleton = (T*)((int)this + offset);
#else
         ms_singleton = static_cast< T* >( this );
 #endif
         }
         ~Singleton( void )
            {  assert( ms_singleton );  ms_singleton = 0;  }
         static T& getSingleton( void )
         {   assert( ms_singleton );  return ( *ms_singleton ); }
        static T* getSingletonPtr( void )
        { return ms_singleton; }
   };
	 template <typename T> T* Singleton <T>::ms_singleton = 0;
}
#endif//CARD3D_BASE_SINGLETON_H