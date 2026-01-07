/**
    BigInt (GMP): In the Python script, numbers in set22 exceed 100 million. When you multiply these, you can easily exceed 264−1. Standard C++ long long would "overflow" (reset to zero or wrap around), giving wrong math. GMP allows numbers to grow to millions of digits if needed.

    The #pragma omp parallel Directive: This tells the compiler to split the loop into chunks. If you have 8 CPU cores, it will process roughly 1/8 of the array on each core simultaneously.

    mpz_probab_prime_p: This is the C++ equivalent of isprime. It is written in highly optimized Assembly for various CPU architectures, making it significantly faster than the Sympy version.

    Memory Management: Unlike Python, which uses a "garbage collector" that can pause your program, C++ manages memory manually or via vectors. This prevents the random "lag" spikes found in the Python version.
    
    g++ -O3 -fopenmp set_maths.cpp -lgmpxx -lgmp -o set_maths

    -O3: Enables maximum compiler optimizations.

    -fopenmp: Enables multi-threading.

    -lgmpxx -lgmp: Links the GMP math libraries.
*/

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <random>
#include <fstream>
#include <gmpxx.h> 
#include <omp.h>
#include <sys/resource.h>

using namespace std;

// set22 will quickly exceed 64bit int limit.
typedef mpz_class BigInt;

// cpp implementation of 'sets' dictionary.
map<string, vector<BigInt>> sets;
int next_set = 23;

bool is_prime(const BigInt& n) {
    if (n <= 1) return false;
    // 0 == composite, >0 == probably prime
    return mpz_probab_prime_p(n.get_mpz_t(), 25) > 0;
}


// column == set, row == value

void save_to_csv(const string& filename) {
    ofstream file(filename);

    if (file.is_open()) {
        bool first = true;
        for (auto const& [name, values] : sets) {
            if (!first) file << ",";
            file << name;
            first = false;
        }
        file << "\n";
        
        size_t max_rows = 0;
        for (auto const& [name, values] : sets) {
            max_rows = max(max_rows, values.size());
        }

        for (size_t r = 0; r < max_rows; ++r) {
            first = true;
            for (auto const& [name, values] : sets) {
                if (!first) file << ",";
                
                if (r < values.size()) {
                    file << values[r].get_str();
                }
                first = false;
            }
            file << "\n";
        }
        file.close();
        cout << "Results saved to " << filename << endl;
    } else {
        cerr << "Error: Could not open file." << endl;
    }
}

void process_sets(const string& id_i, const string& id_j, int limit_val) {
    const vector<BigInt>& set_x = sets[id_i];
    const vector<BigInt>& set_y = sets[id_j];
    vector<BigInt> m_res, a_res, s_res, f_res, mod_res;

    #pragma omp parallel
    {
        vector<BigInt> local_m, local_a, local_s, local_f, local_mod;

        #pragma omp for nowait
        for (int k = 0; k < limit_val; ++k) {
            BigInt x = set_x[k];
            BigInt y = set_y[k];
          
            BigInt m = x * y;
            if (is_prime(m)) local_m.push_back(m);
            
            BigInt a = x + y;
            if (is_prime(a)) local_a.push_back(a);
            
            BigInt s = x - y;
            if (s > 0 && is_prime(s)) local_s.push_back(s);
          
            if (y != 0) {
                BigInt f = x / y;
                if (is_prime(f)) local_f.push_back(f);
                BigInt mod = x % y;
                if (is_prime(mod)) local_mod.push_back(mod);
            }
        }
        
        #pragma omp critical
        {
            m_res.insert(m_res.end(), local_m.begin(), local_m.end());
            a_res.insert(a_res.end(), local_a.begin(), local_a.end());
            s_res.insert(s_res.end(), local_s.begin(), local_s.end());
            f_res.insert(f_res.end(), local_f.begin(), local_f.end());
            mod_res.insert(mod_res.end(), local_mod.begin(), local_mod.end());
        }
    }
  
    if (!m_res.empty()) sets["set" + to_string(next_set++)] = m_res;
    if (!a_res.empty()) sets["set" + to_string(next_set++)] = a_res;
    if (!s_res.empty()) sets["set" + to_string(next_set++)] = s_res;
    if (!f_res.empty()) sets["set" + to_string(next_set++)] = f_res;
    if (!mod_res.empty()) sets["set" + to_string(next_set++)] = mod_res;
    cout << "Processed. Next set index is: " << next_set << endl;
}

int main() {
    struct rlimit mem_limit;
    mem_limit.rlim_cur = 4ULL * 1024 * 1024 * 1024;
    mem_limit.rlim_max = 4ULL * 1024 * 1024 * 1024;
    setrlimit(RLIMIT_AS, &mem_limit);

    sets["set21"] = {2, 3, 5, 7, 13, 17, 19, 31, 61, 89, 107, 127, 521, 607, 1279, 2203, 2281, 3217, 4253, 4423, 9689, 9941, 11213};
    sets["set22"] = {BigInt("44497"), BigInt("86243"), BigInt("110503"), BigInt("132049"), BigInt("216091")};

    string i, j;
    cout << "First set name (e.g., set21): ";
    cin >> i;
    cout << "Second set name (e.g., set22): ";
    cin >> j;

    if (sets.count(i) && sets.count(j)) {
        int limit_val = min(sets[i].size(), sets[j].size());
        process_sets(i, j, limit_val);
        save_to_csv("results.csv");
    } else {
        cout << "Invalid sets." << endl;
    }

    return 0;
}
