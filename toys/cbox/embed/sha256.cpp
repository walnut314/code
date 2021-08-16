#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <cctype>
#include <utility>
#include <sstream>

using namespace std;

// g++ -std=c++0x sha256.cpp
 
bool Check(std::istream & stream, std::istream & hashStream)
{
    cout << "Check returning true" << endl;
    return true;
}

bool HashCheck(std::istream & stream, const std::string & hashFile)
{
    std::fstream hashStream;
    hashStream.open(hashFile, std::ios_base::in);
    if (!hashStream || !hashStream.is_open())
    {
        cout << "HashCheck returning false" << endl;
        return false;
    }

    return Check(stream, hashStream);
}


void sha256()
{
    bool enabled = false;
    static const std::string SALT("PeanutButterAndJellySandwich"); ///< Fixed hash salt
    static const std::string HASH_FILE("override.sha256"); ///< Fixed hash file location

    // Determine the current time
    using std::chrono::system_clock;
    using std::setw;

    static const int EPOCH_START = 1900;

    const auto timeNow = system_clock::to_time_t(system_clock::now());

    struct tm * time = localtime(&timeNow);

    std::stringstream ss;
    ss << SALT;
        
    ss.fill('0');
        
    ss << '_';
    ss << setw(4) << (time->tm_year + EPOCH_START);
    ss << '_';
    ss << setw(2) << (time->tm_mon + 1); // tm_mon starts at January = 0.
    ss << '_';
    ss << setw(2) << time->tm_mday;

    // Check the hash
    enabled = HashCheck(ss, HASH_FILE);
    cout << ss << endl;

}

int main()
{
    sha256();
}
