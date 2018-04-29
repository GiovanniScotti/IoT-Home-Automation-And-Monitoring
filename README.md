# IoT - Home Automation And Monitoring

This is the repository of Giovanni Scotti and Angelo Claudio Re for the project of the 
Internet Of Things course attended at Politecnico di Milano.

## Brief Description
The system aims to offer a smart solution to home automation and monitoring needs.
It is intended for those kind of users who want to visualize information about their home and control devices remotely.

The structure of the system consists of a back-end provided by a Raspberry Pi 3 board, which is in charge of managing MQTT 
as message broker and running Node-RED. The Pi 3 gathers data coming from ESP boards provided with sensors and send them to ThingSpeak 
to store information into the cloud and to open up further analysis.

The front end is offered by Node-RED itself through a user-friendly and web-accessible dashboard that can be easily reached from
any device connected to the same network or to the Internet by means of port forwarding.

The system must be secure. This is why authentication is required in order to access the dashboard.
Moreover, MQTT clients provide credentials to the broker to join the network.
Last but not least, power consumption has been taken into account: battery-powered ESP modules enter a deep sleep state after sending data.

## Documentation
Download the latest version [here](https://github.com/GiovanniScotti/IoT-Home-Automation-And-Monitoring/doc/pdf/ham_doc.pdf)

## Team Members
* Angelo Claudio Re
* Giovanni Scotti
