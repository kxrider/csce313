void swap (int &x, int &y) { 

    int temp = x;

    x = y;

    y = temp;

}


class Mutex{

private:

//add necessary member variables
int x;
int y;

public:

    Mutex (){ //fill this constructor
        x = 0;
        y = 1;
    }  


void lock()
{
  while (x == 1);
  swap (x, y);
}
void Unlock()
{
  while (x == 0);
  swap(x, y);
}

};


