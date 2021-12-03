#ifndef __OBJECT_MANAGER_HPP__
#define __OBJECT_MANAGER_HPP__

template<typename T>
class _ObjectManager {
public:
	static T* GetRef() noexcept {
		return s_pObject;
	}

	static void CleanUp() noexcept {
		if (s_pObject) {
			delete s_pObject;
			s_pObject = nullptr;
		}
	}

	static void Set(T* pObject) {
		if (!s_pObject)
			s_pObject = pObject;
	}

private:
	static T* s_pObject;
};

template<typename T>
T* _ObjectManager<T>::s_pObject = nullptr;
#endif
