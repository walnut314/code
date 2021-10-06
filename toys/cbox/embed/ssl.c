#include <stdio.h> 
#include <openssl/aes.h>   

// http://stackoverflow.com/questions/9889492/how-to-do-encryption-using-aes-in-openssl

static const unsigned char key[] = {
  0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
      0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        };

void main()
{
    unsigned char text[]="virident";
    unsigned char out[10]; 
    unsigned char decout[10];

    AES_KEY wctx;

    AES_set_encrypt_key(key, 128, &wctx);
    AES_encrypt(text, out, &wctx);  

    printf("encryp data = %s\n", out);
    
    AES_decrypt(out, decout, &wctx);
    printf(" Decrypted o/p: %s \n", decout);

}

#if 0

// http://stackoverflow.com/questions/2262386/generate-sha256-with-openssl-and-c

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

using namespace std;

#include <openssl/sha.h>
string sha256(const string str)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    return ss.str();
}

int main() {
    cout << sha256("1234567890_1") << endl;
    cout << sha256("1234567890_2") << endl;
    cout << sha256("1234567890_3") << endl;
    cout << sha256("1234567890_4") << endl;
    return 0;
}

#endif
