#include <vector>
#include <cmath>
#include <algorithm>
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
	template<typename Func>
	void jiggle(Func wiggle ) 
	{
		transform( m_ctrl.begin(), m_ctrl.end(), m_ctrl.begin(), wiggle );
	}

	T sample(float t)
	{
		size_t start =0;

		// chain to further pairs 
		if(t > 1)
		{
			double s = 0;
			t = modf( t, & s );
			start = ((size_t)s)*2;
			if(start > m_ctrl.size())
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