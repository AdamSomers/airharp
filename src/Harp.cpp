#include "Harp.h"

Harp* Harp::sInstance = NULL;

#define MAX_STRINGS 30

Harp::Harp()
: numStrings(1)
{
    Init();
}

Harp::~Harp()
{
    Cleanup();
}

void Harp::Init()
{
    Cleanup();
    for (int i = 0; i < numStrings; ++i)
    {
        strings.push_back(new Karplus(0.009));
        accumulators.push_back(new SampleAccumulator());
        accumulators.back()->SetInput(strings.back());
        accumulators.back()->SetSamplesPerPixel(6);
    }
    
    for (int i = 0; i < numStrings; ++i)
    {
        AudioServer::GetInstance()->AddClient(accumulators.at(i), 0);
    }
}

void Harp::Cleanup()
{
    for (int i = 0; i < strings.size(); ++i)
    {
        AudioServer::GetInstance()->EnterLock();
        AudioServer::GetInstance()->RemoveClient(accumulators.at(i), 0);
        AudioServer::GetInstance()->ExitLock();
        delete strings.at(i);
        delete accumulators.at(i);
    }
    strings.clear();
    accumulators.clear();
}

void Harp::AddString()
{
    if (numStrings == MAX_STRINGS )
        return;

    strings.push_back(new Karplus(0.009));
    accumulators.push_back(new SampleAccumulator());
    accumulators.back()->SetInput(strings.back());
    accumulators.back()->SetSamplesPerPixel(6);
    AudioServer::GetInstance()->AddClient(accumulators.back(), 0);
    numStrings++;
}

void Harp::RemoveString()
{
    if (numStrings == 1)
        return;
    AudioServer::GetInstance()->EnterLock();
    AudioServer::GetInstance()->RemoveClient(accumulators.back(), 0);
    AudioServer::GetInstance()->ExitLock();
    delete accumulators.back();
    accumulators.pop_back();
    delete strings.back();
    strings.pop_back();
    numStrings--;
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