# MLWeather
![IMG_4512](https://github.com/cammcinnes/MLWeather/assets/88452661/e6db920c-1c08-476a-8a70-76fd027186cc)

![IMG_4513](https://github.com/cammcinnes/MLWeather/assets/88452661/ae8fca63-dcb3-428b-8556-67bc65d34c6f)

## Recording Data

This project utilizes a Arduino MKR 1010 WIFI and a BME680 to record time-stamped weather metrics every hour and send them to a google sheet to be analyzed. The weather metrics recorded are temperature, pressure, humidity, gas resistance, a calculated air quality score (https://github.com/G6EJD/BME680-Example) and altitude data. The arduino utilizes the pushingbox api (api.pushingbox.com) to send a get request to a google web app made with google app script.

## Data Analysis

Work In Progress

References:

- Inspiration from https://github.com/sborsay/Arduino_Wireless/blob/master/MKR1000_to_GoogleSheet_viaPushingBox
- Used code from this repository for personal use in calculating IAQ index https://github.com/G6EJD/BME680-Example/blob/master/ESP32_bme680_CC_demo_02.ino
- IAQ for BME680 by David Bird: https://github.com/G6EJD/BME680-Example
