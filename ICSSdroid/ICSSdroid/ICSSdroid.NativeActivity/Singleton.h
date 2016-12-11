#pragma once

/**
*  @brief  �V���O���g���z���_�[
*/
namespace ICSS {
	template <class _Ty>
	class Singleton
	{
	public:
		typedef _Ty InstanceType;

	public:
		/// ����
		static void create()
		{
			if (_instance == NULL) {
				_instance = new InstanceType();
			}
		}
		/// �j��
		static void destroy()
		{
			if (_instance != NULL) {
				delete _instance;
				_instance = NULL;
			}
		}

	#if 1
		/// �C���X�^���X�Q�b�g
		static InstanceType& getInstance()
		{
			return *_instance;
		}
	#else
		/// �C���X�^���X�Q�b�g
		static InstanceType* getInstance()
		{
			return _instance;
		}
	#endif
		/// �C���X�^���X�Q�ƃQ�b�g
		static InstanceType& getInstanceRef()
		{
			return *_instance;
		}

		/// �C���X�^���X�|�C���^�Q�b�g
		static InstanceType* getInstancePtr()
		{
			return _instance;
		}

		/// �����ς�??
		static bool isCreate() { return _instance != NULL; };
		/// �j���ς�??
		static bool isDestroy() { return _instance == NULL; };

	private:
		static InstanceType* _instance; //!< �C���X�^���X

	private:
		// �֎~
		Singleton() = delete;
		virtual ~Singleton() = delete;

		Singleton(const Singleton&) = delete;
		Singleton& operator=(const Singleton&) = delete;
	};

	template <class _Ty>
	typename Singleton<_Ty>::InstanceType* Singleton<_Ty>::_instance = NULL;

}