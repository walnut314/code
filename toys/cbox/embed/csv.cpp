#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdlib>

using namespace std;
using vec_str = std::vector<std::string>;
enum { INT, FLOAT, STRING };

// data description
// alter this to match the csv layout
#define NUM_COLS (3)
int layout[NUM_COLS] = {INT, INT, FLOAT};
string csv_in_filename  = "test5.csv";
string csv_out_filename = "testout.csv";
////////


bool is_sep_or_white(char a, char sep)
{
    if (a == sep) return true;
    return isspace(a);
}

std::vector<std::string> split(const std::string& s, char sep)
{
    std::vector<std::string> ret;
    typedef std::string::size_type string_size;
    string_size i = 0;
    while (i != s.size()) {
        // skip white space up to separator
        while (i != s.size() && is_sep_or_white(s[i], sep)) ++i;
        string_size j = i;
        // grab data up to separator
        while (j != s.size() && !is_sep_or_white(s[j], sep)) ++j;
        if (i != j) {
            // the substr is the data
            ret.push_back(s.substr(i, j - i));
            i = j;
        }
    }
    return ret;
}

void merge(std::ofstream& out, vec_str data)
{
    std::ostringstream ss;
    for (auto f : data) {
        out << f << ", ";
    }
    out << '\n';
}

std::vector<vec_str> csv_in(std::string file)
{
    std::ifstream csv_in(file);
    std::vector<vec_str> csv_data;
    std::string line;
    while(getline(csv_in, line)) {
        vec_str data = split(line, ',');
        csv_data.push_back(data);
    }
    return csv_data;
}

void csv_out(const std::vector<vec_str>& data, std::string file)
{
    std::ofstream out(file);
    for (auto d : data) {
        merge(out, d);
    }
}

void convert(int type, string data)
{
    int i;
    float f;
    switch (type) {
        case INT:
            i = atoi(data.c_str());
            cout << i;
            break;
        case FLOAT:
            f = atof(data.c_str());
            cout << f;
            break;
        case STRING:
            cout << data;
            break;
    }           
    std::cout << ", ";
}

void dump_csv(std::vector<vec_str> data)
{
    for (auto row : data) { // rows
        int num_cols = row.size();
        for (int col = 0; col < num_cols; col++) {
            int type = layout[col];
            convert(type, row[col]);
        }
        std::cout << '\n';
    }
}

std::vector<vec_str> process_csv(std::vector<vec_str> data)
{
    std::vector<vec_str> data_out;
    for (auto row : data) { // rows
        data_out.push_back(row);
    }
    return data_out;
}

void csv_open(std::string& name, std::ifstream& file)
{
    file.open(name);
}

std::vector<vec_str> csv_reader(std::ifstream& file)
{
    std::vector<vec_str> csv_data;
    std::string line;
    while(getline(file, line)) {
        vec_str data = split(line, ',');
        csv_data.push_back(data);
    }
    return csv_data;
}


void test1()
{
    std::vector<vec_str> data_in = csv_in(csv_in_filename);
    dump_csv(data_in);
    std::vector<vec_str> data_out = process_csv(data_in);
    csv_out(data_out, csv_out_filename);
}

std::ifstream csv_file;
void test2()
{
    csv_open(csv_in_filename, csv_file);
    std::vector<vec_str> csv_rows = csv_reader(csv_file);
    for (auto &row : csv_rows) {
        cout << "frame: " << row[0] << ", raw: " << row[1] << ", cooked: " << row[2] << endl;
    }
}


int main()
{
    test2();
}

