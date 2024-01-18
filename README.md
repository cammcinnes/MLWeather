# MLWeather

## Recording Data

This project utilizes a Arduino MKR 1010 WIFI and a BME680 to record time-stamped weather metrics every hour and send them to a google sheet to be analyzed. The weather metrics recorded are temperature, pressure, humidty, gas resistance, a calculated Air Quality Score(https://github.com/G6EJD/BME680-Example) and Altitude data. The arduino utilizes the pushingbox api (api.pushingbox.com) to send a get request to a google web app made with google app script.

## Data Analysis

Work In Progress

References:

- Inspiration from https://github.com/sborsay/Arduino_Wireless/blob/master/MKR1000_to_GoogleSheet_viaPushingBox
- Used code from this repository for personal use in calculating IAQ index https://github.com/G6EJD/BME680-Example/blob/master/ESP32_bme680_CC_demo_02.ino
- IAQ for BME680 by David Bird: https://github.com/G6EJD/BME680-Example
