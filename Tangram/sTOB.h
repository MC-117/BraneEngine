//sTOB.h

#ifndef _STOB_H_
#define _STOB_H_

//Include the Standard library new to get in-place new operator for _Kick
#include <new>

//Use these macros in place of 'static' for instances
//that must be statically allocated but only constructed 
//on first use
//The resulting sTOB is used as a pointer to the static instance
#define Declare_pseudo_static( _I ) static sTOB< _I >
#define Implement_pseudo_static( _I ) sTOB< _I >


template< class T >
struct sTOB
{
private:
	//A real T pointer which points to start of the memory array
	union
	{
		T* m_pThis;	
		double dStuffing;// to force alignment of "m_InternalData"
	};

	//Memory array large enough to contain a T
	char m_InternalData[sizeof T];

	//Switch to record first call having been made
	bool m_bInitialised;

	//The _Kick macro ensures that the real T is 
	//created in the statically allocated memory
	//This is the per call overhead but it only 
	//inserts a handful of assmebler instructions

	//_Kick uses the in-place new operator to construct
	//the T in the memory already reserved in the 
	//runtime image for the array.

	#define _Kick								\
	if( !m_bInitialised )						\
	{											\
		/*Set initialised to true first to*/	\
		/*allow codependent constructions*/		\
		m_bInitialised = true;					\
		/*T() is called indirectly here*/		\
		m_pThis = new(&m_InternalData[0]) T;	\
	}


public:

	//We need to overide all these operators
	//The client code may use any of them to 
	//access the sTOB the first time so each must _Kick

	//The sTOB may be assigned from a real T
	T& operator = (T _t)
	{
		_Kick
		*m_pThis = _t;
		return *m_pThis;
	}

	//To convert the sTOB into a simple T*
	operator T*()
	{
		_Kick
		return m_pThis;
	}

	//A function may take a T& and a sTOB<T> be passed
	operator T&()
	{
		_Kick
		return  *m_pThis;
	}

	//The address of a smart T* should be a T** right
	T** operator &()
	{
		_Kick
		return &m_pThis;
	}

	//This allows client code to call through the sTOB
	//e.g mysTOB->GetCmdLine();

	T* operator ->()
	{
		_Kick
		return m_pThis;
	}


	//Construct the sTOB at static initialisation time	
	sTOB() 
	{
		m_bInitialised = false;
		//This sets up the correct actual value for m_pThis		
		//but it won't be valid until after construction
		m_pThis = reinterpret_cast<T*>(m_InternalData);
	}

	
	//Destruct the sTOB at static teardown time
	//Be aware that everything including some of the runtime
	//may have disappeared by the time this is called
	~sTOB()
	{		
		if(m_bInitialised)
		{
			//T destructor called here
			m_pThis->T::~T();
		}
	}
};

#endif//_STOB_H_