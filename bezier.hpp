#pragma once
#include <vector>
#include <cmath>
#include <algorithm>
#include <functional>
#include <complex>

using namespace std;


template<typename T>
class bezier
{
public: 
	bezier(std::vector<T> init)
		: m_ctrl(init)
	{
		
	}

	void append_control(std::vector<T> l)
	{
		m_ctrl.insert(m_ctrl.end(), l.begin(), l.end());
	}

	//how do I do this, in haskell it'd be  :: T -> T 
	template<typename Functor>
	void jiggle(Functor wiggle ) 
	{
		for(auto & i : m_ctrl)
		{
			i = wiggle(i);
		}
	}
	int size() const{
		return m_ctrl.size();
	}
	
	void typesafe_jiggle(std::function<T(T)> wiggle)
	{
		for(auto & i : m_ctrl)
		{
			i = wiggle(i);
		}
	}

	static float arc_length(const bezier< complex<float> > &b, size_t i) 
	{
		float len{};
		if( i < 0 || (i + 2) > b.m_ctrl.size() ){
			return len;
		}

		float chord = sqrt(norm(b.m_ctrl[i]  - b.m_ctrl[i+2]));
		float ctrl_net = sqrt(norm(b.m_ctrl[i] - b.m_ctrl[i+1]))  + sqrt(norm(b.m_ctrl[i+2] - b.m_ctrl[i+1]));
		len += (chord+ctrl_net)/2.0f;
		return len;
	}
	T sample(float t) const
	{
		size_t start =0;

		// chain to further pairs 
		if(t < 0)
		{
			return m_ctrl.front();
		}
		if(t > 1)
		{
			double s = 0;
			t = modf( t, & s );
			start = ((size_t)s)*2;
			if(start+2 > m_ctrl.size())
			{
				return m_ctrl.back();
			}
		}
		float inv_t = (1-t);
		return inv_t*inv_t * m_ctrl[start] + 2*inv_t*t*m_ctrl[start+1] + t*t*m_ctrl[start+2];
	}
private:
	vector<T> m_ctrl;
};

