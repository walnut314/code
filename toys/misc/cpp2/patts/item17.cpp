#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <windows.h>

class Character {
private:
    char m_char;
    std::vector<bool> m_pixels = std::vector<bool>(10000);
public:
    Character(const char& char_) : m_char(char_) {
    }
    void display(const unsigned short& color) const {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, color);
        std::cout << m_char;
    }
};

class Factory {
private:
    std::map<char, std::shared_ptr<Character>> m_character;
public:
    static std::shared_ptr<Factory> instance() {
        static std::shared_ptr<Factory> instance = std::make_shared<Factory>();
        return instance;
    }
    static std::shared_ptr<Character> getLetter(char char_) {
        return instance()->get(char_);
    }
    std::shared_ptr<Character> get(char char_) {
        auto iter = m_character.find(char_);
        if (iter != m_character.end())
            return iter->second;
        std::shared_ptr<Character> character = std::make_shared<Character>(char_);
        m_character[char_] = character;
        return character;
    }
};

class Letter {
private:
    unsigned short m_color;
    std::shared_ptr<Character> m_character;
public:
    Letter(char char_, const unsigned short& color) 
        : m_color(color), m_character(Factory::getLetter(char_)) {}
    void display() const {
        m_character->display(m_color);
    }
};

class Text {
private:
    std::vector<Letter> m_letters;
public:
    void add_Letters(const std::string& text, const unsigned short& color) {
        for (const char c: text)
            m_letters.emplace_back(c, color);
    }
    void display() const {
        for (const Letter& letter: m_letters)
            letter.display();
    }
};

void defaults(int save)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    static CONSOLE_SCREEN_BUFFER_INFO ConsoleScreenBufferInfo;
    if (save) {
        GetConsoleScreenBufferInfo(hConsole, &ConsoleScreenBufferInfo);
    } else {
        SetConsoleTextAttribute(hConsole, ConsoleScreenBufferInfo.wAttributes);
    }
}

int main()
{
    Text text;
    defaults(1);
    text.add_Letters("ababa", 12);
    text.add_Letters("fgggf", 9);
    text.add_Letters("hijklmnop", 13);
    text.display();
    defaults(0);
    return 0;
}

