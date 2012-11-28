//#include "AudioServer.h"
//#include "Harp.h"
#include "GLStuff.h"

int main(int argc, const char** argv)
{
//    RtAudioDriver driver;    // init rtaudio
//    Harp::GetInstance(); // init insaniac
    GLStuff::GLStartup(argc, argv);
    std::cout << "byebye!\n";
    return 0;
}