#include "RtAudio.h"
#include <math.h>
#include <iostream>
#include <random>
#include <cstdlib>
#include <string.h>
using namespace std;

std::random_device rd;
std::mt19937 gen(rd());
std::normal_distribution<> d(0,0.333);

// our datetype
#define SAMPLE double
// corresponding format for RtAudio
#define MY_FORMAT RTAUDIO_FLOAT64
// sample rate
#define MY_SRATE 44100
// number of channels
#define MY_CHANNELS 2
// for convenience
#define MY_PIE 3.14159265358979

// global for frequency
SAMPLE g_freq;
// global sample number variable
SAMPLE g_t = 0;
// global for period
SAMPLE g_T;
// global for samples in a fundamental period
int samps_in_period;
// width of duty cycle
double width;
// duty cycle
int duty_cycle;
// Switch int for signal
int sig;
// Bool for mic on
bool micOn;


//-----------------------------------------------------------------------------
// name: usageText()
// desc: prints genit usage text
//-----------------------------------------------------------------------------
void usageText() {
    cerr << "genit is a CLI signal generator tool." << endl;
    cerr << endl;
    cerr << "genit -h" << endl;
    cerr << "     - print the usage text" << endl;
    cerr << endl;
    cerr << "genit [type] [frequency] [width] [input]" << endl;
    cerr << endl;
    cerr << "    [type]: --sine | --saw | --pulse | --noise | --impulse" << endl;
    cerr << "            Default value: --sine" << endl;
    cerr << "    [frequency]: an integer > 0 representing the frequency in Hz " << endl;
    cerr << "                 (only applicable to some signal types)" << endl;
    cerr << "                 Default value: 440" << endl;
    cerr << "    [width]: pulse width (only applicable to some signal types). " << endl;
    cerr << "             Must be a floating point value between 0.0 and 1.0, exclusive." << endl;
    cerr << "             Default value: 0.5" << endl;
    cerr << "    [input]: --input - if the input flag is given, genit will take an input" << endl; 
    cerr << "             signal and multiply it by the generated signal." << endl;
    cerr << endl;
    cerr << "    Note: All flags are optional and have default values as described, but" << endl;
    cerr << "          flags must be entered in order. For example, if you mean to specify a" << endl;
    cerr << "          width, you must also specify a type and a frequency. Providing" << endl;
    cerr << "          a value for a signal type that does not support that flag is" << endl;
    cerr << "          allowed. The value will be ignored." << endl;
    
    
}


//-----------------------------------------------------------------------------
// name: callme()
// desc: audio callback
//-----------------------------------------------------------------------------
int callme( void * outputBuffer, void * inputBuffer, unsigned int numFrames,
            double streamTime, RtAudioStreamStatus status, void * data ) {
    
    int samp;
    double val;
    
    // debug print something out per callback
    cerr << ".";

    // cast!
    SAMPLE * buffy = (SAMPLE *)outputBuffer;
    SAMPLE * inBuffy = (SAMPLE *)inputBuffer;
    
    // fill
    for( int i = 0; i < numFrames; i++ ) {
        // generate signal
        switch (sig) {
            case 1:
                buffy[i*MY_CHANNELS] = ::sin( 2 * MY_PIE * g_freq * g_t / MY_SRATE ); // Sine wave
                break;
                
            case 2:
                buffy[i*MY_CHANNELS] =  d(gen); //Gaussian noise
                break;
                
            case 3:
                buffy[i*MY_CHANNELS] = 0.0; // Impulse train
                if(i % samps_in_period == 0) { // condition: this sample is the beginning of the next fundamental period
                    buffy[i*MY_CHANNELS] = 1.0;
                }
                break;
                
            case 4:
                if ((i % samps_in_period) > duty_cycle) { // Pulse wave
                    buffy[i*MY_CHANNELS] = 0.0;
                } else {
                    buffy[i*MY_CHANNELS] = 1.0;
                }
                break;
                
            case 5:
                if (i % samps_in_period == 0) { // Triangle wave
                    buffy[i*MY_CHANNELS] = 0.0;
                } else if ((i % samps_in_period) % duty_cycle == 0){
                    buffy[i*MY_CHANNELS] = 1.0;
                } else if ((i % samps_in_period) < duty_cycle) {
                    samp = i % samps_in_period;
                    val = 0.0 + (double) (samp * (double)(1.0 / duty_cycle));
                    buffy[i*MY_CHANNELS] = val;
                    
                } else if ((i % samps_in_period) > duty_cycle) {
                    samp = i % samps_in_period;
                    val = 1.0 - (double) ((samp - duty_cycle) * (double)(1.0 / (samps_in_period - duty_cycle)));
                    buffy[i*MY_CHANNELS] = val;
                }
                break;
                
            default:
                buffy[i*MY_CHANNELS] = ::sin( 2 * MY_PIE * g_freq * g_t / MY_SRATE ); // Sine wave
                break;
        }
        
        if (micOn) { // use input flag
            buffy[i*MY_CHANNELS] = buffy[i*MY_CHANNELS] * inBuffy[i*MY_CHANNELS];
        }
      
        // copy into other channels
        for( int j = 1; j < MY_CHANNELS; j++ )
            buffy[i*MY_CHANNELS+j] = buffy[i*MY_CHANNELS];
            
        // increment sample number
        g_t += 1.0;
    }
    
    return 0;
}




//-----------------------------------------------------------------------------
// name: main()
// desc: entry point
//-----------------------------------------------------------------------------
int main( int argc, char ** argv ) {
    
    // instantiate RtAudio object
    RtAudio adac;
    // variables
    unsigned int bufferBytes = 0;
    // frame size
    unsigned int bufferFrames = 512;
  
    if (argc == 1) {
      usageText();
    }
    
    if (argc >= 2) {
        if (strcmp(argv[1], "-h") == 0) {
            usageText();
            exit(1);
        } else if (strcmp(argv[1], "--sine") == 0) {
            sig = 1;
        } else if (strcmp(argv[1], "--noise") == 0) {
            sig = 2;
        } else if (strcmp(argv[1], "--impulse") == 0) {
            sig = 3;
        } else if (strcmp(argv[1], "--pulse") == 0) {
            sig = 4;
        } else if (strcmp(argv[1], "--saw") == 0) {
            sig = 5;
        } else {
            cerr << "Signal specified is not available. Defaulting to sine..." << endl;
            sig = 1;
        }
        
    } else {
        cerr << "No signal specified. Defaulting to sine..." << endl;
        sig = 1;
    }
    
    if (argc >= 3) {
        try {
            g_freq = stoi(argv[2]);
        } catch (invalid_argument& e) {
            cerr << "Please enter a properly formatted frequency next time. Defaulting to 440..." << endl;
            g_freq = 440;
        } catch (out_of_range& e) {
            cerr << "Frequency too large for C++. Defaulting to 440..." << endl;
            g_freq = 440;
        }
        
        if (g_freq == 0) {
            cerr << "A frequency of 0 is not allowed. Setting to default of 440..." << endl;
            g_freq = 440;
        }
        
        g_T = 1 / g_freq;
        samps_in_period = (int) ((1/g_freq) * MY_SRATE);
    } else {
        g_freq = 440;
        g_T = 1 / g_freq;
        samps_in_period = (int) ((1/g_freq) * MY_SRATE);
    }
    
    if (argc >= 4) {
        width = atof(argv[3]);
        if ((width == 0) || (width == 1.0) || (width < 0) || (width > 1.0)) {
            cerr << "Widths at or beyond 0.0 and 1.0 are not allowed. Setting to default width of 0.5" << endl;
            width = 0.5;
        }
        duty_cycle = (int) (samps_in_period * width);
    } else {
        width = 0.5;
        duty_cycle = (int) (samps_in_period * width);
    }
    
    if (argc >= 5) {
        if
            (strcmp(argv[4], "--input") == 0) {
            micOn = true;
        } else {
            micOn = false;
        }
    } else {
        micOn = false;
    }
    
    // check for audio devices
    if( adac.getDeviceCount() < 1 ) {
        // nopes
        cout << "no audio devices found!" << endl;
        exit( 1 );
    }

    // let RtAudio print messages to stderr.
    adac.showWarnings( true );

    // set input and output parameters
    RtAudio::StreamParameters iParams, oParams;
    iParams.deviceId = adac.getDefaultInputDevice();
    iParams.nChannels = MY_CHANNELS;
    iParams.firstChannel = 0;
    oParams.deviceId = adac.getDefaultOutputDevice();
    oParams.nChannels = MY_CHANNELS;
    oParams.firstChannel = 0;
    
    // create stream options
    RtAudio::StreamOptions options;

    // go for it
    try {
        // open a stream
        adac.openStream( &oParams, &iParams, MY_FORMAT, MY_SRATE, &bufferFrames, &callme, (void *)&bufferBytes, &options );
    }
    catch( RtError& e ) {
        // error!
        cout << e.getMessage() << endl;
        exit( 1 );
    }

    // compute
    bufferBytes = bufferFrames * MY_CHANNELS * sizeof(SAMPLE);
    
    // test RtAudio functionality for reporting latency.
    cout << "stream latency: " << adac.getStreamLatency() << " frames" << endl;

    // go for it
    try {
        // start stream
        adac.startStream();

        // get input
        char input;
        std::cout << "running... press <enter> to quit (buffer frames: " << bufferFrames << ")" << endl;
        std::cin.get(input);
        
        // stop the stream.
        adac.stopStream();
    }
    catch( RtError& e ) {
        // print error message
        cout << e.getMessage() << endl;
        goto cleanup;
    }
    
cleanup:
    // close if open
    if( adac.isStreamOpen() )
        adac.closeStream();
    
    // done
    return 0;
}
