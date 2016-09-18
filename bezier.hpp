#include <vector>
#include <cmath>
#include <algorithm>
#include <functional>
using namespace std;


template<typename T>
class bezier
{
public: 
	bezier(std::vector<T>	init)
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
	int length(){
		return m_ctrl.size();
	}
	void typesafe_jiggle(std::function<T(T)> wiggle)
	{
		for(auto & i : m_ctrl)
		{
			i = wiggle(i);
		}
	}

	T sample(float t) const
	{
		size_t start =0;

		// chain to further pairs 
		if(t > 1)
		{
			double s = 0;
			t = modf( t, & s );
			start = ((size_t)s)*2;
			if(start+2 > m_ctrl.size())
			{
				return T{};
			}
		}
		float inv_t = (1-t);
		return inv_t*inv_t * m_ctrl[start] + 2*inv_t*t*m_ctrl[start+1] + t*t*m_ctrl[start+2];
	}
private:
	vector<T> m_ctrl;
};