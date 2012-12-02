#include "AudioServer.h"
#include "GLStuff.h"

int main(int argc, const char** argv)
{
    RtAudioDriver driver(256);    // init rtaudio
    GLStuff::GLStartup(argc, argv);
    std::cout << "byebye!\n";
    return 0;
}