#pragma once

#include <math.h>

template <class T>
class Animation
{
public:
	Animation() {
		m_Start = 0;
		m_End = 0;
		m_Duration = 0;
	}
	Animation(T start, T end, T duration) :
		m_Start(start),
		m_End(end),
		m_Duration(duration)
	{
	}

	void SetStart(T start)
	{
		m_Start = start;
	}

	T GetStart()
	{
		return m_Start;
	}

	void SetEnd(T end)
	{
		m_End = end;
	}

	T GetEnd()
	{
		return m_End;
	}

	void SetDuration(T duration)
	{
		m_Duration = max(0, duration);
	}

	T GetDuration()
	{
		return m_Duration;
	}

	T GetValue(T time)
	{
		time = min(max(time, 0), m_Duration);
		return ComputeValue(time);
	}

protected:
	virtual T ComputeValue(T time) = 0;

	T m_Duration;
	T m_Start;
	T m_End;
};

/*template <class T>
class LinearAnimation : public Animation<T>
{
public:
	LinearAnimation(T start = 0, T end = 0, T duration = 0) :
		Animation(start, end, duration)
	{
	}
protected:
	virtual T ComputeValue(T time)
	{
		return this->m_Start + ((this->m_End - this->m_Start) * (time / this->m_Duration));
	}
};
template <class T>
class EaseInExponentialAnimation : public Animation<T>
{
public:
	EaseInExponentialAnimation(T start = 0, T end = 0, T duration = 0) : Animation(start, end, duration)
	{
	}
protected:
	T ComputeValue(T time)
	{
		return this->m_Start + (this->m_End - this->m_Start) * pow(2, 10 * (time / this->m_Duration - 1));
	}
};

template <class T>
class EaseOutExponentialAnimation : public Animation<T>
{
public:
	EaseOutExponentialAnimation(T start = 0, T end = 0, T duration = 0) :
		Animation(start, end, duration)
	{
	}
protected:
	T ComputeValue(T time)
	{
		return this->m_Start + (this->m_End - this->m_Start) * (-pow(2, -10 * time / this->m_Duration) + 1);
	}
};*/

template <class T>
class EaseInOutExponentialAnimation : public Animation<T>
{
public:
	EaseInOutExponentialAnimation() :
		Animation(0,0,0) {
	}
	EaseInOutExponentialAnimation(T start, T end, T duration) 
	{
		this->m_Start = start;
		this->m_End = end;
		this->m_Duration = duration;
	}
protected:
	T ComputeValue(T time)
	{
		//compute the current time relative to the midpoint
		time = time / (this->m_Duration / 2);
		//if we haven't reached the midpoint, we want to do the ease-in portion
		if (time < 1)
		{
			return (T)(this->m_Start + (this->m_End - this->m_Start)/2 * (-pow(2, -7 * (time-0.2)) + 1));
		}
		//otherwise, do the ease-out portion
		return (T)(this->m_Start + (this->m_End - this->m_Start)/2 * (pow(2, 7 * (time-2)) + 1));
	}
};