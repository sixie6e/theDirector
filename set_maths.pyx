# tag: openmp
# cython: language_level=3
import numpy as np
cimport numpy as cnp
from sympy import isprime
import pickle
import resource
import random
import os
import sys
import matplotlib.pyplot as plt

cdef dict sets = {}
cdef int next_set = 23

cdef cnp.ndarray filter_primes(cnp.ndarray[cnp.int64_t, ndim=1] arr):
    cdef list result = []
    cdef long val
    for val in arr:
        if isprime(int(val)):
            result.append(val)
    return np.array(result, dtype=np.int64)

def set_maths():
    global next_set
    
    cdef str mode, i_key, j_key, max_len_input, new_set_id
    cdef long limit_val, target_len
    cdef list all_keys, same_len_keys
    cdef cnp.ndarray[cnp.int64_t, ndim=1] set_x, set_y, m, a, m_filtered, a_filtered
    
    mode = input("(1) Manual (2) Auto: ").strip()
    
    while True:
        if mode == '2':
            all_keys = list(sets.keys())
            i_key = random.choice(all_keys)            
            target_len = len(sets[i_key])
            limit_val = target_len
            same_len_keys = [k for k in all_keys if len(sets[k]) == target_len and k != i_key]
     
            if target_len == 0:
                continue
            
            try:    
                j_key = random.choice(same_len_keys)            
            except IndexError:
                continue
            print(f"Auto-selected {i_key} and {j_key} (Len: {target_len})")
        else:
            i_key = input(f'First set number: ').strip()
            j_key = input('Second set number: ').strip()
            if i_key not in sets or j_key not in sets:
                print("Invalid.")
                continue
            max_len_input = input(f'Array length: ').strip()
            limit_val = int(max_len_input) if max_len_input.isdigit() else min(len(sets[i_key]), len(sets[j_key]))

        set_x = sets[i_key][:limit_val].astype(np.int64)
        set_y = sets[j_key][:limit_val].astype(np.int64)
        
        try:
            m = np.multiply(set_x, set_y)
            a = (np.add(set_x, set_y) - 1).astype(np.int64)
            
            m_filtered = filter_primes(m)
            a_filtered = filter_primes(a)

            results = {'m_filtered': m_filtered, 'a_filtered': a_filtered}
            
            for key_name, data in results.items():
                if len(data) > 0:
                    new_set_id = f'set{next_set}'
                    sets[new_set_id] = data
                    print(f"Stored {key_name} as {new_set_id} (Len: {len(data)})")
                    
                    plt.figure(figsize=(10, 6))
                    plt.scatter(range(len(data)), data, s=10)
                    plt.title(f'Primes in {new_set_id}')
                    plt.savefig(f'./img/{i_key}_{j_key}_{next_set}.png')
                    plt.close()
                    next_set += 1
                else:
                    next_set += 1

            if mode == '1' and input("\nRun again? (y/n): ").lower() != 'y':
                break

        except KeyboardInterrupt:
            sys.exit(0)
'''
cython -3 "set_maths.pyx" && gcc -shared -pthread -fPIC -fwrapv -O2 -Wall -I/usr/include/python3.10 -o "${fileBasenameNoExtension}.so" "${fileBasenameNoExtension}.c" (in directory: /home/sixie6e/theDirector/cythonize)
'''
