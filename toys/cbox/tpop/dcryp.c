#include<stdio.h>

#include<ctype.h>

int main() {

    char text[500], ch;

    int key;

    // Taking user input.

    printf("Enter a message to decrypt: ");

    scanf("%s", text);

    printf("Enter the key: ");

    scanf("%d", & key);

    // Visiting each character.
    for (int i = 0; text[i] != '\0'; ++i) {

        ch = text[i];
        // Check for valid characters.
        if (isalnum(ch)) {
            //Lowercase characters.
            if (islower(ch)) {
                ch = (ch - 'a' - key + 26) % 26 + 'a';
            }
            // Uppercase characters.
            if (isupper(ch)) {
                ch = (ch - 'A' - key + 26) % 26 + 'A';
            }
            // Numbers.
            if (isdigit(ch)) {
                ch = (ch - '0' - key + 10) % 10 + '0';
            }
        }
        // Invalid characters.
        else {
            printf("Invalid Message");
        }
        // Adding decoded character back.
        text[i] = ch;

    }

    printf("Decrypted message: %s\n", text);

    return 0;

}

