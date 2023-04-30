#include <set>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;


struct PTE
{
	int PFN;
	int present = 0;
};

class Memory
{
	public:
		int numPageFaults = 0;

		int vsize;
		int psize;
		int ssize;
		set<int> memory;
		set<int> swap;
		PTE *pageTable;

		void printPageTable()
		{
			printf("Page Table:\n");
			for (int i = 0; i < vsize; i++)
			{
				printf("Page %d: PFN: %d, Present: %d\n", i, pageTable[i].PFN, pageTable[i].present);
			}
			printf("\n");
		}

		Memory(int vsize, int psize, int ssize)
		{
			this->vsize = vsize;
			this->psize = psize;
			this->ssize = ssize;

			pageTable = new PTE[vsize];
		}

		int getVictimFrame(int vaddr)
		{
			printPageTable();
			try
			{
				if (memory.size() < psize)
				{
					pageTable[vaddr].PFN = memory.size();
					memory.insert(vaddr);

					return vaddr;
				}
				else
				{
					int victimPage;
					int PFN = rand() % psize;
					printf("PFN: %d\n", PFN);
					for (int i = 0; i < vsize; i++)
					{
						if (pageTable[i].present == 1 && pageTable[i].PFN == PFN)
						{
							victimPage = i;
							break;
						}
					}
					memory.erase(victimPage);
					if (swap.size() < ssize)
					{
						swap.insert(victimPage);
					}
					else
					{
						throw "Swap space full!";
					}

					if (swap.find(vaddr) != swap.end())
					{
						swap.erase(vaddr);
					}
					memory.insert(vaddr);
					return victimPage;
				}
			}
			catch (const char* msg)
			{
				cerr << "Error: " << msg << endl;
				exit(1);
			}
		}

		void request(int vaddr)
		{
			PTE pte = pageTable[vaddr];
			
			if (pte.present == 1)
			{
				// Page hit
			}
			else
			{
				// Page fault
				numPageFaults++;

				// Find victim page
				int victimPage = getVictimFrame(vaddr);
				int PFN = pageTable[victimPage].PFN;

				// Update page table
				pageTable[victimPage].present = 0;
				pageTable[vaddr].present = 1;
				pageTable[vaddr].PFN = PFN;
			}
		}

		int getNumPageFaults()
		{
			return numPageFaults;
		}
};

int main(int argc, char const *argv[])
{
	if (argc != 5)
	{
		cout << "Usage: ./fifo <vsize> <psize> <ssize> <filename>" << endl;
		return 1;
	}

	int vsize = atoi(argv[1]);
	int psize = atoi(argv[2]);
	int ssize = atoi(argv[3]);
	char* filename = (char *)argv[4];

	// Create memory
	Memory mem(vsize, psize, ssize);

	// Read file
	fstream file;

	file.open(filename, ios::in);

	char line[256];
	file.getline(line, 256);

	stringstream ss(line);

	int num;
	while (ss >> num)
	{
		mem.request(num-1);
	}

	cout << "Number of page faults: " << mem.getNumPageFaults() << endl;
}