genit
==============

signal generator for CCRMA Music 256a w/ Ge Wang


Building 
==============

```
make
```

Usage
==============

genit is a CLI signal generator tool.

```sh
genit
     - print the usage text and run with default values
     
genit -h
     - print the usage text

genit [type] [frequency] [width] [input]

    [type]: --sine | --saw | --pulse | --noise | --impulse
            Default value: --sine
    [frequency]: an integer > 0 representing the frequency in Hz 
                 (only applicable to some signal types)
                 Default value: 440
    [width]: pulse width (only applicable to some signal types). 
             Must be a floating point value between 0.0 and 1.0, exclusive.
             Default value: 0.5
    [input]: --input - if the input flag is given, genit will take an input
             signal and multiply it by the generated signal.

    Note: All flags are optional and have default values as described, but
          flags must be entered in order. For example, if you mean to specify a
          width, you must also specify a type and a frequency. Providing
          a value for a signal type that does not support that flag is
          allowed. The value will be ignored.
```

Comments
==============

In deciding how to filter the signal type flags, I had a couple of options. I could define a global and switch on that inside the callback, or I could pass `RtAudio::openStream` a pointer to `userData` that contained things like signal type, frequency, etc. I felt that due to the smaller nature of the project, globals would work fine. In larger projects, I'll user the `userData` pointer.

Difficulties
==============

The only difficulty I came up against was a mathematical one. I could not remember how to find the number of samples in a period for a while. That calculation is simply `sampling_rate * fundamental_period`.

Collaborators
==============

This project is forked from Ge Wang's [HelloSine](https://ccrma.stanford.edu/courses/256a-fall-2014/lectures/1b/HelloSine/).
