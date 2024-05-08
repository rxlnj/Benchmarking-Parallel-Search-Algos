import os
import subprocess
import re

# Dataset sizes to benchmark
dataset_sizes = [10, 50, 100, 500, 1000, 5000, 10000, 50000, 100000, 500000, 1000000, 5000000, 10000000, 50000000, 100000000]


# List of implementations to benchmark
implementations = [
    ("binary_mpi", True),
    ("linear_mpi", True),
    ("binary_omp", False),
    ("linear_omp", False),
    ("binary_pthread", False),
    ("linear_pthread", False)
]

# Regular expression patterns to extract execution time
time_patterns = {
    "mpi": r"Time taken to search: (\d+\.\d+)ms",
    "omp": r"Search time: (\d+\.\d+) milliseconds",
    "pthreads": r"Search time: (\d+\.\d+) milliseconds"
}

# Function to run an implementation and capture its output
def run_implementation(impl_name, use_mpi, dataset_size):
    dataset_path = f"Testcases/dataset_{dataset_size}.txt"
    if use_mpi:
        cmd = ["mpirun", "-n", "8", "bin/" + impl_name, dataset_path]
    else:
        cmd = ["./bin/" + impl_name, dataset_path]
    
    result = subprocess.run(cmd, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True)
    output = result.stdout.strip()
    
    if use_mpi:
        pattern = time_patterns["mpi"]
    elif "omp" in impl_name:
        pattern = time_patterns["omp"]
    else:
        pattern = time_patterns["pthreads"]
    
    match = re.search(pattern, output)
    if match:
        execution_time = match.group(1)
    else:
        execution_time = "N/A"
        print("[ERROR] TIME WAS NOT SAVED!")
    print("[INFO] Finished Running " + str(impl_name) + " on Dataset of length " + str(dataset_size))
    return execution_time

# Run the benchmark and write results to file
output_filename = "benchmark_results.csv"
with open(output_filename, "w") as output_file:
    output_file.write("Dataset length,Binary_MPI_time,Linear_MPI_time,Binary_omp_time,Linear_omp_time,Binary_pthreads,Linear_pthreads\n")
    for dataset_size in dataset_sizes:
        binary_mpi_time = run_implementation("binary_mpi", True, dataset_size)
        linear_mpi_time = run_implementation("linear_mpi", True, dataset_size)
        binary_omp_time = run_implementation("binary_omp", False, dataset_size)
        linear_omp_time = run_implementation("linear_omp", False, dataset_size)
        binary_pthread_time = run_implementation("binary_pthread", False, dataset_size)
        linear_pthread_time = run_implementation("linear_pthread", False, dataset_size)
        output_file.write(f"{dataset_size},{binary_mpi_time},{linear_mpi_time},{binary_omp_time},{linear_omp_time},{binary_pthread_time},{linear_pthread_time}\n")

print(f"[SUCCESS] Benchmark results written to {output_filename}")
