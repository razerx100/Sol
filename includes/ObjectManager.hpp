#ifndef OBJECT_MANAGER_HPP_
#define	OBJECT_MANAGER_HPP_
#include <vector>
#include <memory>
#include <functional>
#include <ranges>
#include <algorithm>
#include <concepts>

template<typename T>
concept HasArgs = requires() {
    typename T::Args;
};

template<typename T>
concept HasNoArgs = !HasArgs<T>;

template <class _Base, class _Derived>
concept Base_of = std::derived_from<_Derived, _Base>;

class ObjectManager {
    struct Priority {
        std::function<void()> destructor;
        std::uint32_t priority;
    };

public:
    ObjectManager() noexcept : m_cleanedUp{ false } {}
    ~ObjectManager() noexcept {
        if (!m_cleanedUp)
            StartCleanUp();
    }

    template<HasNoArgs T, Base_of<T> G>
    void CreateObject(std::unique_ptr<G>& emptyPtr, std::uint32_t priorityLevel) {
        emptyPtr = std::make_unique<T>();

        _createDestructor(emptyPtr, priorityLevel);
    }
    template<HasNoArgs T, Base_of<T> G>
    void CreateObject(std::shared_ptr<G>& emptyPtr, std::uint32_t priorityLevel) {
        emptyPtr = std::make_shared<T>();

        _createDestructor(emptyPtr, priorityLevel);
    }
    template<HasArgs T, Base_of<T> G>
    void CreateObject(
        std::unique_ptr<G>& emptyPtr, typename T::Args arguments, std::uint32_t priorityLevel
    ) {
        emptyPtr = std::make_unique<T>(arguments);

        _createDestructor(emptyPtr, priorityLevel);
    }
    template<HasArgs T, Base_of<T> G>
    void CreateObject(
        std::shared_ptr<G>& emptyPtr, typename T::Args arguments, std::uint32_t priorityLevel
    ) {
        emptyPtr = std::make_shared<T>(arguments);

        _createDestructor(emptyPtr, priorityLevel);
    }
    template<typename T, std::derived_from<T> G>
    void CreateObject(
        std::unique_ptr<T>& emptyPtr, std::unique_ptr<G> inputPtr, std::uint32_t priorityLevel
    ) {
        emptyPtr = std::move(inputPtr);

        _createDestructor(emptyPtr, priorityLevel);
    }
    template<typename T, std::derived_from<T> G>
    void CreateObject(
        std::shared_ptr<T>& emptyPtr, std::shared_ptr<G> inputPtr, std::uint32_t priorityLevel
    ) {
        emptyPtr = std::move(inputPtr);

        _createDestructor(emptyPtr, priorityLevel);
    }
    template<typename T, std::derived_from<T> G>
    void CreateObject(
        std::unique_ptr<T>& emptyPtr, G* heapAllocatedMemory, std::uint32_t priorityLevel
    ) noexcept {
        emptyPtr = std::unique_ptr<T>(heapAllocatedMemory);

        _createDestructor(emptyPtr, priorityLevel);
    }
    template<typename T, std::derived_from<T> G>
    void CreateObject(
        std::shared_ptr<T>& emptyPtr, G* heapAllocatedMemory, std::uint32_t priorityLevel
    ) {
        emptyPtr = std::shared_ptr<T>(heapAllocatedMemory);

        _createDestructor(emptyPtr, priorityLevel);
    }
    template<HasArgs T>
    void CreateObject(
        std::unique_ptr<T>& emptyPtr, typename T::Args arguments, std::uint32_t priorityLevel
    ) {
        emptyPtr = std::make_unique<T>(arguments);

        _createDestructor(emptyPtr, priorityLevel);
    }
    template<HasArgs T>
    void CreateObject(
        std::shared_ptr<T>& emptyPtr, typename T::Args arguments, std::uint32_t priorityLevel
    ) {
        emptyPtr = std::make_shared<T>(arguments);

        _createDestructor(emptyPtr, priorityLevel);
    }
    template<HasNoArgs T>
    void CreateObject(std::unique_ptr<T>& emptyPtr, std::uint32_t priorityLevel) {
        emptyPtr = std::make_unique<T>();

        _createDestructor(emptyPtr, priorityLevel);
    }
    template<HasNoArgs T>
    void CreateObject(std::shared_ptr<T>& emptyPtr, std::uint32_t priorityLevel) {
        emptyPtr = std::make_shared<T>();

        _createDestructor(emptyPtr, priorityLevel);
    }

    void StartCleanUp() noexcept {
        std::ranges::sort(
            m_destructionPriorities,
            [](const Priority& p1, const Priority& p2) {return p1.priority < p2.priority; }
        );

        for (auto& priority : m_destructionPriorities)
            priority.destructor();

        m_cleanedUp = true;
    }

private:
    template<typename T>
    void _createDestructor(T& emptyPtr, std::uint32_t priorityLevel) noexcept {
        Priority priority{
            .destructor = std::function<void()>([&] {emptyPtr.reset(); }),
            .priority = priorityLevel
        };
        m_destructionPriorities.emplace_back(priority);
    }

private:
    std::vector<Priority> m_destructionPriorities;
    bool m_cleanedUp;
};
#endif
