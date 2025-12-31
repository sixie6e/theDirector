from gmpy2 import mpz, powmod
from sympy import isprime
import json
import os
import sys

x = 19760537
# x = 198131369 # not emirp
# y = 21343377 +- 2
# print(isprime(x))
# print(isprime(y))
# print(is_prime((2**x)- 1))
# print(is_prime((2**y)- 1))

p = 19760537 
checkpoint = "checkpoint.json"
interval = 1000

def llcheck(p):
    mers = (mpz(1) << p) - 1
    s = mpz(4)
    start_iteration = 1
    
    if os.path.exists(checkpoint):
        with open(checkpoint, 'r') as f:
            data = json.load(f)
            if data['p'] == p:
                print(f"Resuming from iteration {data['iteration']}...")
                s = mpz(data['s'], 16)
                start_iteration = data['iteration'] + 1
            else:
                print(f"Iteration for different number located: {data['p']}. New session started for {p}.")

    try:
        for i in range(start_iteration, p - 1):
            s = (s * s - 2) % mers
            
            if i % interval == 0:
                save_progress(p, i, s)
                print(f"progress: {i}/{p-2} ({(i/(p-2))*100:.2f}%)", end='\r')
                
        is_prime = (s == 0)
        print(f"\nConclusion: {'Prime' if is_prime else 'Compost'}")
        if os.path.exists(checkpoint):
            os.remove(checkpoint) 

    except KeyboardInterrupt:
        print("\nKeyboard Interrupt.")
        save_progress(p, i, s)
        sys.exit()

    except MemoryError:
        print("\nMemory Error.")
        save_progress(p, i, s)
        sys.exit()

def save_progress(p, iteration, s):
    with open(checkpoint, 'w') as f:
        json.dump({
            'p': p,
            'iteration': iteration,
            's': s.digits(16) 
        }, f)

if __name__ == "__main__":
    llcheck(p)
