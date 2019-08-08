#pragma once
template<class T> class Singleton
{
private:
	static T theSingleInstance;

public:
	static T& GetInstance()
	{
		return theSingleInstance;
	}
	static T* GetInstnacePtr()
	{
		return &theSingleInstance;
	}
};
template<class T> T Singleton<T>::theSingleInstance;