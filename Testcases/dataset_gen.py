import argparse
import random

random.seed(1)

class SortedDatasetGenerator:
    def __init__(self, size, min_value, max_value):
        self.size = size
        self.min_value = min_value
        self.max_value = max_value

    def generate_sorted_dataset(self):
        dataset = sorted(random.sample(range(self.min_value, self.max_value + 1), self.size))
        return dataset

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

    if args.s > args.max:
        print("Max value should be greater than size, unless you want consecutive numbers; in that case, it should be equal to the size.")
        exit(1)

    if not any(vars(args).values()):
        parser.print_help()
    else:
        size = args.s if args.s else 1000000
        min_value = args.min if args.min else 1
        max_value = args.max if args.max else 1000000000
        filename = f'dataset_{size}.txt'

    generator = SortedDatasetGenerator(size, min_value, max_value)
    sorted_dataset = generator.generate_sorted_dataset()
    generator.write_to_file(sorted_dataset, filename)
