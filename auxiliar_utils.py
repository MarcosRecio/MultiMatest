from concurrent.futures import ThreadPoolExecutor
from typing import Callable, Iterable
def parallel_threads(function: Callable, iterable: Iterable, threads:int):
    """
    Description: Use multithreading to compute the function 
                over the items of the iterable.

    Arguments:
        - function (Callable): Function to be executed over each item of the iterable
        - iterable (Iterable): Iterable with elements that are arguments of the function
        - threads (int): Integer to choose the number of threads to be open

    Output:
        - list: List with the results of the computations                 
    """
    
    with ThreadPoolExecutor(max_workers=threads) as executor:
        return list(executor.map(function, iterable))