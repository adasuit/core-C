#include "setup.h"
#include "errors.h"

struct Ports leds_info[] = {
    {"power"  , 26},
    {"network", 21},
    {"shift",   23}
};

struct Ports buttons_info[] = {
    {"cease"  , 24} // terminate the program
};

struct Ports buzzers_info[] = {
    {"all"    , 17} // only buzzer I am using
};

// get total number of each component
int component_total(int component)
{
    int size;
    switch(component) {
        case 0: // LEDS
            size = sizeof(leds_info) / sizeof(leds_info[0]);
            break;
        
        case 1: // BUTTONS
            size = sizeof(buttons_info) / sizeof(buttons_info[0]);
            break;
            
        case 2: // BUZZERS
            size = sizeof(buzzers_info) / sizeof(buzzers_info[0]);
            break;

        default:
            size = 0;
            break;
    }
    
    return size;
}

// initialise the gpio chip
int chip_init(struct gpiod_chip **chip)
{
    *chip = gpiod_chip_open_by_name(GPIO_CHIP_NAME);
    if (!(*chip)) {
        perror("Error opening GPIO chip");
        return 1;
    }
    return 0;
}

// initalise lines and configure them for input/output
int line_init(struct gpiod_line **line, struct gpiod_chip *chip, struct Ports *port, bool mode)
{
    *line = gpiod_chip_get_line(chip, port->pin);
    if (!(*line)) {
        perror("Error opening GPIO line");
        gpiod_chip_close(chip);
        return 1;
    }
    if (mode) {
        // input
        if (gpiod_line_request_input(*line, port->name) < 0) {
            perror("Error configuring GPIO line for input");
            gpiod_line_release(*line);
            gpiod_chip_close(chip);
            return 1;
        }    
    } else {
        // output
        if (gpiod_line_request_output(*line, port->name, 0) < 0) {
            perror("Error configuring GPIO line");
            gpiod_line_release(*line);
            gpiod_chip_close(chip);
            return 1;
        }
    }
    
    return 0;
}

// initialise LED array with LED output lines
int leds_init(struct gpiod_line *leds[], struct gpiod_chip *chip)
{
    int size = component_total(0);
    int i = 0;
    for (i = 0; i < size; i++) {
        line_init(&leds[i], chip, &leds_info[i], OUTPUT);
    }
    return 0;
}

// initialise button array with Button input lines
int buttons_init(struct Button *buttons, struct gpiod_chip *chip)
{
    int size = component_total(1);
    int i = 0;
    for (i = 0; i < size; i++) {
        line_init(&buttons[i].call, chip, &buttons_info[i], INPUT);
        buttons[i].sleep = 0; // by default buttons aren't on cooldown
    }
    return 0;
}

// initialise Buzzer array with Buzzer output lines
int buzzers_init(struct gpiod_line *buzzers[], struct gpiod_chip *chip)
{
    int size = component_total(2);
    int i = 0;
    for (i = 0; i < size; i++) {
        line_init(&buzzers[i], chip, &buzzers_info[i], OUTPUT);
    }
    return 0;
}

// release all components
void clean_resources(struct gpiod_line *leds[], struct Button *buttons, struct gpiod_line *buzzers[])
{
    int i = 0;

    for (i = 0; i < component_total(0); i++) {
        gpiod_line_release(leds[i]);
    }

    for (i = 0; i < component_total(1); i++) {
        gpiod_line_release(buttons[i].call);
    }

    for (i = 0; i < component_total(2); i++) {
        gpiod_line_release(buzzers[i]);
    }
}
