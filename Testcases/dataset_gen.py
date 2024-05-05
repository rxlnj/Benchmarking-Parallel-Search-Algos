import argparse
import random
from typing import Generator

random.seed(1)

class SortedDatasetGenerator:
    def __init__(self, size, min_value, max_value, chunk_size=10000):
        self.size = size
        self.min_value = min_value
        self.max_value = max_value
        self.chunk_size = chunk_size

    def generate_sorted_dataset(self) -> Generator[int, None, None]:
        remaining_size = self.size
        start = self.min_value

        while remaining_size > 0:
            end = min(start + self.chunk_size, self.max_value + 1)
            chunk = sorted(random.sample(range(start, end), min(self.chunk_size, remaining_size)))
            remaining_size -= len(chunk)
            start = end

            for num in chunk:
                yield num

    def write_to_file(self, dataset, filename):
        with open(filename, 'w') as file:
            for num in dataset:
                file.write(str(num) + '\n')

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Generate a sorted dataset and write it to a file.')
    parser.add_argument('-s', type=int, help='Size of the dataset')
    parser.add_argument('-min', type=int, help='Minimum value in the dataset')
    parser.add_argument('-max', type=int, help='Maximum value in the dataset')
    args = parser.parse_args()

    if args.s > args.max - args.min + 1:
        print("Max value should be greater than or equal to min value + size - 1, unless you want consecutive numbers; in that case, it should be equal to min value + size - 1.")
        exit(1)

    if not any(vars(args).values()):
        parser.print_help()
    else:
        size = args.s if args.s else 1000000
        min_value = args.min if args.min else 1
        max_value = args.max if args.max else min_value + size - 1

        filename = f'dataset_{size}.txt'
        generator = SortedDatasetGenerator(size, min_value, max_value)
        sorted_dataset = generator.generate_sorted_dataset()
        generator.write_to_file(sorted_dataset, filename)