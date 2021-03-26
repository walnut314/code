#include <iostream>
#include <map>
#include <vector>
#include <memory>

class Character {
private:
    char m_char;
    std::vector<bool> m_pixels = std::vector<bool>(10000);
public:
    Character(const char& char_) : m_char(char_) {
        std::cout << "Character Pixels created for " << char_ 
            << std::endl;
    }
    const std::vector<bool>& get() const { return m_pixels; }
};

class Letter {
private:
    char m_char;
    std::shared_ptr<Character> m_character;
public:
    Letter(char char_) : m_char(char_) {}
    const std::vector<bool>& get() {
        if (!m_character) // lazy
            m_character = std::make_shared<Character>(m_char);
        return m_character->get();
    }
    const std::vector<bool>& get() const {
        return const_cast<Letter*>(this)->get();
    }
};

int main()
{
    std::map<char, std::shared_ptr<Letter>> letters_auth;
    for (size_t i = 0; i < 26; ++i) {
        char c = (char)('a' + i);
        letters_auth[c] = std::make_shared<Letter>(c);
    }
    printf("l: "); letters_auth['l']->get(); printf("\n");
    printf("a: "); letters_auth['a']->get(); printf("\n");
    printf("a: "); letters_auth['a']->get(); printf("\n");
    printf("a: "); letters_auth['a']->get(); printf("\n");
    printf("z: "); letters_auth['z']->get(); printf("\n");
    printf("y: "); letters_auth['y']->get(); printf("\n");
    //letters_auth['a']->get();
    //letters_auth['a']->get();
    //letters_auth['a']->get();
    //letters_auth['z']->get();
    //letters_auth['y']->get();
    return 0;
}
