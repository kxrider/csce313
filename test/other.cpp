Semaphore fullSlots = 0; 
Semaphore emptySlots = bufSize;//Initially all empty
Semaphore mutex = 1; 
int itemct = 0;
int consumect = 0;
Producer(item) {
    emptySlots.P(); // Wait until space
    mutex.P(); // Wait until machine free
    Enqueue(item);
    itemct++;
    if (ct == 5) {
        itemct = 0;
        for (int i = 0; i < 5; i++)
            fullSlots.V();
    }
    mutex.V(); // Notify there is more coke
}

Consumer() {
    fullSlots.P(); // Check if there’s a coke
    mutex.P(); // Wait until machine free
    item = Dequeue();
    if (++consumect == 5) {
        consumect = 0;
        emptySlots.V();
    }
    mutex.V();
     // tell producer need more
    return item;
}

