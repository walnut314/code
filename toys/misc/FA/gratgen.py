#!/usr/bin/python3
import random, string

br_str = [] 
br_str.append("8 oz yog")
br_str.append("6 oz berries")
br_str.append("examples")

print("automatic gratitude/meal generator\n\n")
myrg = random.SystemRandom()
grats = range(256)
gr = myrg.choice(grats)

meals = range(3)
br = myrg.choice(meals)
lu = myrg.choice(meals)
di = myrg.choice(meals)

#print("gr: ", gr)
#print("br: ", br)
#print("lu: ", lu)
#print("di: ", di)

print("B: 8 oz yog, 6 oz berries, 2 oz oats\n")
print("L: 2 eggs, 6 oz carrots, 6 oz salad, 1 T oil\n")
print("D: 6 oz tofu, 6 oz broc, 6 oz salad, 1 T oil\n\n")

print("          Grats:")
print("1. ")
print("2. ")
print("3. ")
print("4. ")
print("5. ")


