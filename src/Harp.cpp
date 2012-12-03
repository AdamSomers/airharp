#include "Harp.h"

Harp* Harp::sInstance = NULL;

#define MAX_STRINGS 30
#define SAMPS_PER_PIXEL 6

Harp::Harp()
: numStrings(1)
, mixer(NULL)
, outputGain(NULL)
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
        accumulators.back()->SetSamplesPerPixel(SAMPS_PER_PIXEL);
    }
    
    mixer = new Adder;
    
    for (int i = 0; i < numStrings; ++i)
    {
        mixer->AddInput(accumulators.at(i));
    }
    
    outputGain = new Multiplier;
    outputGain->SetA(mixer);
    outputGain->SetVal(1/12.f);
    
    AudioServer::GetInstance()->AddClient(outputGain, 0);
    AudioServer::GetInstance()->AddClient(outputGain, 1);

}

void Harp::Cleanup()
{
    for (int i = 0; i < strings.size(); ++i)
    {
        AudioServer::GetInstance()->EnterLock();
        mixer->RemoveInput(accumulators.at(i));
        AudioServer::GetInstance()->ExitLock();
        delete strings.at(i);
        delete accumulators.at(i);
    }
    if (outputGain) {
        delete outputGain;
        AudioServer::GetInstance()->RemoveClient(outputGain, 0);
        AudioServer::GetInstance()->RemoveClient(outputGain, 1);
    }
    if (mixer)
        delete mixer;

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
    accumulators.back()->SetSamplesPerPixel(SAMPS_PER_PIXEL);
    AudioServer::GetInstance()->EnterLock();
    mixer->AddInput(accumulators.back());
    AudioServer::GetInstance()->ExitLock();
    numStrings++;
    //outputGain->SetVal(fmax(1.f/numStrings, 1/12.f));
}

void Harp::RemoveString()
{
    if (numStrings == 1)
        return;
    AudioServer::GetInstance()->EnterLock();
    mixer->RemoveInput(accumulators.back());
    AudioServer::GetInstance()->ExitLock();
    delete accumulators.back();
    accumulators.pop_back();
    delete strings.back();
    strings.pop_back();
    numStrings--;
    //outputGain->SetVal(fmax(1.f/numStrings, 1/12.f));
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