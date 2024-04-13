# Author: meysam_khazaee
import threading
import hashlib
import time

itratetion_number = 2**20
thread_number = 10

def new_hash_function(input, k):
    threads = []
    last_sha1s = [["" for element in range(itratetion_number)] for element in range(k)]
    mutex = threading.Lock()
    
    def thread_function(thread_id):
        mutex.acquire()
        nonlocal last_sha1s  # Access last_sha1s in the enclosing scope
        group_mate = thread_id ^ 1  # XOR with 1 to get the group mate
        for counter in range(itratetion_number):
            if counter == 0 :
                data = f"{thread_id}{counter}{input}"
            else:
                while(last_sha1s[group_mate][counter-1] == ""):
                    pass
                data = f"{thread_id}{last_sha1s[group_mate][counter-1]}{counter}{input}"
            sha1 = hashlib.sha1(data.encode()).hexdigest()
            last_sha1s[thread_id][counter] = sha1
        mutex.release()
            

    # Create threads and start their execution
    for i in range(k):
        thread = threading.Thread(target=thread_function, args=(i,))
        threads.append(thread)
        thread.start()

    # Wait for all threads to finish
    for thread in threads:
        thread.join()

    # Collect final SHA1s from each thread
    final_hash = []
    for row in last_sha1s:
    # Append the last element to the row (modifies original matrix)
        final_hash.append(row[-1])
    final_sha1s = "".join(final_hash)

    # Calculate the overall SHA1
    overall_sha1 = hashlib.sha1(final_sha1s.encode()).hexdigest()

    return overall_sha1

def main():
    start_time = time.time()
    hash_result = new_hash_function("meysam_khazaee", thread_number)
    end_time = time.time()
    execution_time = end_time - start_time
    print("Hash Value:", hash_result)
    print("Execution Time:", execution_time, "seconds")

if __name__ == "__main__":
    main()