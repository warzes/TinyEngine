#pragma once

class SingletonInterface
{
public:
	virtual ~SingletonInterface() = default;
};

struct SingletonData final
{
	std::string name;
	std::unique_ptr<SingletonInterface> ptr;
};

class SingletonContainer
{
public:
	static void RegisterSingleton(const std::string&, std::unique_ptr<SingletonInterface>);
	static void DestroyAllSingletons();

private:
	SingletonContainer() = default;
	~SingletonContainer() = default;

	static std::vector<SingletonData> m_singletonsVector;
};

// Base class for singleton (no thread-safe)
template<class T> 
class Singleton : public SingletonInterface 
{
public:
	static T& Instance()
	{
		if (!m_objectT)
		{
			auto newObjectT = std::unique_ptr<T>(new T);
			m_objectT = newObjectT.get();
			SingletonContainer::RegisterSingleton(typeid(T).name(), std::move(newObjectT));
		}

		return *m_objectT;
	}
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	~Singleton() override { m_objectT = nullptr; }

protected:
	Singleton() = default;

private:
	static T* m_objectT = nullptr;
};

// Base class for thread-safe singleton
template<class T> 
class ThreadSafeSingleton : public SingletonInterface 
{
public:
	static T& Instance()
	{
		T* singletonInstance = objectT.load(std::memory_order_acquire);
		if (!singletonInstance) [[unlikely]] {
				std::scoped_lock<std::mutex> lock(mutexInstanceCreation);
				singletonInstance = objectT.load(std::memory_order_relaxed);
				if (!singletonInstance) {
					auto newObjectT = std::unique_ptr<T>(new T);
					singletonInstance = newObjectT.get();
					SingletonContainer::registerSingleton(typeid(T).name(), std::move(newObjectT));
					objectT.store(singletonInstance, std::memory_order_release);
				}
			}

		return *singletonInstance;
	}
	ThreadSafeSingleton(const ThreadSafeSingleton&) = delete;
	ThreadSafeSingleton& operator=(const ThreadSafeSingleton&) = delete;
	~ThreadSafeSingleton() override { m_objectT.store(nullptr, std::memory_order_release); }

protected:
	ThreadSafeSingleton() = default;

private:
	static std::atomic<T*> m_objectT;
	static std::mutex m_mutexInstanceCreation;
};

template<class T> std::atomic<T*> ThreadSafeSingleton<T>::m_objectT = nullptr;
template<class T> std::mutex ThreadSafeSingleton<T>::m_mutexInstanceCreation;