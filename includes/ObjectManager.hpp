#ifndef __OBJECT_MANAGER_HPP__
#define __OBJECT_MANAGER_HPP__

template<class Type, class Specialization>
class _ObjectManager {
public:
	static Type* GetRef() noexcept {
		return s_pObject;
	}

	static void CleanUp() noexcept {
		if (s_pObject) {
			delete s_pObject;
			s_pObject = nullptr;
		}
	}

	static void Set(Type* pObject) {
		if (!s_pObject)
			s_pObject = pObject;
	}

private:
	static Type* s_pObject;
};

template<class Type, class Specialization>
Type* _ObjectManager<Type, Specialization>::s_pObject = nullptr;
#endif
