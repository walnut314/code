#include <cstdio>
#include <bitset>

using namespace std;

#define N_BITS (8 * 1024 * 1024)

int main()
{
    typedef std::size_t length_t, position_t;
    bitset<N_BITS> bvec;

    //bvec.set(position_t(1), true);
    bvec[1] = 1;
    if (bvec[1] == true) printf("bvec[%d] is set\n", 1);
    else printf("bvec[%d] is clr\n", 1);
    printf("bvec bytes: %luK\n", bvec.size()/1024);
}

