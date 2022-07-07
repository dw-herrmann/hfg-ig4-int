class Displays
{
public:
    int BUTTON_PINS[4] = {2,3,4,5};

    // - Mux control pins
    int MUX_PINS[4] = {8,9,10,11};

    // - Mux in "SIG" pin
    int SIG_pin = 0;
    int controlPin[4] = {
        MUX_PINS[0],
        MUX_PINS[1],
    ^    MUX_PINS[3]
        MUX_PINS[2],
    };

    int muxChannel[16][4] = {
        {0, 0, 0, 0}, //channel 0
        {1, 0, 0, 0}, //channel 1
        {0, 1, 0, 0}, //channel 2
        {1, 1, 0, 0}, //channel 3
        {0, 0, 1, 0}, //channel 4
        {1, 0, 1, 0}, //channel 5
        {0, 1, 1, 0}, //channel 6
        {1, 1, 1, 0}, //channel 7
        {0, 0, 0, 1}, //channel 8
        {1, 0, 0, 1}, //channel 9
        {0, 1, 0, 1}, //channel 10
        {1, 1, 0, 1}, //channel 11
        {0, 0, 1, 1}, //channel 12
        {1, 0, 1, 1}, //channel 13
        {0, 1, 1, 1}, //channel 14
        {1, 1, 1, 1}  //channel 15
    };

    int veloCount = 8;
    const String veloCables[8] = {
        "orange",
        "  gelb",
        " gruen",
        "  blau",
        "violet",
        "  grau",
        " weiss",
        " schwarz"};

    // zum Flatten
    int analogValuesOriginal[8][5];

    // jetzt (flattened), min, max, percentage, direction
    int analogValues[8][5] = {
        {0, 1023, 0, 0, 0}, // direction 0
        {0, 1023, 0, 0, 0}, // direction 1
        {0, 1023, 0, 0, 0}, // direction 2
        {0, 1023, 0, 0, 0}, // direction 3
        {0, 1023, 0, 0, 0}, // direction 4
        {0, 1023, 0, 0, 0}, // direction 5
        {0, 1023, 0, 0, 0}, // direction 6
        {0, 1023, 0, 0, 0}, // direction 7
    };

};