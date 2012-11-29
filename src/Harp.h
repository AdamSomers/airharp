#ifndef __AirHarp__Harp__
#define __AirHarp__Harp__

#include <iostream>

//#include "MidiServer.h"
#include "Voices.h"

class Harp
{
public:
    Harp();
    void NoteOn(int note, int velocity);
    int GetNumStrings() const { return numStrings; }
    static Harp* GetInstance();
    
private:
    static Harp* sInstance;
    Poly p;
    int numStrings;
};

#endif /* defined(__AirHarp__Harp__) */
