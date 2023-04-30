#include <set>
#include <queue>
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
		set<int> swap;
		queue<int> activePageQueue;
		PTE *pageTable;

		Memory(int vsize, int psize, int ssize)
		{
			this->vsize = vsize;
			this->psize = psize;
			this->ssize = ssize;

			pageTable = new PTE[vsize];
		}

		int getVictimFrame(int vaddr)
		{
			try
			{
				if (activePageQueue.size() < psize)
				{
					pageTable[vaddr].PFN = activePageQueue.size();
					activePageQueue.push(vaddr);

					return vaddr;
				}
				else
				{
					int victimPage = activePageQueue.front();
					activePageQueue.pop();
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
					activePageQueue.push(vaddr);
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
				// Find page in queue and put it at the back
				queue<int> tempQueue;
				while (!activePageQueue.empty())
				{
					int page = activePageQueue.front();
					activePageQueue.pop();
					if (page != vaddr)
					{
						tempQueue.push(page);
					}
				}
				activePageQueue = tempQueue;
				activePageQueue.push(vaddr);
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