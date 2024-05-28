from concurrent.futures import ThreadPoolExecutor
from typing import Callable
def parallel_threads(function: Callable, iterable, threads):
    with ThreadPoolExecutor(max_workers=threads) as executor:
        return executor.map(function, iterable)