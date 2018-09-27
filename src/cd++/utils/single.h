/*******************************************************************
*
*  DESCRIPTION: template class Singleton<T> and SetSingleton<T>
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*
*  DATE: 27/6/1998
*
*******************************************************************/

#ifndef _SINGLETON_HPP
#define _SINGLETON_HPP

/** include files **/
#include <assert.h>

/** definitions **/

template <class T>
class Singleton
{
public:
	/* Queries */
	// --- Devuelve la instancia correspondiente
	static T *Instance() ;

private:
	/* Forward declarations */

	/* Variables de clase */
	// --- La unica instancia de la calse
	// static T *_instance  ;

	/* Constructor */
	// --- Protected porque es una clase abstracta
	Singleton() ;
} ;

/** inline methods **/

template <class T>
inline
T *Singleton<T>::Instance()
{
	static T *_instance( NULL ) ;

	if( !_instance )
		_instance = new T() ;
	return _instance ;      
}

#endif // _SINGLETON_HPP
