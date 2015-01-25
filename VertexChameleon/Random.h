#ifndef RANDOM_H
#define RANDOM_H

/**
*  Random number generator based on Mersenne Twister.
*  http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
*  The test shows this to be 4 times faster than stdlib rand()
*/
class RandomMT
{
public:
	RandomMT();

	/// Initialize with a seed.
	void Seed( unsigned long s );

	unsigned int	RandomUInt();			// [0,0xffffffff]
	int				RandomInt();			// [0,0x7fffffff]
	float			RandomUnit();			// [ 0.0f,1.0f]
	float			RandomSignedUnit();		// [-1.0f,1.0f]
	bool			RandomBool();			// [false, true]
	float			RandomRange(float low, float hi);		// [low, high]

private:
	void	Next_State();

	enum { N = 624, M = 397 };	// Period parameters.
	unsigned int	m_state[N];	// The state vector.
	unsigned int*	m_next;
	int				m_index,m_seed,m_left,m_initf;
};

extern RandomMT	gRand;

#include "Random.inl"

#endif
