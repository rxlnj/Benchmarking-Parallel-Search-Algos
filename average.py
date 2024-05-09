import csv

def read_csv(file_path):
    data = []
    with open(file_path, 'r') as file:
        csv_reader = csv.reader(file)
        for row in csv_reader:
            data.append(row)
    return data

def write_csv(file_path, data):
    with open(file_path, 'w', newline='') as file:
        csv_writer = csv.writer(file)
        for row in data:
            csv_writer.writerow(row)

def is_float(value):
    try:
        float(value)
        return True
    except ValueError:
        return False

file_paths = ["1benchmark_results.csv", "2benchmark_results.csv", "3benchmark_results.csv"]
data = [read_csv(file_path) for file_path in file_paths]

averaged_data = []
for rows in zip(*data):
    averaged_row = []
    for cells in zip(*rows):
        if all(is_float(cell) for cell in cells):
            average = sum(float(cell) for cell in cells) / len(cells)
            averaged_row.append(average)
        else:
            averaged_row.append(cells[0])  
    averaged_data.append(averaged_row)

write_csv("averaged.csv", averaged_data)


