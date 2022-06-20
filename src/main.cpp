#include <iostream>
#include "wav.h"

int main(int argc, char const *argv[]){
    
    lay::Wav x;  
    x.play(argv[1]);
    return 0;
}
