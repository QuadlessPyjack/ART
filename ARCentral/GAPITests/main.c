#include <stdio.h>
#include "gapi.h"

int main()
{
    gapiInit();

    //gapiDrawTarget(10, 4);
    //gapiDrawMarker(15, 5);

    for (int i = 0; i < 10; ++i)
    {
        gapiDrawTarget(25, 50);
        gapiUpdate();
    }


    return 0;
}
