
//

#include "stdafx.h"
#include <vector>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

class CoroutineNode
{
public:
	typedef std::shared_ptr<CoroutineNode> Ptr;
	typedef std::function<Ptr()> NextDelegate;
public:
	CoroutineNode(NextDelegate n): next(n) {}
	virtual ~CoroutineNode() {}
public:
	NextDelegate next = nullptr;
};
template<typename T>
class CoroutineNodeImp : public CoroutineNode
{
public:
	CoroutineNodeImp(T v, NextDelegate n)
		:CoroutineNode(n)
		, value(v) {}
	T value;

};

//1.每一个yield return被包装为一个函数（后面表示为Y）,Y函数接收两个函数，参数1为object类型，参数2为一个Y函数
template<typename T>
CoroutineNode::Ptr yield(T v, CoroutineNode::NextDelegate next = nullptr)
{
	return CoroutineNode::Ptr(std::make_shared<CoroutineNodeImp<T>>(v,next));
}

class IEnumerator 
{
	CoroutineNode::Ptr _node;
	CoroutineNode::Ptr _current;
public:
	IEnumerator(CoroutineNode::Ptr node) :_node(node) {}
	template<typename T>
	T Current(){
		auto subPtr = std::dynamic_pointer_cast<CoroutineNodeImp<T>>(_current);
		return subPtr->value;
	}
	template<typename T>
	bool currentIs() {
		return std::dynamic_pointer_cast<CoroutineNodeImp<T>>(_current) != nullptr;
	}

	//迭代器的MoveNext调用即调用了一次Y函数，该函数返回一个object，另一个Y函数或者空
	bool MoveNext()
	{
		if (_node != nullptr)
		{
			_current = _node;
			_node = _node->next != nullptr ? _node->next() : nullptr;
			return true;
		}
		//如果返回的Y函数为空，则迭代结束
		return false;
	}
};
CoroutineNode::Ptr Combine(CoroutineNode::Ptr a, CoroutineNode::Ptr b)
{
	if (a == nullptr)
	{
		return b;
	}
	else if (b == nullptr)
	{
		return a;
	}
	else
	{
		if (a->next == nullptr)
		{
			a->next = [=](){ return b; };
		}
		else
		{
			auto next = a->next;
			a->next = [=](){ return Combine(next(), b); };
		}
		return a;
	}
}
typedef std::function<CoroutineNode::Ptr(int)> YieldDelegate;
CoroutineNode::Ptr For(int min, int max, YieldDelegate y)
{
	CoroutineNode::Ptr node = nullptr;
	for (int i = min; i < max; ++i)
	{
		node = Combine(node, y(i));
	}
	return node;
}


#define  yield_return(x) return yield(x, [=]() {

IEnumerator coroutine()
{
	yield_return(std::string("hello"));
	yield_return(2);
	yield_return(10);
	yield_return(std::string("wolrd"));
	return yield(5);
	});	
	});
	});
	});
}
IEnumerator coroutine1()
{
	return For(0, 10, [](int i) ->CoroutineNode ::Ptr{
		if (i < 3)
		{
			return yield(i);
		}
		else
		{
			return nullptr;
		}
	});
}
#define  _for(min,max,i) return For(min,max,[](int i)
IEnumerator coroutine3()
{
	_for(0, 10, i){
		return yield(i);
	});
}

IEnumerator coroutine2()
{
	int x = 10;
	yield_return(std::string("hello"));
	int y = x + 100;
	yield_return(y);
	int z = y + 1100;
	yield_return(z);
	return Combine(
		For(0, 5, [](int i) {return yield(5); })
		,
		For(2, 10, [](int i) {
			return For(0, 3, [=](int j) {
				return yield(i + j);
			});
		})
	);
	});
	});
	});
}
int main()
{
	IEnumerator iter = coroutine();
	while (iter.MoveNext())
	{
		if (iter.currentIs<std::string>()) {
			std::cout << iter.Current<std::string>() << std::endl;
		}
		else if (iter.currentIs<int>())
		{
			std::cout << iter.Current<int>() << std::endl;
		}
	}
    return 0;
}

