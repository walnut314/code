#!/usr/bin/python3
import random, string
length = 12
print("automatic password generator: length = ", length)
myrg = random.SystemRandom()
# if you want non-English characters, remove the [0:52]
#alphabet = string.letters[0:52] + string.digits
alphabet = string.ascii_letters + string.digits + '$' + '^' + '@' + '%'
pw = str().join(myrg.choice(alphabet)
for _ in range(length))
print("password is: ", pw)

