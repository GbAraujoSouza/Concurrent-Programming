import os
import re
import subprocess

EXECUTABLE_1_PATH = "./vec.o"
EXECUTABLE_2_PATH = "./main.o"

NUM_RUNS = 5

OUTPUT_FILE = "test_results.txt"

params_for_vector_generation = [
    [str(10**6)],
    [str(10**7)],
    [str(10**8)],
]

params_for_concurrent_dot_product = [["vectors", str(i)] for i in range(1, 7)]


def parse_output(output_text):
    relative_error = None
    operation_time = None

    error_match = re.search(r"relative error: ([\d.eE+-]+)", output_text)
    if error_match:
        relative_error = float(error_match.group(1))

    time_match = re.search(r"Time \(ms\) for operation: ([\d.eE+-]+)", output_text)
    if time_match:
        operation_time = float(time_match.group(1))

    return relative_error, operation_time


def run_and_log(
    executable_vector_path,
    executable_product_path,
    vector_params,
    dot_product_params,
    output_file,
):
    if not os.path.isfile(executable_vector_path) or not os.access(
        executable_vector_path, os.X_OK
    ):
        error_message = f"Error: Executable not found or not executable at '{executable_vector_path}'\n"
        print(error_message)
        with open(output_file, "a") as f:
            f.write(error_message)
        return

    header = f"\n{'='*20}\nExecutando testes para produto interno\n{'='*20}\n"
    print(header)
    with open(output_file, "a") as f:
        f.write(header)

    for param in vector_params:

        errors = []
        times = []
        try:
            command_array_vector = [executable_vector_path] + param
            result = subprocess.run(
                command_array_vector, capture_output=True, text=True, check=True
            )
            for product_param in dot_product_params:
                command_array_product = [executable_product_path] + product_param
                command_str = " ".join(command_array_product)

                log_entry = f"\n--- Executando teste para: {product_param[1]} threads e dimensão: {param[0]}---\n"
                print(log_entry)

                with open(output_file, "a") as f:
                    f.write(log_entry)

                    for _ in range(5):
                        result = subprocess.run(
                            command_array_product,
                            capture_output=True,
                            text=True,
                            check=True,
                        )
                        error, time = parse_output(result.stdout)
                        if error is not None:
                            errors.append(error)
                        if time is not None:
                            times.append(time)

                avg_error = "N/A"
                if errors:
                    avg_error = sum(errors) / len(errors)

                avg_time = "N/A"
                if times:
                    avg_time = sum(times) / len(times)


                log_entry = (
                    f"Teste: {command_str}\n"
                    f"  - Variação média relativa: {avg_error:.26f}\n"
                    f"  - Tempo médio em ms: {avg_time:.26f}\n\n"
                )
                print(log_entry)
                with open(output_file, "a") as f:
                    f.write(log_entry)

        except FileNotFoundError:
            error_msg = f"Error: Command not found. Is '{executable_vector_path}' in your PATH or current directory?\n"
            print(error_msg)
            with open(output_file, "a") as f:
                f.write(error_msg)
        except subprocess.CalledProcessError as e:
            error_msg = (
                f"Return Code: {e.returncode}\n"
                f"Standard Output (stdout):\n{e.stdout}\n"
                f"Standard Error (stderr):\n{e.stderr}\n"
            )
            print(error_msg)
            with open(output_file, "a") as f:
                f.write(error_msg)
        except Exception as e:
            error_msg = f"An unexpected error occurred: {e}\n"
            print(error_msg)
            with open(output_file, "a") as f:
                f.write(error_msg)


if __name__ == "__main__":
    if os.path.exists(OUTPUT_FILE):
        os.remove(OUTPUT_FILE)

    print(f"Starting tests. Results will be saved to '{OUTPUT_FILE}'")

    run_and_log(
        executable_vector_path=EXECUTABLE_1_PATH,
        executable_product_path=EXECUTABLE_2_PATH,
        vector_params=params_for_vector_generation,
        dot_product_params=params_for_concurrent_dot_product,
        output_file=OUTPUT_FILE,
    )

    print("\nAll tests completed.")

