// Author: TING-WEI (Willy) SU
// Course: CSCE 614 (Spring 2022)
// Institution: Texas A&M University ECE
// Compilation: g++ -Wall -g3 -std=c++11 -o cache cache.cpp
// Execution: gzip -dc 429.mcf-184B.trace.txt.gz | ./cache 2048 64 64 l 55752 5.575200% 55703 5.610155% 49 0.689752%

#include <iostream>
#include <string>
#include <queue>

#define DEBUG 1

#define KB 1024


#define ARGC_NUM 5

using namespace std;

// doubly linked list for managing the cache block
struct cacheblock{
    unsigned int mem_block_index; // main memory block ref
    struct cacheblock * next; // pointer to next node
    struct cacheblock * prev; // pointer to previos node
};

class cache{
    private:
        unsigned int total_cache_size; // size of the total cache: 4096, 8192... (Unit: kilo-Byte)
        unsigned int total_cache_set; // total number of the cache set
        unsigned int cache_set_count; // counter of the cache set
        unsigned int total_cache_block; // total number of the cache block
        unsigned int block_size; // size of block: 1024, 2048, 4096... (Unit: Byte)
        unsigned int n; // set associtive: N = 1, 2, 4...
        char replacement_policy; // 

        struct cacheblock * head;
        struct cacheblock * tail;
        

        // Output data
        unsigned int read_miss;
        unsigned int read_hit;

        unsigned int write_miss;
        unsigned int write_hit;

        char add_new_block(unsigned int block_index);

        char remove_victim_block();

        char switch_block_to_head(struct cacheblock * ptr);

        char replace_block(unsigned int block_index);

        struct cacheblock * search_cache_block(string opcode, unsigned int block_index);
#if DEBUG
        void print_list();
#endif
    public: 
        cache();

        cache(unsigned int _total_size, unsigned int _block_size, unsigned int _n, char _replacement_policy);

        char access_data_from_cache(string opcode, unsigned long long int addr);

        unsigned int get_r_miss_cnt();
        
        unsigned int get_w_miss_cnt();

        ~cache();
};

// Default constructor
cache::cache()
{
    read_hit = 0;
    read_miss = 0;
    write_miss = 0;
    write_hit = 0;

    total_cache_size = 2048;
    block_size = 64;
    n = 64;
    total_cache_set = total_cache_size * 1 * KB / (block_size * n);
    replacement_policy = 'l';
    cache_set_count = 0;
}

// Constructor
cache::cache(unsigned int _total_size, unsigned int _block_size, unsigned int _n, char _replacement_policy)
{
    read_hit = 0;
    read_miss = 0;
    write_miss = 0;
    write_hit = 0;

    total_cache_size = _total_size;
    block_size = _block_size;
    n = _n;
    replacement_policy = _replacement_policy;
    total_cache_set = total_cache_size * 1 * KB / (block_size * n);
    replacement_policy = 'l';
    cache_set_count = 0;
}

char cache::add_new_block(unsigned int block_index)
{
    char ret = 0;
    struct cacheblock * new_node = (struct cacheblock *) malloc(sizeof(struct cacheblock));

    if (cache_set_count >= total_cache_set)
    {
        return -1;
    }

    cache_set_count++;
    
    if (head == NULL)
    {
        // Add new node to the head
        // Nothing in cache
        new_node->mem_block_index = block_index;
        new_node->next = NULL;
        head = new_node;
        tail = head;
    }
    else
    {
        // Add new node to the head
        new_node->mem_block_index = block_index;
        new_node->next = head;
        new_node->prev = NULL;
        head->prev = new_node;
        head = new_node;
    }

#if DEBUG
    print_list();
#endif
    return ret;
}

char cache::remove_victim_block()
{
    char ret = -1;
    // Find victim, which is at the tail
    if (cache_set_count == 0)
    {
        return ret;
    }

    cache_set_count--;

    tail = tail->prev;
    delete tail->next;
    tail->next = NULL;

#if DEBUG
    print_list();
#endif
    return ret;
}

char cache::replace_block(unsigned int block_index)
{
    // First remove the victim 
    remove_victim_block();

    // Then add new block
    add_new_block(block_index);

#if DEBUG
    print_list();
#endif
}

struct cacheblock * cache::search_cache_block(string opcode, unsigned int block_index)
{
    bool is_hit = false;
    struct cacheblock * ptr_node;

    if (head != NULL)
    {
        // Search through the end of the list
        ptr_node = head;
        while(ptr_node != NULL)
        {
            if (ptr_node->mem_block_index == block_index)
            {
                // Found the block in cache, we have a hit
                is_hit = true;
                
                return ptr_node;
            }
            // traverse to the next node
            ptr_node = ptr_node->next;
        }
    }
    
#if DEBUG
    print_list();
#endif
    return NULL;
}

char cache::switch_block_to_head(struct cacheblock * ptr)
{
    char ret = -1;
    // Make the target block to the head
    if (ptr->prev == NULL)
    {
        // target already at head
        ret = 0;
        return ret;
    }
    else if (ptr->next == NULL)
    {
        // This is the tail node
        tail = ptr->prev;
        tail->next = NULL;
        ptr->next = head;
        ptr->prev = NULL;
        head = ptr;
    }
    else 
    {
        ptr->prev->next = ptr->next;
        ptr->next->prev = ptr->prev;
        ptr->next = head;
        ptr->prev = NULL;
        head = ptr;
    }
#if DEBUG
    print_list();
#endif
    ret = 0;
    return ret;
}

#if DEBUG
void cache::print_list()
{
    struct cacheblock * ptr = head;
    while (ptr != NULL)
    {
        cout << ptr->mem_block_index << "->";
        ptr = ptr->next;
    }    
    cout << "x" << endl;
}
#endif


char cache::access_data_from_cache(string opcode, unsigned long long int addr)
{
    char ret = 0;
    bool is_hit = false;
    unsigned int block_index = addr / block_size;
    unsigned int set_index = addr / (block_size * n);
    unsigned int offset = addr % block_size;
    struct cacheblock * target_block;

    if (head == NULL)
    {
        ret = add_new_block(block_index);

        if (ret < 0)
        {
            cout << "Error: the cache is full" << endl;
        }
    }
    else
    {
        // Search through the end of the list
        target_block = search_cache_block(opcode, block_index);

        if (target_block != NULL)
        {
            // Cached it already
            if (opcode == "w")
            {
                cout << "Write: " << addr << endl;
                write_hit++;
            }
            else if (opcode == "r")
            {
                cout << "Read: " << addr << endl;
                read_hit++;
            }
            switch_block_to_head(target_block);
        }
        else
        {
            // Miss occured
            ret = add_new_block(block_index);

            if (opcode == "w")
            {
                cout << "Write: " << addr << endl;
                write_miss++;
            }
            else if (opcode == "r")
            {
                cout << "Read: " << addr << endl;
                read_miss++;
            }
        }
    }

    return ret;
}

// Class methods 
unsigned int cache::get_r_miss_cnt()
{
    return read_miss;
}

unsigned int cache::get_w_miss_cnt()
{
    return write_miss;
}

// Destructor
cache::~cache()
{
    delete head;
}

int main(int argc, char **argv) {


    string opcode; // Could only be either 'r' or 'w'
    string delimiter = " ";
    int addr_length = 0;
    unsigned long long int addr = 0;
    string::size_type str_index = 0;
    char ret = -1;

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
    string std_in;
    while (getline(cin, std_in)) {
#if DEBUG
        cout << std_in << endl;
#endif
        // Parse the operator and memory address
        str_index = std_in.find_first_of(delimiter);
        cout << "delimeter: " << str_index << endl;
        opcode = std_in.substr(0, 1);
        addr_length = std_in.length() - 2;
        std_in = std_in.substr(str_index + 1, std_in.length() - 2);
        cout << "Address: " << std_in << endl;
        if (!std_in.empty())
        {
            // parse the address from cin
            addr = stoll(std_in, &str_index, 16);
        }

#if DEBUG
        cout << "Operation: " << opcode << endl;
        cout << "Address: " << addr << endl;
#endif

        // request data from cache
        ret = CacheSim.access_data_from_cache(opcode, addr);

        if (ret)
        {
            // Request data from cache fail
            // Find out why? 
        }

        std_in.clear();
    }

    // Request data from cache class
    r_miss = CacheSim.get_r_miss_cnt();
    w_miss = CacheSim.get_w_miss_cnt();

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