#include "csv.hpp"
#include <iostream>

using namespace std;

string csv_in_filename  = "test5.csv";

int main()
{
    CSV_MINI csv;
    csv.open(csv_in_filename);
    std::vector<vec_str> csv_rows = csv.reader(',');
    for (auto &row : csv_rows) {
        cout << "frame: " << row[0] << ", raw: " << row[1] << ", cooked: " << row[2] << endl;
    }
    
}
