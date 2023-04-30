import random


# LRU Prefering Sequence Generator
file = open("req2.dat", "w")
for _ in range(77):
	if random.random() < 0.2:
		file.write("3 ")
	elif random.random() < 0.25:
		file.write("5 ")
	else:
		file.write(f"{random.randint(1, 60)} ")
file.close()

# Random Sequence Generator
file = open("req3.dat", "w")
for _ in range(100):
	file.write(f"{random.randint(1, 30)} ")
file.close()

# LRU Prefering Sequence Generator
file = open("req4.dat", "w")
for _ in range(100):
	if random.random() < 0.2:
		file.write("3 ")
	elif random.random() < 0.25:
		file.write("5 ")
	else:
		file.write(f"{random.randint(1, 30)} ")
file.close()

# LRU Prefering Sequence Generator
file = open("req5.dat", "w")
for _ in range(100):
	file.write(f"{min(max(1, int(random.normalvariate(15, 2))), 30)} ")
file.close()