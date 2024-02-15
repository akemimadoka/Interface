#include <type_traits>

#ifndef INTERFACE_DEFINITION
#error "No interface defined!"
#endif

// 引用自 https://github.com/akemimadoka/Cafe.Core/blob/master/src/Cafe/Misc/UtilityMacros.h

#define CAFE_CONCAT_IMPL(a, b) a##b
#define CAFE_CONCAT(a, b) CAFE_CONCAT_IMPL(a, b)

#define CAFE_ID(...) __VA_ARGS__

#define CAFE_GET_HEAD(a, ...) a
#define CAFE_GET_TAIL(a, ...) __VA_ARGS__

#define CAFE_EXPAND_OR_IMPL(replacement, ...) CAFE_GET_HEAD(__VA_ARGS__ __VA_OPT__(, ) replacement)
#define CAFE_EXPAND_OR(replacement, a) CAFE_EXPAND_OR_IMPL(replacement, a)

#define CAFE_IS_EMPTY(...) CAFE_EXPAND_OR(true, __VA_OPT__(false))

#define CAFE_IF_IMPL_true(a, b) a
#define CAFE_IF_IMPL_1(a, b) a
#define CAFE_IF_IMPL_false(a, b) b
#define CAFE_IF_IMPL_0(a, b) b
#define CAFE_IF(cond, a, b) CAFE_CONCAT(CAFE_IF_IMPL_, cond)(a, b)

// 引用结束

#define PREPEND_PARAMETERS(elem, parameters)                                                       \
	CAFE_IF(CAFE_IS_EMPTY parameters, (elem), (elem, CAFE_ID parameters))

#define TRAITS_DEFINITION_BEGIN_WITH_NAME(name)                                                    \
	template <typename T>                                                                          \
	struct name##Traits
#define TRAITS_DEFINITION_MEMBER(returnType, name, parameters, ...)
#define TRAITS_DEFINITION_END ;

INTERFACE_DEFINITION(TRAITS_DEFINITION_BEGIN_WITH_NAME, TRAITS_DEFINITION_MEMBER,
                     TRAITS_DEFINITION_END)

#undef TRAITS_DEFINITION_BEGIN_WITH_NAME
#undef TRAITS_DEFINITION_MEMBER
#undef TRAITS_DEFINITION_END

#define VTABLE_LAYOUT_BEGIN_WITH_NAME(name)                                                        \
	struct name##VTable                                                                            \
	{                                                                                              \
		template <typename T>                                                                      \
		using Traits = name##Traits<T>;                                                            \
		template <typename T>                                                                      \
		static const name##VTable From;
#define VTABLE_LAYOUT_MEMBER(returnType, name, parameters, ...)                                    \
	returnType(*const name) PREPEND_PARAMETERS(__VA_ARGS__ void*, parameters);
#define VTABLE_LAYOUT_END                                                                          \
	}                                                                                              \
	;

INTERFACE_DEFINITION(VTABLE_LAYOUT_BEGIN_WITH_NAME, VTABLE_LAYOUT_MEMBER, VTABLE_LAYOUT_END)

#undef VTABLE_LAYOUT_BEGIN_WITH_NAME
#undef VTABLE_LAYOUT_MEMBER
#undef VTABLE_LAYOUT_END

#define VTABLE_DEFINITION_BEGIN_WITH_NAME(name)                                                    \
	template <typename T>                                                                          \
	constexpr name##VTable name##VTable::From                                                      \
	{
#define VTABLE_DEFINITION_MEMBER(returnType, name, parameters, ...)                                \
	.name = [](__VA_ARGS__ void* receiver, auto... args) {                                         \
		return Traits<T>::name(*static_cast<__VA_ARGS__ T*>(receiver),                             \
		                       static_cast<decltype(args)&&>(args)...);                            \
	},
#define VTABLE_DEFINITION_END                                                                      \
	}                                                                                              \
	;

INTERFACE_DEFINITION(VTABLE_DEFINITION_BEGIN_WITH_NAME, VTABLE_DEFINITION_MEMBER,
                     VTABLE_DEFINITION_END)

#undef VTABLE_DEFINITION_BEGIN_WITH_NAME
#undef VTABLE_DEFINITION_MEMBER
#undef VTABLE_DEFINITION_END

#define DYNAMIC_FORWARDER_DEFINITION_BEGIN_WITH_NAME(name)                                         \
	template <typename OpaquePointerType>                                                          \
	struct name##Traits;                                                                           \
                                                                                                   \
	template <typename OpaquePointerType = void*>                                                  \
	struct name##DynamicForwarder                                                                  \
	{                                                                                              \
	public:                                                                                        \
		template <typename T>                                                                      \
		explicit name##DynamicForwarder(T&& obj)                                                   \
		    : ptr(std::addressof(obj)), vtable{ &name##VTable::From<std::remove_cvref_t<T>> }      \
		{                                                                                          \
		}                                                                                          \
                                                                                                   \
	private:                                                                                       \
		OpaquePointerType ptr;                                                                     \
		const name##VTable* vtable;                                                                \
                                                                                                   \
		friend struct name##Traits<name##DynamicForwarder>;

#define DYNAMIC_FORWARDER_DEFINITION_MEMBER(returnType, name, parameters, ...)
#define DYNAMIC_FORWARDER_DEFINITION_END                                                           \
	}                                                                                              \
	;

INTERFACE_DEFINITION(DYNAMIC_FORWARDER_DEFINITION_BEGIN_WITH_NAME,
                     DYNAMIC_FORWARDER_DEFINITION_MEMBER, DYNAMIC_FORWARDER_DEFINITION_END)

#undef DYNAMIC_FORWARDER_DEFINITION_BEGIN_WITH_NAME
#undef DYNAMIC_FORWARDER_DEFINITION_MEMBER
#undef DYNAMIC_FORWARDER_DEFINITION_END

#define DYNAMIC_FORWARDER_TRAITS_DEFINITION_BEGIN_WITH_NAME(name)                                  \
	template <typename OpaquePointerType>                                                          \
	struct name##Traits<name##DynamicForwarder<OpaquePointerType>>                                 \
	{                                                                                              \
		using DynamicForwarder = name##DynamicForwarder<OpaquePointerType>;
#define DYNAMIC_FORWARDER_TRAITS_DEFINITION_MEMBER(returnType, name, parameters, ...)              \
	static constexpr returnType(*name)                                                             \
	    PREPEND_PARAMETERS(const volatile DynamicForwarder& self, parameters) =                    \
	        [](const volatile DynamicForwarder& self, auto... args) {                              \
		        return self.vtable->name(self.ptr, static_cast<decltype(args)&&>(args)...);        \
	        };
#define DYNAMIC_FORWARDER_TRAITS_DEFINITION_END                                                    \
	}                                                                                              \
	;

INTERFACE_DEFINITION(DYNAMIC_FORWARDER_TRAITS_DEFINITION_BEGIN_WITH_NAME,
                     DYNAMIC_FORWARDER_TRAITS_DEFINITION_MEMBER,
                     DYNAMIC_FORWARDER_TRAITS_DEFINITION_END)

#undef DYNAMIC_FORWARDER_TRAITS_DEFINITION_BEGIN_WITH_NAME
#undef DYNAMIC_FORWARDER_TRAITS_DEFINITION_MEMBER
#undef DYNAMIC_FORWARDER_TRAITS_DEFINITION_END

#define CONCEPT_DEFINITION_BEGIN_WITH_NAME(name)                                                   \
	template <typename T>                                                                          \
	concept name = requires(T& obj) { typename name##Traits<T>;
#define CONCEPT_DEFINITION_MEMBER(returnType, name, parameters,                                    \
                                  ...) // 写不下去了！原本应该检查 Traits 定义了相应的 member
#define CONCEPT_DEFINITION_END                                                                     \
	}                                                                                              \
	;

INTERFACE_DEFINITION(CONCEPT_DEFINITION_BEGIN_WITH_NAME, CONCEPT_DEFINITION_MEMBER,
                     CONCEPT_DEFINITION_END)

#undef CONCEPT_DEFINITION_BEGIN_WITH_NAME
#undef CONCEPT_DEFINITION_MEMBER
#undef CONCEPT_DEFINITION_END

#undef INTERFACE_DEFINITION
