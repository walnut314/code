#include <stdio.h>
#include <string.h>
#include <ctype.h> // isspace
#include <stdlib.h>
#include "utils.h"
#include "types.h"

void show(int a[], int n)
{
    int i;
    for (i = 0 ; i < n; i++) {
        printf("%3d", a[i]);
    }
    printf("\n\n\n ");
}

uint64_t bitsort(int *a, int len)
{
    int i;
    uint64_t x = 0;
    for (i = 0; i < len; i++) {
        x |= (1 << a[i]);
    }
    return x;
}

int count_bits(uint32_t x)
{
    int count = 0;
    while (x) {
        count++;
        x &= (x-1);
    }
    return count;
}

int parity(uint32_t x)
{
    int count = count_bits(x);
    //return (count % 2) ? 0 : 1; // even parity
    return (count & 0x01); // odd parity
}

void test_bits()
{
    int i;
    int x = 0x5a;
    printf("count:  %x => %d\n", x, count_bits(x));
    printf("parity: %x => %d\n", x, parity(x));

    int a[] = {4,3,1,2,7,31};
    int len = sizeof(a) / sizeof(a[0]);
    show(a, len);
    uint32_t y = bitsort(a, len);
    printf("y: %08x\n", y);
    for (i = 0; i < 64; i++) {
        if (y & 1) {
            printf("%d\n", i);
        }
        y >>= 1;
    }
}

void sequencer()
{
// https://www.nxp.com/docs/en/application-note/AN4073.pdf    
    int i, n;
    for (i = 0; i < 32; i++) {
        n = 9 * i;
        printf("x[%d] = (%d, %d, %d)\n", i, n  , n+1, n+2);
        printf("y[%d] = (%d, %d, %d)\n", i, n+3, n+4, n+5);
        printf("z[%d] = (%d, %d, %d)\n", i, n+6, n+7, n+8);
    }
    for (i = 0; i < 32; i++) {
        n = 3 * i;
        printf("x[%d] = (%d)\n", i, n  );
        printf("y[%d] = (%d)\n", i, n+1);
        printf("z[%d] = (%d)\n", i, n+2);
    }
}

void fifo2xyz(int *samples, int len)
{
    // 32 sample fifo, 3 bytes per sample, 1 byte per axis, 96 bytes
    int n_samples = len / 24;
    int x[32], y[32], z[32];
    int i, n;
    for (i = 0; i < n_samples; i++) {
        n = 3 * i;
        x[i] = samples[n];
        y[i] = samples[n+1];
        z[i] = samples[n+2];
    }
}

void subsets()
{
    int i, j;
    int arr[] = {3,6,7,1,5,4};
    int n = sizeof(arr) / sizeof(arr[0]);

    for (i = 0; i < (1<<(n)); i++) {
        for (j = 0; j < n; j++) {
            if (i&(1<<j)) {
                printf("%d, ", arr[j]);
            }
        }
        printf("\n");
    }
}

void read_quoted_string(char *s)
{
    int i = 0;
    char c;
    while ((c = getchar()) != '\"') ;
    while ((c = getchar()) != '\"') {
        s[i] = c;
        i++;
    }
    s[i] = '\0';
}

int split(char *line, char *words[])
{
    size_t i = 0, len, j, iwords = 0;
    len = strlen(line);
    while (i != len) {
        while (i != len && (line[i] == '\0' || isspace(line[i])))
            ++i;
        j = i;
        while (j != len && !isspace(line[j]))
            ++j;
        if (i != j) {
            words[iwords++] = &line[i];
            line[j] = '\0'; // needed to account for this above!!
            i = j;
        }
    }
    return iwords;
}

int boyer(char *haystack, char *needle, int start)
{
    int hlen = strlen(haystack);
    int nlen = strlen(needle);
    int jump, offset = start;
    int scan = 0;
    int last = nlen - 1;
    int skip[256] = {0};

    if( nlen <= 0 || hlen <= 0 )
        return -1;
    
    for(scan = 0; scan < last; scan++) {
        skip[ needle[ scan ] ] = last - scan;
    }

    while( hlen >= nlen ) {
        for( scan = last; haystack[ offset + scan ] == needle[ scan ]; scan-- ) {
            if( scan == 0 ) { return offset; }
        }
        jump = skip[ haystack[ offset + last ] ];
        jump = (jump != 0) ? jump : nlen;
        hlen -= jump;
        offset += jump;
    }

    return -1;
}

int match(char *string, char *pattern)
/*
 * return position of the 1st occurence of pattern p
 * in the string, and -1 if not found.
 */
{
    int i, j;
    int plen, tlen;

    plen = strlen(pattern);
    tlen = strlen(string);

    for (i = 0; i <= (tlen-plen); ++i) {
        j = 0;
        while ((j < plen) && (string[i+j] == pattern[j]))
            ++j;
        if (j == plen) return (i);
    }
    return (-1);
}

int palindrome(char *s)
{
    int len = strlen(s);
    int n = len-1;
    int i;
    for (i = 0; i < len/2; i++, n--) {
        if (s[i] != s[n]) {
            return 0;
        }
    }
    return 1;
}

int anagram(char *s1, int len1, char *s2, int len2)
{
    int ascii[256];
    int i;
    if (len1 != len2) {
        return 0;
    }
    for (i = 0; i < 256; i++) ascii[i] = 0;
    for (i = 0; i < len1; i++) ascii[s1[i]]++;
    for (i = 0; i < len1; i++) {
        if (ascii[s2[i]]-- == 0) {
            return 0;
        }
    }
    for (i = 0; i < 256; i++) {
        if (ascii[i] != 0) {
            return 0;
        }
    }
    return 1;
}

int sequential_search(char *s, int n, char k)
{
    int i = 0;
    while (i < n && s[i] != k) i++;
    if (i < n) return i;
    return -1;
}

int binsearch(int *s, int n, char k)
{
/*
 * binary search of array s of size n
 * for the value k.
 */
    int low, high, mid;
    low = 0;
    high = n-1;
    while (low <= high) {
        printf("hello, binsearch: low %d, hi %d\n", low, high);
        mid = (low+high)/2;
        printf("is it %d @ %d\n", s[mid], mid);
        if (k > s[mid]) low = mid + 1;
        else if (k < s[mid]) high = mid - 1;
        else return k;
    }
    return -1;
}

int find_first_k(int x, int v[], int n)
/*
 * similar to binsearch
 */
{
    int low, high, mid;
    low = 0;
    high = n-1;
    while (low <= high) {
        mid = (low+high)/2;
        if (x < v[mid])
            high = mid - 1;
        else if (x > v[mid])
            low = mid + 1;
        else {/* found match */
            int first_k = mid;
            while (v[first_k] == x) first_k--;
            return first_k+1;
        }
    }
    return -1; /* no match */
}

int partition(int *A, int lo, int hi) {
    int i, p, firsthigh;
    p = hi; /* pick a partition (=hi) */
    firsthigh = lo; /* firsthigh is the partition boundary */
    for (i = lo; i < hi; ++i) {
        if (A[i] < A[p]) {
            exchg(A[i], A[firsthigh]);
            firsthigh++;
        }
    }
    exchg(A[p], A[firsthigh]);
    return firsthigh;
}

void seasort_reverse(int a[], int n)
/* O(n)
 */
{
    int i, j = n - 1;
    for (i = 0 ; i < n/2; i++, j--) {
        exchg2(a[i], a[j]);
    }
}

void quicksort(int *A, int lo, int hi) {
    int p;
    if (lo < hi) {
        p = partition(A, lo, hi);
        quicksort(A, lo, p-1);
        quicksort(A, p+1, hi);
    }
}

/* creates new list, in-place, starting at
 * zero, by swapping min for current index.
 * O(n^2)
 */
// find smallest and exchg with advancing first position
void selection(int *a, int len)
{
    int i, j;
    for (i = 0; i < len; i++) {
        for (j = i; j < len; j++) {
            if (a[i] > a[j]) exchg(a[i], a[j]);
        }
    }
}


// runs in O(n+k)
void counting_sort()
{
    // note: uses 1-based arrays, and no zeros in sorting array.
    int k = 4;
    int n = 5;
    int A[] = {0,4,1,3,4,3};
    int B[n+1];
    int C[k+1]; // no zero in array to be sorted
    int i;

    for (i = 0; i <= k; i++) B[i] = C[i] = 0;
    for (i = 1; i <= n; i++) C[A[i]]++;
    show(C, k+1);
    for (i = 2; i <= k; i++) C[i] += C[i-1]; // prefix sums
    show(C, k+1);
    for (i = n; i >= 1; i--) {
        B[C[A[i]]] = A[i];
        C[A[i]]--;
    }
    show(B, n+1);
}

int digit(int n, int k)
{
   int p = pow(10, k-1);
   int x = n/p;
   int d = x % 10;
   printf("%d digit of %d: p:%d, x:%d is %d\n", k, n, p, x, d);
   return d;
}

void radix_sort()
{
    int k = 9; // max digit
    int n = 7;
    int A[] = {0,329,457,657,839,436,721,355};
    int C[k+1]; 
    int B[n+1]; 
    int num_digits = 3;
    int i, w;
    int aux[k+1];
    for (i = 0; i <= n; i++) B[i] = 0;
    for (w = 1; w <= num_digits; w++) {
        for (i = 0; i <= k; i++) C[i] = 0;
        for (i = 1; i <= n; i++) C[digit(A[i], w)]++;
        show(C, k+1);
        for (i = 2; i <= k; i++) C[i] += C[i-1];
        show(C, k+1);
        for (i = n; i >= 1; i--) {
            B[C[digit(A[i], w)]] = A[i];
            C[digit(A[i], w)]--;
        }
        show(B, n+1);
    }
    show(B, n+1);
}

// puts item in correct location each iteration
void insertion(int a[], int n)
{
    int c, d, temp;
    for (c = 1 ; c <= n - 1; c++) { 
        d = c;
        while (d > 0 && a[d] < a[d-1]) { 
            exchg(a[d], a[d-1]);
            d--; 
        }
    }
}

// each largest element bubbles down to end
void bubble(int *a, int len)
{
    int i, j;
    for (i = 0; i < len; i++) {
        int sorted = 1; // short circuit
        for (j = 0; j < len-i-1; j++) { // move the bottom
            if (a[j] > a[j+1]) {
                sorted = 0;
                exchg(a[j], a[j+1]);
            }
        }
        if (sorted) { // no swaps
            printf("jump out: %d\n", i);
            break;
        }
    }
}

void sort_interval(int a[], int sz, int start, int inc) {
/*
 * insertion into ordered list
 * p is the partition between the ordered
 * upper portion and the unordered lower
 * portion of the list. as new items are 
 * encountered, a reverse scan is performed 
 * from the point of the new item to the 
 * beginning of the list to maintain the 
 * invariant of an ordered upper portion.
 */
    int p, i;
    for (i = start; i < sz; i+=inc) {
        for (p = i; p-inc >= 0; p-=inc) {
            if (a[p] < a[p-inc]) {
                exchg(a[p], a[p-inc]);
            }
        }
    }
}

void shell(int a[], int sz) {
    int increment, start, i;
    increment = sz;
    do {
        increment = increment/3 + 1;
        for (start = 0; start < increment; ++start) {
            sort_interval(a, sz, start, increment);
        }   
    } while (increment > 1);
}

// Merges two subarrays of arr[].
// First subarray is arr[l..m]
// Second subarray is arr[m+1..r]
void ss_merge(int arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;
 
    /* create temp arrays */
    int L[n1], R[n2];
 
    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++) L[i] = arr[l + i];
    for (j = 0; j < n2; j++) R[j] = arr[m + 1+ j];
 
    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
    /* Copy the remaining elements of L[], if there
       are any */
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }
 
    /* Copy the remaining elements of R[], if there
       are any */
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void merge_sort(int arr[], int l, int r)
{
    if (l < r) {
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
        int m = l+(r-l)/2; // partition
 
        // Sort first and second halves
        merge_sort(arr, l, m);
        merge_sort(arr, m+1, r);
 
        ss_merge(arr, l, m, r);
    }
}

// iterative
int *zipi(int a[], int b[], int len)
{
    int i;
    int *c = calloc(2*len, sizeof(int));
    for (i = 0; i < len; i++) {
        c[2*i]   = a[i];
        c[2*i+1] = b[i];
    }
    return c;
}

// recursive
int z[256] = {0};
void zipr(int a[], int b[], int i, int len)
{
    if (len == 0) return;
    z[2*i] = a[i];
    z[2*i+1] = b[i];
    zipr(a, b, i+1, len-1);
}


void test_zip()
{
    int a[] = {1, 3, 5, 7};
    int b[] = {2, 4, 6, 8};
    int *c;
    int len = 4;
    c = zipi(a, b, len);
    show(c, 2*len);

    printf("------------- now recursive\n");
    zipr(a, b, 0, len);
    show(z, 2*len);
    
}

void test_strings()
{

    int f;
    char data[]  = "sledbobber\0";
    char data2[] = "sleddels";
    char data3[] = "abbcz";
    char data4[] = "ycabb";
    char patt[]  = "bbe";
    char sentence[] = "now is the time for all good men";
    int a[] = {2,3,5,5,5,7,8,9};
    int len = sizeof(a)/sizeof(a[0]);
     
    //f = match("bob", data);
    //if (f != -1) printf("found match at %d -> %s\n", f, &data[f]);

    printf("is match %d\n", match(data, "bob"));
    printf("is pal   %d\n", palindrome(data2));
    printf("is ana   %d\n", anagram(data3, strlen(data3), data4, strlen(data4)));
    printf("sequential search %d\n", sequential_search(data, 10, 'd'));
    printf("binsearch %c\n", binsearch(data3, 5, 'c'));
    f = find_first_k(5, a, len);
    printf("find_first_k: data[%d] => %d\n", f, a[f]);
    printf("boyer %d\n", boyer(data, patt, 0));
    char *words[16];
    int iw = split(sentence, words);
    int i;
    for (i = 0; i < iw; i++) {
        printf("word[%d] = %s\n", i, words[i]);
    }

}

void test_sort()
{
    int a[] = {26,33,35,29,19,12,22,15,42,69,1};
    int sz = sizeof(a)/sizeof(a[0]);
    char s[32];
    show(a, sz);
    //read_quoted_string(s);
    //printf("%s\n", s);
    //string_show(s);
    //seasort_reverse(a, sz);
    selection(a, sz);
    show(a, sz);
    //printf("bubble: "); bubble(a, sz);
    //printf("insertion: "); insertion(a, sz);
    //printf("merge sort: "); merge_sort(a, 0, sz-1);
    //printf("quicksort: "); quicksort(a, 0, sz-1);
    //printf("shell: "); shell(a,sz);
    //printf("binsearch %d\n", binsearch(a, sz, 42));
    show(a, sz);
    //test_zip();
  
}

int main()
{
    //test_strings();
    test_sort();
    //subsets();
    //sequencer();
    //int a[] = {2,9,1,3,6,4};
    //int len = sizeof(a) / sizeof(a[0]);
    //show(a, len);
    //bubble(a, len);
    //insertion(a, len);
    //selection(a, len);
    //counting_sort();
    // unit test digit10(n,k)
    //printf("%d\n", digit(321,3));
    //radix_sort();

    //printf("test bit stuff\n");
    //test_bits();
  
    return 0;
}
