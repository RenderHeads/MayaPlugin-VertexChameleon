// Random number generator based on Mersenne Twister.
// http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html

#define MNoVersionString
#include "MayaPCH.h"
#include "Random.h"

RandomMT gRand;

#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UMASK 0x80000000UL /* most significant w-r bits */
#define LMASK 0x7fffffffUL /* least significant r bits */
#define MIXBITS(u,v) ( ((u) & UMASK) | ((v) & LMASK) )
#define TWIST(u,v) ((MIXBITS(u,v) >> 1) ^ ((v)&1UL ? MATRIX_A : 0UL))

RandomMT::RandomMT()
{
	m_left=1;
	m_initf=0;
	m_index=0;
	m_seed=5489UL;
	m_next=NULL;
}

/* initializes state[N] with a seed */
void
RandomMT::Seed(unsigned long s)
{
    int j;
	m_seed=s;
    m_state[0]= s & 0xffffffffUL;
    for (j=1; j<N; j++) {
        m_state[j] = (1812433253UL * (m_state[j-1] ^ (m_state[j-1] >> 30)) + j); 
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array state[].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */
        m_state[j] &= 0xffffffffUL;  /* for >32 bit machines */
    }
    m_left = 1; m_initf = 1;
}

void
RandomMT::Next_State(void)
{
    unsigned int *p=m_state;
    int j;

    /* if init_genrand() has not been called, */
    /* a default initial seed is used         */
    if (m_initf==0)
		Seed(5489UL);

    m_left = N;
    m_next = m_state;
	m_index = 0;
    
    for (j=N-M+1; --j; p++) 
        *p = p[M] ^ TWIST(p[0], p[1]);

    for (j=M; --j; p++) 
        *p = p[M-N] ^ TWIST(p[0], p[1]);

    *p = p[M-N] ^ TWIST(p[0], m_state[0]);
}
