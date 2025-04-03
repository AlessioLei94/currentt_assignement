# Currentt Assignment

The application has been developed using ESP-IDF v5.3 selecting ESP-32 S3 as target. The toolchain and required setup has been installed through the ESP-IDF extension of VS Code.

The implementation of the driver to communicate to the SHT40 sensor over I2C has not been tested on a real device so it might contains some errors, having access to a real device and being able to debug the data transferred over the serial bus would be required to make sure everything works properly.

Only minimal functionalities have been implemented to showcase coding style and match what was required by the assignment, more APIs could be added to make use of the full capabilites of the sensor.