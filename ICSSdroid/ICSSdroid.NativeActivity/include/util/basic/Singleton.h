#pragma once

/**
*  @brief  シングルトンホルダー
*/
namespace ICSS {
	template <class _Ty, class... _Args>
	class Singleton
	{
	public:
		typedef _Ty InstanceType;

	public:
		/// 生成
		static void create(_Args... args)
		{
			if (_instance == NULL) {
				_instance = new InstanceType(args...);
			}
		}
		/// 破棄
		static void destroy()
		{
			if (_instance != NULL) {
				delete _instance;
				_instance = NULL;
			}
		}

	#if 1
		/// インスタンスゲット
		static InstanceType& getInstance()
		{
			return *_instance;
		}
	#else
		/// インスタンスゲット
		static InstanceType* getInstance()
		{
			return _instance;
		}
	#endif
		/// インスタンス参照ゲット
		static InstanceType& getInstanceRef()
		{
			return *_instance;
		}

		/// インスタンスポインタゲット
		static InstanceType* getInstancePtr()
		{
			return _instance;
		}

		/// 生成済み??
		static bool isCreate() { return _instance != NULL; };
		/// 破棄済み??
		static bool isDestroy() { return _instance == NULL; };

	private:
		static InstanceType* _instance; //!< インスタンス

	private:
		// 禁止
		Singleton() = delete;
		virtual ~Singleton() = delete;

		Singleton(const Singleton&) = delete;
		Singleton& operator=(const Singleton&) = delete;
		Singleton(const Singleton&&) = delete;
		Singleton& operator=(const Singleton&&) = delete;
	};

	template <class _Ty, class... _Args>
	typename Singleton<_Ty, _Args...>::InstanceType* Singleton<_Ty, _Args...>::_instance = NULL;

}