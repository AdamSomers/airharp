#include "Harp.h"

Harp* Harp::sInstance = NULL;

Harp::Harp()
: numStrings(10)
{
    for (int i = 0; i < numStrings; ++i)
    {
        p.AddVoice(new Karplus(0.009));
    }
    AudioServer::GetInstance()->AddClient(&p, 0);
    AudioServer::GetInstance()->AddClient(&p, 1);
}

void Harp::NoteOn(int note, int velocity)
{
    p.NoteOn(note, velocity);
}

Harp* Harp::GetInstance()
{
    if (!sInstance)
    {
        sInstance = new Harp;
    }
    return sInstance;
}