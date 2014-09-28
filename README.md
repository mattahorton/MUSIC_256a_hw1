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
