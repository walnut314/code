#include <cstdio>
#include <cstdlib>
#include <bitset>

using namespace std;

#define N_BITS (8 * 1024 * 1024)


char *create_bvec(int n)
{
    return (char *) calloc(n, sizeof(char));
}

void set(char *address, int value)
{
    address[value/8] = address[value/8] | (1 << (value%8));
}

void clr(char *address, int value)
{
    address[value/8] = address[value/8] & ~(1 << (value%8));
}

int is_set(char *address, int value)
{
    int v = ( address[value/8] & (1 << (value%8)) );
    if (v) printf("address[%d] is set\n", value);
    else   printf("address[%d] is not set\n", value);
    return v;
}

void bvect()
{
    char *s;
    printf("testing C version\n");
    s = create_bvec(10);
    set(s, 10);
    set(s, 11);
    set(s, 18);
    clr(s, 11);
    is_set(s, 10);
    is_set(s, 5);
    is_set(s, 11);
    is_set(s, 18);
}

int main()
{
    typedef std::size_t length_t, position_t;
    bitset<N_BITS> bvec;

    //bvec.set(position_t(1), true);
    bvec[1] = 1;
    if (bvec[1] == true) printf("bvec[%d] is set\n", 1);
    else printf("bvec[%d] is clr\n", 1);
    printf("bvec bytes: %luK\n", bvec.size()/1024);

    bvect();
}

