// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// TSL2772
// This code is designed to work with the TSL2772_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/products

#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

void main()
{
    // Create I2C bus
    int file;
    char *bus = "/dev/i2c-1";
    if ((file = open(bus, O_RDWR)) < 0)
    {
        printf("Failed to open the bus. \n");
        exit(1);
    }
    // Get I2C device, TSL2772 I2C address is 0x39(57)
    ioctl(file, I2C_SLAVE, 0x39);
    
    // Select enable register(0x00 | 0x80)
    // Set Power ON, ALS Enabled(0x0F)
    char config[2] = {0};
    config[0] = 0x00 | 0x80;
    config[1] = 0x0F;
    write(file, config, 2);
    // Select ALS time register(0x01 | 0x80)
    // Atime = 2.73 ms, Max count = 1024(0xFF)
    config[0] = 0x01 | 0x80;
    config[1] = 0xFF;
    write(file, config, 2);
	// Select Proximity time control register(0x02 | 0x80)
    // Time = 2.73 ms(0xFF)
    config[0] = 0x02 | 0x80;
    config[1] = 0xFF;
    write(file, config, 2);
    // Select wait time register(0x03 | 0x80)
    // Wtime = 2.73 ms(0xFF)
    config[0] = 0x03 | 0x80;
    config[1] = 0xFF;
    write(file, config, 2);
    // Select control register(0x0F | 0x80)
    // Gain = 1x(0x20)
    config[0] = 0x0F | 0x80;
    config[1] = 0x20;
    write(file, config, 2);
    sleep(1);
    
    // Read 6 bytes of data from register(0x14 | 80)
    // c0Data LSB, c0Data MSB, c1Data LSB, c1Data MSB, proximity LSB, proximity MSB
    char reg[1] = {0x14 | 0x80};
    write(file, reg, 1);
    char data[6] = {0};
    if(read(file, data, 6) != 6)
    {
        printf("Erorr : Input/output Erorr \n");
    }
    else
    {	
        // Convert the data
        int c0Data = (data[1] * 256 + data[0]);
        int c1Data = (data[3] * 256 + data[2]);
        int proximity = (data[5] * 256 + data[4]);
        float CPL = (2.73 * 1.0) / 60.0;
        float luminance1 = (1 * c0Data - 1.87 * c1Data) / CPL;
        float luminance2 = (0.63 * c0Data - 1.00 * c1Data) / CPL;
        float luminance = 0.0;
        
        if((luminance1 > 0) && (luminance1 > luminance2))
        {
            luminance = luminance1;
        }
        else if((luminance2 > 0) && (luminance2 > luminance1))
        {
            luminance = luminance2;
        }
        
        // Output data to screen
        printf("Ambient Light luminance : %.2f lux \n", luminance);
        printf("Proximity of the device : %d \n", proximity);
    }
}
