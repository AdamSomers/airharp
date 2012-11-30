#ifndef __AirHarp__Harp__
#define __AirHarp__Harp__

#include <iostream>

//#include "MidiServer.h"
#include "Voices.h"
#include "SignalGenerators.h"

class Harp
{
public:
    Harp();
    ~Harp();
    void NoteOn(int num, int note, int velocity);
    void ExciteString(int num, int note, int velocity, float* buffer, int bufferSize);
    void AddString();
    void RemoveString();
    int GetNumStrings() const { return numStrings; }
    static Harp* GetInstance();
    std::vector<SampleAccumulator*>& GetBuffers() { return accumulators; }
    std::vector<Karplus*>& GetStrings() { return strings; }
    
private:
    void Cleanup();
    void Init();
    
    static Harp* sInstance;
    std::vector<SampleAccumulator*> accumulators;
    std::vector<Karplus*> strings;
    Adder* mixer;
    Multiplier* outputGain;
    int numStrings;
};

#endif /* defined(__AirHarp__Harp__) */
