#include <strsafe.h>
#include <windows.h>

#define NUM_THREADS 10
#define BUF_SIZE 255

struct Package {
    int tid;
    int number;
};

void display_message(HANDLE hScreen, int tid, int number, const char* message) {
    // A character array
    TCHAR message_buffer[BUF_SIZE];
    size_t cchStringSize;
    // DWORD is a Windows-specific type (unsigned 32-bit)
    DWORD dwChars;

    // Print using thread-safe functions
    // Print a string to the buffer and checks to make sure it doesn't exceed
    // length
    StringCchPrintf(message_buffer, BUF_SIZE,
        TEXT("Printing number %d from thread %d. %s\n"), tid,
        number, message);
    // Check for a buffer overrun
    StringCchLength(message_buffer, BUF_SIZE, &cchStringSize);
    // Write character string to console
    WriteConsole(hScreen, message_buffer, cchStringSize, &dwChars, NULL);

    // Just put the thread to sleep for a little
    Sleep(1000);
}

// WINAPI is a macro that specifies the use of the Windows calling
// convention
// LPVOID is just a void pointer
DWORD WINAPI boring_thread_function(LPVOID lpParam) {
    // Cast our void pointer to our defined struct
    Package local_package = *(Package*)lpParam;

    // Get Handle to the screen
    HANDLE hStdout = NULL;
    // Get handle to stdout
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    // Call our function with thread-safe print
    display_message(hStdout, local_package.tid, local_package.number,
        "Hello, there!");

    return 0;
}

int main() {
    // Create an array of handles
    HANDLE array_of_handles[NUM_THREADS] = { 0 };
    // Create an array of struct as arguments
    Package packages[NUM_THREADS];

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        packages[i].tid = i;
        packages[i].number = i * i;

        array_of_handles[i] = CreateThread(NULL, 0, boring_thread_function, &packages[i], 0, NULL);
    }

    // Wait for a number of threads to be in the signalled state
    // Infinite states to wait forever
    WaitForMultipleObjects(NUM_THREADS, array_of_handles, true, INFINITE);

    // Clean up by closing open object handles
    for (int i = 0; i < NUM_THREADS; i++) {
        CloseHandle(array_of_handles[i]);
    }

    return 0;
}