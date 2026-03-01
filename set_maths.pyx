# tag: openmp
import numpy as np
cimport numpy as cnp
from sympy import isprime
import pickle
import resource
import random
import os
import sys
import matplotlib.pyplot as plt

from libc.stdlib cimport malloc, free

cdef dict sets = {}
cdef int next_set = 23

def resume():
    global next_set
    cdef list resume_files = [f for f in os.listdir('.') if f.endswith('.pkl')]
    if not resume_files:
        print("No file found.")
        return

    resume_files.sort(key=os.path.getmtime, reverse=True)
    recent = resume_files[0]
    print(f"Trying: {recent}")
    
    try:
        with open(recent, 'rb') as f:
            data = pickle.load(f)
            if isinstance(data, dict):
                if 'sets' in data:
                    sets.update(data['sets'])
                    next_set = data.get('next_set', next_set)
                    print(f"Success. Next set index: {next_set}")
    except Exception as e:
        print(f"Failed: {e}")

cdef cnp.ndarray[cnp.int64_t, ndarray=True] filter_primes(cnp.ndarray[cnp.int64_t, ndarray=True] arr):
    cdef list result = []
    cdef long val
    for val in arr:
        if isprime(int(val)):
            result.append(val)
    return np.array(result, dtype=np.int64)

def set_maths():
    global next_set
    cdef str mode = input("(1) Manual (2) Auto: ").strip()
    cdef str i, j, max_len_input
    cdef long limit_val, target_len
    cdef list all_keys, same_len_keys
    
    while True:
        results = {}
               
        if mode == '2':
            all_keys = list(sets.keys())
            i = random.choice(all_keys)            
            target_len = len(sets[i])
            limit_val = target_len
            same_len_keys = [k for k in all_keys if len(sets[k]) == target_len and k != i]
     
            if target_len == 0:
                continue
            
            try:    
                j = random.choice(same_len_keys)            
            except IndexError:
                continue
                
            print(f"Auto-selected {i} and {j} (Len: {target_len})")
        else:
            i = input(f'First set number: ').strip()
            j = input('Second set number: ').strip()
            if i not in sets or j not in sets:
                print("Invalid.")
                continue
            max_len_input = input(f'Array length: ').strip()
            limit_val = int(max_len_input) if max_len_input.isdigit() else min(len(sets[i]), len(sets[j]))

        cdef cnp.ndarray set_x = sets[i][:limit_val].astype(np.int64)
        cdef cnp.ndarray set_y = sets[j][:limit_val].astype(np.int64)
        
        try:
            m = np.multiply(set_x, set_y)
            a = np.add(set_x, set_y) - 1
            m_filtered = filter_primes(m)
            a_filtered = filter_primes(a)

            results = {'m_filtered': m_filtered, 'a_filtered': a_filtered}
            
            for key_name, data in results.items():
                if len(data) > 0:
                    new_set_id = f'set{next_set}'
                    sets[new_set_id] = data
                    print(f"Stored {key_name} as {new_set_id} (Len: {len(data)})")
                    
                    plt.figure(figsize=(10, 6))
                    plt.scatter(data, data, s=10)
                    plt.title(f'Primes found in: {i} op {j}')
                    plt.savefig(f'/img/{i}_{j}_{key_name}.png')
                    plt.close()
                    next_set += 1
                else:
                    next_set += 1

            if mode == '1' and input("\nRun again? (y/n): ").lower() != 'y':
                break

        except KeyboardInterrupt:
            sys.exit(0)
