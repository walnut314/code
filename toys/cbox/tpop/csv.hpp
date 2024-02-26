#include <iostream>
#include <vector>
#include <string>

using namespace std;

class Csv {
 
    public:
        Csv(istream& fin = cin, string sep = ",") :
            fin(fin), fieldsep(sep) {}

        int getline(string&);
        string getfield(int n);
        int getnfield() const { return nfield; }

    private:
        istream& fin;
        string line;
        vector<string> field;
        int nfield;
        string fieldsep;

        int split();
        int endofline(char);
        int advplain(const string& line, string& fld, int);
        int advquoted(const string& line, string& fld, int);
};

int Csv::getline(string& str)
{
    char c;

    for (line = ""; fin.get(c) && !endofline(c); )
        line += c;
    split();
    str = line;
    return !fin.eof();
}

int Csv::endofline(char c)
{
    int eol;

    eol = (c == '\r' || c == '\n');
    if (c == '\r') {
        fin.get(c);
        if (!fin.eof() && c != '\n')
            fin.putback(c);
    }
    return eol;
}

int Csv::split()
{
    string fld;
    int i, j;

    nfield = 0;
    if (line.length() == 0)
        return 0;
    i = 0;
    do {
        if (i < line.length() && line[i] == '"')
            j = advquoted(line, fld, ++i);
        else
            j = advplain(line, fld, i);
        if (nfield >= field.size())
            field.push_back(fld);
        else
            field[nfield] = fld;
        nfield++;
        i = j + 1;
    } while (j < line.length());

    return nfield;
}

int Csv::advquoted(const string& s, string& fld, int i)
{
    int j;

    fld = "";
    for (j = i; j < s.length(); j++) {
        if (s[j] == '"' && s[++j] != '"') {
            int k = s.find_first_of(fieldsep, j);
            if ( k > s.length())
                k = s.length();
            for (k -= j; k-- > 0; )
                fld += s[j++];
            break;
        }
        fld += s[j];
    }
    return j;
}

int Csv::advplain(const string& s, string& fld, int i)
{
    int j;

    j = s.find_first_of(fieldsep, i);
    if (j > s.length())
        j = s.length();
    fld = string(s, i, j-i);
    return j;
}

string Csv::getfield(int n)
{
    if (n < 0 || n >= nfield)
        return "";
    else
        return field[n];
}



