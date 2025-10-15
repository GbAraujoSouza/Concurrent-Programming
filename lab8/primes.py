import math
def ehPrimo(n: int) -> bool:
    if n <= 1:
        return False
    if n == 2:
        return True
    if n % 2 == 0:
        return False
    r = math.isqrt(n)              # inteiro: floor(sqrt(n))
    for i in range(3, r + 1, 2):   # só ímpares
        if n % i == 0:
            return False
    return True


N = 1 * 10**6
count = 0
for i in range(1, N):
    if ehPrimo(i):
        count += 1
print(count)
