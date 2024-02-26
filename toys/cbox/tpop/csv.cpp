#include "csv.hpp"

int main(void)
{
    string line;
    Csv csv;

    while (csv.getline(line) != 0) {
        cout << "line = '" << line << "'\n";
        for (int i = 0; i < csv.getnfield(); i++)
            cout << "field[" << i << "] = '" << csv.getfield(i) << "'\n";
    }

    return 0;
}

