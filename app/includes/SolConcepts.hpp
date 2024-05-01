#ifndef SOL_CONCEPTS_HPP_
#define SOL_CONCEPTS_HPP_
#include <concepts>

template<typename T>
concept HasArgs = requires() {
    typename T::Args;
};

template<typename T>
concept HasNoArgs = !HasArgs<T>;

template <class _Base, class _Derived>
concept Base_of = std::derived_from<_Derived, _Base>;

template <class _Derived, class _Base>
concept DerivedWithArgs = HasArgs<_Derived> && std::derived_from<_Derived, _Base>;

template <class _Derived, class _Base>
concept DerivedWithoutArgs = HasNoArgs<_Derived> && std::derived_from<_Derived, _Base>;
#endif
