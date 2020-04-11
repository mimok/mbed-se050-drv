# A lightweight Mbed SE050 Driver

## Introduction

 This library provides a set of functions allowing to interact with a SE050 chip.
 While NXP already provides such a library, it seems necessary to make available 
 an alternative implementation for the following reasons:
 
 * NXP Trust&Plug MW library uses a lot of stack memory. Memory usage almost exceeds what is avalibale on small 
 chip like murata Type ABZ chip. Therefore, there is no room for additional features like the lorawan one.
 * NXP Trust&Plug MW library is made of both proprietary and open source pieces of code and therefore is not
 suitable for fully open source product.
 
 This library is designed to use as little memory as possible. Indeed, all APDU processings are performed
 in place in the APDU buffer. Therefore a single 900-byte buffer is used for all command processing 
 (unfortunately an additional buffer is used by NXP T1oI2C library which is used by the product).
 
 ## Implemented features
 
 At the time of writing this documentation, only attested I2CM commands are implemented. This feature 
 allows an host processor to request attested read/write operations from/to an I2C sensor directly connected to the SE050
 chip. Data read from the sensor maybe trusted even if the host processor is compromised as it has no
 direct access to the I2C sensor.
 
 ## Installation
 
 This library can be added to an mbed project by entering the root directory of your projet and typing:
 ```bash
 mbed add <library git>
 ```