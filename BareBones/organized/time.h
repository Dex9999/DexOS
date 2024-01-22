// regular time function! :D nothing to see here

// about a second
int test_time = 15000000;
int wait(float seconds) {
    for (volatile int i = 0; i < seconds * test_time; i++){
        // wait
    }
    return 0;
}