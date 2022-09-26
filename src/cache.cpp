// Author: TING-WEI (Willy) SU
// Course: CSCE 614 (Spring 2022)
// Institution: Texas A&M University ECE
// Compilation: g++ -g3 -std=c++11 -o cache cache.cpp
// Execution: gzip -dc 429.mcf-184B.trace.txt.gz | ./cache 2048 64 64 l 55752 5.575200% 55703 5.610155% 49 0.689752%

#include <iostream>
#include <string>
#include <queue>

#define DEBUG 1

#define ARGC_NUM 5

using namespace std;

class cache{
    private:
        unsigned int total_size; // size of the total cache: 4096, 8192... (Unit: kilo-Byte)
        unsigned int cache_set_index; // index of the set
        unsigned int cache_block_index; // index of the block
        unsigned int block_size; // size of block: 1024, 2048, 4096... (Unit: Byte)
        unsigned int n; // set associtive: N = 1, 2, 4...
        char replacement_policy; // 
        queue <int> replacement_queue;

        // Output data
        unsigned int read_miss;
        unsigned int read_hit;

        unsigned int write_miss;
        unsigned int write_hit;



    public: 
        cache();

        cache(unsigned int _total_size, unsigned int _block_size, unsigned int _n, char _replacement_policy);
        
        ~cache();
};

// Default constructor
cache::cache()
{
    total_size = 2048;
    block_size = 64;
    n = 64;
    replacement_policy = 'l';
}

// Constructor
cache::cache(unsigned int _total_size, unsigned int _block_size, unsigned int _n, char _replacement_policy)
{
    total_size = _total_size;
    block_size = _block_size;
    n = _n;
    replacement_policy = _replacement_policy;
#if DEBUG
    cout << total_size << " ";
    cout << block_size << " ";
    cout << n << " ";
    cout << replacement_policy << " ";
#endif

}

// Destructor
cache::~cache()
{
}

int main(int argc, char **argv) {

    unsigned int cache_size;
    unsigned int block_size;
    unsigned int N;
    char repl;

    unsigned int total_miss;
    double total_miss_rate;
    unsigned int w_miss;
    double w_miss_rate;    
    unsigned int r_miss;
    double r_miss_rate;

    // Assert the argc is correct
    if (argc != ARGC_NUM) {
	    cout << "Execution format: ./cache <cache size (KB)> <block size (B)> <N (set)> <Replacement policy>" << endl;
	    exit(0);
    }

    cache_size = atoi(argv[1]);
    block_size = atoi(argv[2]);
    N = atoi(argv[3]);
    repl = argv[4][0];

    cache CacheSim(cache_size, block_size, N, repl);

    // Take-in the cache simulator parameter
    // Format: ./cache 2048 64 64 l
    string lineInput = " ";
    while (getline(cin,lineInput)) {
      cout << lineInput << endl;
      
    }

    // Request data from cache class


    // Output statistics to terminal
    cout << total_miss << " "; 
    cout << total_miss_rate << " "; 
    cout << w_miss << " "; 
    cout << w_miss_rate << " "; 
    cout << r_miss << " "; 
    cout << r_miss_rate << " "; 
    cout << endl;
    return 0;
}