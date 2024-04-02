/*
 * This example turns the ESP32 into a Bluetooth LE gamepad that presses buttons and moves axis
 *
 * At the moment we are using the default settings, but they can be canged using a BleGamepadConfig instance as parameter for the begin function.
 *
 * Possible buttons are:
 * BUTTON_1 through to BUTTON_16
 * (16 buttons by default. Library can be configured to use up to 128)
 *
 * Possible DPAD/HAT switch position values are:
 * DPAD_CENTERED, DPAD_UP, DPAD_UP_RIGHT, DPAD_RIGHT, DPAD_DOWN_RIGHT, DPAD_DOWN, DPAD_DOWN_LEFT, DPAD_LEFT, DPAD_UP_LEFT
 * (or HAT_CENTERED, HAT_UP etc)
 *
 * bleGamepad.setAxes sets all axes at once. There are a few:
 * (x axis, y axis, z axis, rx axis, ry axis, rz axis, slider 1, slider 2)
 *
 * Library can also be configured to support up to 5 simulation controls
 * (rudder, throttle, accelerator, brake, steering), but they are not enabled by default.
 *
 * Library can also be configured to support different function buttons
 * (start, select, menu, home, back, volume increase, volume decrease, volume mute)
 * start and select are enabled by default
 */

#include <Arduino.h>
#include <BleGamepad.h>

#define UART_TX 5
#define UART_RX 6
#define DEBUG_UART_TX 39
#define DEBUG_UART_RX 38

BleGamepad bleGamepad;
char buffer[100];


void setup()
{
    //Serial.begin(115200, SERIAL_8N1, DEBUG_UART_RX, DEBUG_UART_TX);
	Serial.begin(115200);
    Serial.println("Starting BLE work!");
    Serial1.begin(115200, SERIAL_8N1, UART_RX, UART_TX);
    bleGamepad.begin();
    // The default bleGamepad.begin() above enables 16 buttons, all axes, one hat, and no simulation controls or special buttons
}

uint8_t hexToNibble(char hex)
{
	if (hex >= '0' && hex <= '9')
	{
		return hex - '0';
	}
	else if (hex >= 'A' && hex <= 'F')
	{
		return hex - 'A' + 10;
	}
	else if (hex >= 'a' && hex <= 'f')
	{
		return hex - 'a' + 10;
	}
	else
	{
		return 0;
	}
}

void getAxes(uint8_t *values)
{
    // Wait for newline
    char lastChar = 0;
    uint32_t index = 0;
    while (lastChar != '\n' && index < sizeof(buffer) - 1)
    {
        if (Serial1.available())
        {
            lastChar = Serial1.read();
            buffer[index++] = lastChar;
        }
    }
	Serial.print("Received:");
	Serial.println(buffer);

    // Parse the values
    for (size_t i = 0; i < 6; i++)
    {
        values[i] = hexToNibble(buffer[i * 2]) << 4 | hexToNibble(buffer[i * 2 + 1]);
		Serial.println(values[i]);
    }
}

void loop()
{
	uint8_t inputValues[6];
	int16_t axes[6];

	getAxes(inputValues);

	// Scale the input values to the range of the axes
	// 0-100 -> 0-32767
	for (size_t i = 0; i < 6; i++)
	{
		axes[i] = inputValues[i] * 32767 / 100;
	}


    if (bleGamepad.isConnected())
    {
        bleGamepad.setAxes(axes[0], axes[1], axes[2], axes[3], axes[4], axes[5], 0, 0);
        // All axes, sliders, hats etc can also be set independently. See the IndividualAxes.ino example

    }
}