#pragma once

namespace util {

	class NoCopyable {

	protected:

		NoCopyable() noexcept = default;
		virtual ~NoCopyable() noexcept = default;

	public:

		NoCopyable(const NoCopyable&) = delete;
		NoCopyable& operator=(const NoCopyable&) = delete;
	};

	class NoMoveable {

	protected:

		NoMoveable() noexcept = default;
		virtual ~NoMoveable() noexcept = default;

	public:

		NoMoveable(NoMoveable&&) = delete;
		NoMoveable& operator=(NoMoveable&&) = delete;
	};

	class NoPointer {

	protected:

		NoPointer() noexcept = default;
		virtual ~NoPointer() noexcept = default;

	public:

		void* operator new(std::size_t) = delete;
		void* operator new[](std::size_t) = delete;

		void operator delete(void*) = delete;
		void operator delete[](void*) = delete;
	};

	template<typename T>
	class Singleton :
		public NoCopyable,
		public NoMoveable,
		public NoPointer {

	public:

		[[nodiscard]] static T& GetSingleton() noexcept {
			static T instance;
			return instance;
		}

	protected:

		Singleton() noexcept = default;
		~Singleton() noexcept = default;
	};
}