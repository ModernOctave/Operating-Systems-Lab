import subprocess
import sys
from matplotlib import pyplot as plt
import numpy as np


def getPageFaults(method="fifo", psize=20, vsize=60, ssize=60, testcase="req2.dat"):
	print("Running " + method + " with page size " + str(psize) + " and virtual size " + str(vsize) + " and stack size " + str(ssize) + " and test case " + testcase)
	output = subprocess.Popen(["./" + method, str(vsize), str(psize), str(ssize), testcase], stdout=subprocess.PIPE).communicate()[0]
	return int(output.split()[-1])

def plot(vsize=60, testcase="req1.dat"):
	for method in ["fifo", "lru", "rand"]:
		x = np.arange(1, vsize+1, 1)
		pageFaults = []
		for i in x:
			pageFaults.append(getPageFaults(method=method, vsize=vsize, psize=i, testcase=testcase))
		plt.plot(x, pageFaults, label=method)

	plt.title(testcase)
	plt.ylabel("Number of Page Faults")
	plt.xlabel("Number of Frames")
	plt.legend()
	plt.savefig(testcase.split(".")[0] + ".jpg")
	plt.clf()

if __name__ == "__main__":
	plot(60, "req1.dat")
	plot(60, "req2.dat")
	plot(30, "req3.dat")
	plot(30, "req4.dat")
	plot(30, "req5.dat")