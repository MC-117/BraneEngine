#pragma once
#ifndef _DELEGATE_H_
#define _DELEGATE_H_

#include <functional>
#include <list>

template<typename ...T>
class Delegate{ };


template<typename R, typename ...Args>
class Delegate<R(Args...)>
{
public:
	typedef std::function<R(Args...)> dele_func;
	typedef std::list<dele_func> function_list;
	typedef R(*func_type)(Args...);
	typedef Delegate<R(Args...)>  this_type;


	Delegate()
	{
	};

	~Delegate()
	{
		func_list.clear();
		//cout << typeid(this_type).path() << " destroy\n";
	}

	bool empty()
	{
		return func_list.empty();
	}

	int count()
	{
		return func_list.size();
	}

	void clear()
	{
		func_list.clear();
	}

	this_type& operator -=(func_type fun)
	{
		auto itr = std::find_if(func_list.begin(), func_list.end(),
			[&](dele_func&f)->bool
		{
			auto _fun = f.target<R(*)(Args...)>();
			if (_fun && *_fun == fun)
			{
				return true;
			}
			return false;
		});

		if (itr != func_list.end())
			func_list.erase(itr);

		return *this;
	}

	this_type& operator +=(func_type fun)
	{
		func_list.push_back(fun);
		return *this;
	}

	void operator()(Args...args)
	{
		for (auto itr = func_list.begin(); itr != func_list.end(); itr++)
		{
			try
			{
				(*itr)(args...);
			}
			catch (exception ex)
			{
				//do something...
			}
		}
	}

private:
	function_list func_list;
};


#endif // !_DELEGATE_H_
