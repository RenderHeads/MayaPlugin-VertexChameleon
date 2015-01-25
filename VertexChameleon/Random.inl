///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/* generates a random number on [0,0xffffffff]-interval */
inline unsigned int
RandomMT::RandomUInt(void)
{
	unsigned int y;

	if (--m_left == 0)
		Next_State();
	y = *m_next++;
	m_index++;

	/* Tempering */
	y ^= (y >> 11);
	y ^= (y << 7) & 0x9d2c5680UL;
	y ^= (y << 15) & 0xefc60000UL;
	y ^= (y >> 18);

	return y;
}

inline int
RandomMT::RandomInt(void)
{
	return (int)(RandomUInt()>>1);
}
inline float
RandomMT::RandomUnit(void)
{
	return float(RandomUInt())*(1.0f/4294967295.0f);	// divided by 2^32-1
}
inline float
RandomMT::RandomSignedUnit(void)
{
	return RandomUnit()*2.0f-1.0f;
}
inline bool
RandomMT::RandomBool()
{
	return RandomUInt() >= 0x80000000UL;
}
inline float
RandomMT::RandomRange(float low, float hi)
{
	return RandomUnit() * (hi - low) + low;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
