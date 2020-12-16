# Introduction #
BLE tracking system based on ESP32 uC and nrf52832 iBeacon. The main idea is to stablish a net of beacons which
comunicate with the ESP32 masters via Bluetooth Low Energy on Beacon Mode.

Every beacon wake up every 1200ms and send their uuid, mayor, minor, battery and rssi. The most important parameter is the rssi because with that we can calculate the distance between masters and I can positionate them on a map.

The visualize tool is made with Python and Pygame, all the masters send data to a free to use broker and the console is listenning incoming messages in order to calculate the distance and situate them on the static map.
## Hardware ##
### ESP32 master and  nrf52832 ###
I have develop a simple esp32 board that can be powered via 220@VAC and every minute it listen to the slaves and calculate the distance between beacons.
| Board Specifications  |  ESP32 WROVER       |  nrf52832          |
| :--------------------:|:-------------------:| :----------------: | 
| Flash                 |         448kB       |      256 kB        | 
| RAM                   |        520kB        |       32 kB        | 
| Consumption on sleep  |         10uA        |    1.9 μA at 3 V   | 

### Images beacon ###
In the following image we can see the pcb of the esp32 beacon device and the master esp32 pcb:


<p align="center">
    <img src="https://github.com/RarceD/TFM--Bluetooth-Beacon-Tracking-/blob/master/Technical%20project/    Schematics/emiter_1.PNG" alt="drawing"  height= "200" width="200"/>
    <img src="https://github.com/RarceD/TFM--Bluetooth-Beacon-Tracking-/blob/master/Technical%20project/Schematics/emiter_2.PNG" alt="drawing"  height= "200" width="200"/>
</p>


The real boards:


<p align="center">
    <img src="https://github.com/RarceD/TFM--Bluetooth-Beacon-Tracking-/blob/master/Technical%20project/Schematics/3d_beacon_2.jpeg" alt="drawing"  height= "200" width="200"/>
    <img src="https://github.com/RarceD/TFM--Bluetooth-Beacon-Tracking-/blob/master/Technical%20project/Schematics/mechanical_beacon.PNG" alt="drawing"  height= "200" width="200"/>
</p>



### Images master listener ###
Kicad files also included:

<p align="center">
    <img src="https://github.com/RarceD/TFM--Bluetooth-Beacon-Tracking-/blob/master/Technical%20project/Schematics/receiver_2.PNG" alt="drawing"  height= "200"  width="200"/>
    <img src="https://github.com/RarceD/TFM--Bluetooth-Beacon-Tracking-/blob/master/Technical%20project/Schematics/receiver_1.PNG" alt="drawing"  height= "200"  width="200"/>
</p>


The real boards:
<p align="center">
    <img src="https://github.com/RarceD/TFM--Bluetooth-Beacon-Tracking-/blob/master/Technical%20project/Schematics/3d_antenna.jpeg" alt="drawing"  height= "200"  width="200"/>
    <img src="https://github.com/RarceD/TFM--Bluetooth-Beacon-Tracking-/blob/master/Technical%20project/Schematics/real_master_pcb.jpeg" alt="drawing"  height= "200"  width="200"/>
    <img src="https://github.com/RarceD/TFM--Bluetooth-Beacon-Tracking-/blob/master/Technical%20project/    Schematics/real_beacon_pcb.jpeg" alt="drawing"  height= "200"  width="200"/>
</p>


## Software ##
### Trilateration ###
If two elements on a net are listened by two masters the algorithm will calculate the middle distance of both of them acording to the strengh of the signal received. Appling trilateration we can obtein the position between three masters and only one beacon sending data. I don't cosider more than 3 masters detected the same beacon.


<p align="center">
    <img src="https://github.com/RarceD/TFM--Bluetooth-Beacon-Tracking-/blob/master/Technical%20project/Memmory/images/supermarket_3.PNG" alt="drawing"  height= "300"/>
</p>


## 3D Printing ##
The electronics need a case in order not to be touch by no people, thats why I have design a simple model in Freecad:


<p align="center">
    <img src="https://github.com/RarceD/TFM--Bluetooth-Beacon-Tracking-/blob/master/Technical%20project/    Schematics/model_master.PNG" alt="drawing"  height= "300" />
</p>

## Visualizer ##
For making the fastest prototype of the visualization tool I have program a pygame console that is connected to mqtt and always listening to the master messages.
I'm using free mqtt broker: 
```python 
 broker_address = "broker.mqttdashboard.com"
```
It has fall out just one time in more than 3 months, it seems right.
