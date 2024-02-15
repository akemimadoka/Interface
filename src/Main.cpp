#include <iostream>

#define INTERFACE_DEFINITION(BEGIN_WITH_NAME, MEMBER, END)                                         \
	BEGIN_WITH_NAME(Interface)                                                                     \
	MEMBER(int, Add, (int a, int b))                                                               \
	MEMBER(int, GetValue, (), const)                                                               \
	END
#include "DefineInterface.h"

struct TestInterface
{
	int Value;
};

template <>
struct InterfaceTraits<TestInterface>
{
	static constexpr int Add(TestInterface& obj, int a, int b)
	{
		return obj.Value + a + b;
	}

	static constexpr int GetValue(const TestInterface& obj)
	{
		return obj.Value;
	}
};

struct TestInterface2
{
};

template <>
struct InterfaceTraits<TestInterface2>
{
	static constexpr int Add(TestInterface2& obj, int a, int b)
	{
		return a + b;
	}

	static constexpr int GetValue(const TestInterface2& obj)
	{
		return 233;
	}
};

int TestAdd(Interface auto&& obj)
{
	return InterfaceTraits<std::remove_cvref_t<decltype(obj)>>::Add(obj, 1, 2);
}

int main()
{
	TestInterface test{ .Value = 123 };
	std::cout << InterfaceTraits<TestInterface>::Add(test, 1, 2) << std::endl
	          << InterfaceTraits<TestInterface>::GetValue(test) << std::endl;

	TestInterface2 test2{};
	std::cout << InterfaceTraits<TestInterface2>::Add(test2, 1, 2) << std::endl
	          << InterfaceTraits<TestInterface2>::GetValue(test2) << std::endl;

	std::cout << TestAdd(test) << std::endl << TestAdd(test2) << std::endl;

	const InterfaceDynamicForwarder<> dynTestArray[]{ InterfaceDynamicForwarder(test),
		                                              InterfaceDynamicForwarder(test2) };
	for (const auto& dynObj : dynTestArray)
	{
		std::cout << InterfaceTraits<InterfaceDynamicForwarder<>>::Add(dynObj, 1, 2) << std::endl
		          << InterfaceTraits<InterfaceDynamicForwarder<>>::GetValue(dynObj) << std::endl;
	}
}
