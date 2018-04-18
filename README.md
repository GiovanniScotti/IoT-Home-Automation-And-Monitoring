# IoT - Home Automation And Monitoring

This is the repository of Giovanni Scotti and Angelo Claudio Re for the project of the 
Internet Of Things course attended at Politecnico di Milano.

## Brief Description
The system aims to offer a smart solution to home automation and monitoring needs.
It is intended for those kind of users who want to visualize information about their home and control devices remotely.

The structure of the system consists of a back-end provided by a Raspberry Pi 3 board, which is in charge of managing MQTT 
as message broker and running node-red. The Pi 3 gathers data coming from an ESP-12E board provided with sensors and send them to ThingSpeak 
to store information into the cloud and to open up further analysis.

The front end is offered by node-red itself through a dashboard that can be easily accessed from any device connected to the same network
or to the Internet by means of port forwarding.

The system must be secure. This is why authentication is required in order to access the dashboard.
Last but not least, power consumption has been taken into account: the ESP-12E module enters a deep sleep state after sending data.

## Team Members
* Angelo Claudio Re
* Giovanni Scotti
