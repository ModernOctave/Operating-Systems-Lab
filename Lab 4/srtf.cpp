#include <algorithm>
#include <fstream>
#include <vector>
#include <map>
#include <queue>
#include <sstream>

using namespace std;

int context_switch_overhead = 1;

typedef struct Process
{
	int id;

	int arrival_time;
	vector<int> burst_times;

	int turnaround_time = 0;
	int waiting_time = 0;
} Process;

Process createProcess(int id, const string &s) {
    Process process;
    stringstream ss (s);
    string item;

	process.id = id;

	getline(ss, item, ' ');
	process.arrival_time = stoi(item);

    while (getline(ss, item, ' ')) {
		if (item.compare("-1") == 0) break;

        process.burst_times.push_back(stoi(item));
    }

    return process;
}

vector<Process> loadProcesses(const string &filename)
{
	ifstream input_file;
	input_file.open(filename);

	vector<Process> processes;

	// Skip lines until <pre>
	while (true)
	{
		char line[200];
		string line_str;

		input_file.getline(line, 200);
		line_str = line;

		if (line_str.compare(0, 5, "<pre>", 0, 5) == 0) break;
	}

	// Read lines until </pre>
	int process_id = 0;
	while (true)
	{
		char line[200];
		string line_str;

		input_file.getline(line, 200);
		line_str = line;

		if (input_file.eof() || line_str.compare(0, 6, "</pre>", 0, 6) == 0) break;

		Process process = createProcess(process_id, line_str);

		processes.push_back(process);
		process_id++;
	}

	input_file.close();

	return processes;
}

class SRTF
{
	public:
		int io_available_time = 0;
		int quantum = 10;

		class CompareBurst{
			public:
				bool operator()(Process a, Process b)
				{
					return a.burst_times[0] > b.burst_times[0];
				}
		};
		class CompareArrival{
			public:
				bool operator()(Process a, Process b)
				{
					return a.arrival_time > b.arrival_time;
				}
		};

		priority_queue<Process, vector<Process>, CompareBurst> ready;
		priority_queue<Process, vector<Process>, CompareArrival> waiting;
		vector<Process> finished;

		void addProcess(Process process)
		{
			waiting.push(process);
		}

		int run(int time)
		{
			// Pop any processes that have arrived
			while (!waiting.empty() && waiting.top().arrival_time <= time)
			{
				ready.push(waiting.top());
				waiting.pop();
			}

			if (!ready.empty())
			{		
				Process process = ready.top();
				ready.pop();
				// printf("Process %d is running at time %d\n", process.id, time);

				process.waiting_time += time - process.arrival_time;
				process.turnaround_time += process.waiting_time + min(quantum, process.burst_times[0]);

				// Update time
				time += min(quantum, process.burst_times[0]);

				// If CPU burst not complete
				if (process.burst_times[0] > quantum)
				{
					process.arrival_time = time;
					process.burst_times[0] -= quantum;
					ready.push(process);
					return time;
				}
				else
				{
					// Remove CPU burst
					process.burst_times.erase(process.burst_times.begin());

					// If IO follows, update arrival time
					if (process.burst_times.size() > 0)
					{
						process.arrival_time = max(time, io_available_time) + process.burst_times[0];
						io_available_time = process.arrival_time;
						// Remove IO burst
						process.burst_times.erase(process.burst_times.begin());
						waiting.push(process);
					}
					else {
						finished.push_back(process);
					}
				}
			}
			else {
				time++;
			}

			return time;
		}
};

int main(int argc, char const *argv[])
{
	vector<Process> processes = loadProcesses(argv[1]);

	SRTF srtf;
	for (Process process : processes)
	{
		srtf.addProcess(process);
	}

	int time = 0;
	int context_switches = -1;
	while (!srtf.ready.empty() || !srtf.waiting.empty())
	{
		context_switches++;
		time = srtf.run(time);
	}
	

	// Print process info
	printf("Process\tWaiting Time\tTurnaround Time\tPenalty Ratio\n");
	sort(srtf.finished.begin(), srtf.finished.end(), [](Process a, Process b) { return a.id < b.id; });
	for (Process process : srtf.finished)
	{
		printf("%d\t%d\t\t%d\t\t%f\n", process.id, process.waiting_time, process.turnaround_time, (float)process.turnaround_time/(process.turnaround_time-process.waiting_time));
	}
	// Print average info
	int total_waiting_time = 0;
	int total_turnaround_time = 0;
	float total_penalty_ratio = 0;
	for (Process process : srtf.finished)
	{
		total_waiting_time += process.waiting_time;
		total_turnaround_time += process.turnaround_time;
		total_penalty_ratio += (float)process.turnaround_time/(process.turnaround_time-process.waiting_time);
	}
	printf("Average\t%f\t%f\t%f\n", (float)total_waiting_time / srtf.finished.size(), (float)total_turnaround_time / srtf.finished.size(), total_penalty_ratio / srtf.finished.size());
	printf("Throughput: %f\n", (float)srtf.finished.size() / (time+context_switches*context_switch_overhead));
	return 0;
}
