# Sup Usage Manual

- [Initial Setup](#initial-setup)
  - [1. Connecting Your Device](#1-connecting-your-device)
    - [Accessing the Configuration Page](#accessing-the-configuration-page)
    - [Setting up the Database Connection](#setting-up-the-database-connection)
    - [Connect to WiFi](#connect-to-wifi)
    - [Troubleshooting](#troubleshooting)
  - [2. Configuring the Web App](#2-configuring-the-web-app)
- [Web App Usage](#web-app-usage)
- [Device Buttons](#device-buttons)
- [Configuration Mode](#configuration-mode)
- [Firmware Updates](#firmware-updates)

## Initial Setup

Before you begin, make sure you have an account set up for the web app. There is *no* public sign-up UI, so ask Tyler for an account.

### 1. Connecting Your Device

#### Accessing the Configuration Page

Connect the device to a power source. Since the device doesn't yet know how to connect to WiFi, after the initial boot-up sequence it will enter configuration mode and display "WiFi Error/Use Web Config"

In configuration mode, the device will host its own WiFi network. To access the configuration, use your computer or phone and connect to this network. (Ask Tyler for the default network name/SSID and password.)
Then, go to <http://sup.local> or <http://192.168.0.1> (note the `http` and **not** `https`!) to access the configuration interface:

![Configuration interface](imgs/config1.png)

#### Setting up the Database Connection

First, we will set up the database connection. Click on the Database Connection link:

![Database connection setup](imgs/config2.png)

In the email and password fields, enter the credentials of your account on the web app. The device will use this to log into the database to retrieve the display data. In the display data location field, enter where you want *your* device to read the data from. This could be, for example, your name (without spaces or special characters), but you can choose anything alphanumeric.

The display data location is the location in the database where your device looks for what to display. Your friend can write display data to this location from the web app, which your device will then pick up and display.
The choice is yours, just make sure to tell your friend later!

When you're done, click update and follow the link back to the main configuration interface.

#### Connect to WiFi

Next, we will connect your device to WiFi. Click on the Wi-Fi Settings link:

![WiFi settings](imgs/config3.png)

Use WPA2-PSK if you have a normal home WiFi network with only a name and password. Use WPA2-Enterprise if you have a work or school WiFi network with a username/password login. You only need to use one of them.

Enter the WiFi credentials for either one, then click connect. After 1 second, the device should try to connect to WiFi.

#### Troubleshooting

If the display shows "WiFi Error/Use Web Config" again, then it has failed to connect to WiFi. It will host a network again for you to connect to and reconfigure. Follow the same steps as above and try again (you can skip the database connection).
Double check that you have the correct network name and password, and that the signal is strong enough.

NOTE: Due to hardware limitations, *the device only works on 2.4GHz WiFi networks*. Double check that the network you're trying to connect to is not 5GHz if you're having issues.

If the device connects to WiFi successfully, but shows "DB/Error", then it has failed to connect to the database. This is usually caused by incorrect login credentials.
In this case, you need to *put the device into configuration mode manually*, then follow the instructions above to set up the database connection again.

### 2. Configuring the Web App

## Web App Usage

## Device Buttons

## Configuration Mode

## Firmware Updates
