// ConsoleApplication13.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <vector>
#include <functional>
#include <iostream>
#include <memory>


class CoroutineNode
{
public:
	typedef std::shared_ptr<CoroutineNode> Ptr;
	typedef std::function<Ptr()> NextDelegate;
public:
	CoroutineNode(int v, NextDelegate n):value(v), next(n) {}
public:
	int value;
	NextDelegate next = nullptr;
};

//1.每一个yield return被包装为一个函数（后面表示为Y）,Y函数接收两个函数，参数1为object类型，参数2为一个Y函数
CoroutineNode::Ptr yield(int v, CoroutineNode::NextDelegate next = nullptr)
{
	return std::make_shared<CoroutineNode>(v,next);
}

class IEnumerator 
{
	CoroutineNode::Ptr _node;
	int _current;
public:
	IEnumerator(CoroutineNode::Ptr node) :_node(node) {}
	
	int Current(){return _current;}

	//迭代器的MoveNext调用即调用了一次Y函数，该函数返回一个object，另一个Y函数或者空
	bool MoveNext()
	{
		if (_node != nullptr)
		{
			_current = _node->value;
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
		CoroutineNode::NextDelegate next = a->next;
		if (next == nullptr)
		{
			next = [=](){ return b; };
		}
		else
		{
			next = [=](){ return Combine(a->next(), b); };
		}
		return std::make_shared<CoroutineNode>(a->value, next);
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
	yield_return(1);
	yield_return(2);
	yield_return(10);
	return yield(5);
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
	yield_return(x);
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
	IEnumerator iter = coroutine2();
	while (iter.MoveNext())
	{
		std::cout << iter.Current() << std::endl;
	}
    return 0;
}

