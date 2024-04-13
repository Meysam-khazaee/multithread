import threading
import hashlib


def new_hash_function(input_string, thread_count):
    """Calculates the SHA-1 hash of the input string using multithreading.

    Args:
        input_string: The string to calculate the SHA-1 hash for.
        thread_count: The number of threads to use for parallel processing.

    Returns:
        The SHA-1 hash of the input string as a hexadecimal string.
    """

    threads = []
    last_sha1s = [""] * thread_count  # Pre-allocate list for efficiency
    queue = []  # Use a queue for thread synchronization

    def thread_function(queue, last_sha1s, thread_id, input_string):
        """Calculates intermediate SHA-1 values and updates the queue."""
        for counter in range(2**20):
            if counter == 0:
                data = f"{thread_id}{counter}{input_string}"
            else:
                while queue[thread_id % thread_count] is None:
                    pass
                data = f"{thread_id}{queue[thread_id % thread_count]}{counter}{input_string}"
            sha1 = hashlib.sha1(data.encode()).hexdigest()
            last_sha1s[thread_id] = sha1
            queue[(thread_id + 1) % thread_count] = sha1

    # Create and start threads
    for i in range(thread_count):
        thread = threading.Thread(target=thread_function, args=(queue, last_sha1s, i, input_string))
        thread.start()
        threads.append(thread)

    # Wait for all threads to finish
    for thread in threads:
        thread.join()

    # Final SHA-1 calculation
    final_hash = last_sha1s[-1]  # Use the last element directly

    return final_hash


def main():
    """Calculates and prints the SHA-1 hash of a sample string."""
    input_string = "meysam_khazaee"
    thread_count = 10

    start_time = time.time()
    hash_result = new_hash_function(input_string, thread_count)
    end_time = time.time()
    execution_time = end_time - start_time

    print("Hash Value:", hash_result)
    print("Execution Time:", execution_time, "seconds")


if __name__ == "__main__":
    main()
