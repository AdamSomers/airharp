#include "Harp.h"

Harp* Harp::sInstance = NULL;

Harp::Harp()
: numStrings(40)
{
    for (int i = 0; i < numStrings; ++i)
    {
        strings.push_back(new Karplus(0.009));
        accumulators.push_back(SampleAccumulator());
        accumulators.back().SetInput(strings.back());
        accumulators.back().SetSamplesPerPixel(6);
    }

    for (int i = 0; i < numStrings; ++i)
    {
        AudioServer::GetInstance()->AddClient(&accumulators.at(i), 0);
    }
}

Harp::~Harp()
{
    for (int i = 0; i < strings.size(); ++i)
    {
        delete strings.at(i);
    }
}

void Harp::NoteOn(int num, int note, int velocity)
{
    strings.at(num)->NoteOn(note, velocity);
}

void Harp::ExciteString(int num, int note, int velocity, float* buff, int bufferSize)
{
    strings.at(num)->NoteOn(note, velocity, buff, bufferSize);
}

Harp* Harp::GetInstance()
{
    if (!sInstance)
    {
        sInstance = new Harp;
    }
    return sInstance;
}