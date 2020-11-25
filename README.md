# Introduction #
The main idea is to create a net of ESP32 boards to track the nrf52832 devises, this elements comunicate with the master via Bluetooth Low Energy Beacon Mode, they wake up every 500ms and send their uuid, 
battery and rssi. The most important parameter is the rssi because with that we can convert to the distance between masters and I can set them in a map.  
## Hardware ##
### ESP32 master ###
I have develop a simple esp32 board that can be powered via 220@VAC and every minute it listen to the slaves and calculate the distance between both elements.
| Board Specifications  | Idea           
| ---------------|:----------------:| 
| Flash          |         m        | 
| SRAM           |         m        | 
### Considerations ###
If two elements on a net are listened by two masters the algorithm will calculate the middle distance of both of them acording to the strengh of the signal received.
Appling trilateration we can obtein the position between three masters and only one beacon sending.

### nrf52832 and ESP32 beacon ###

| Board Specifications  | Idea           
| ---------------|:----------------:| 
| Flash          |         m        | 
| SRAM           |         m        | 

### Images beacon ###
In the following image we can see the pcb of the esp32 beacon device and the master esp32 pcb:
<img src="https://github.com/RarceD/TFM--Bluetooth-Beacon-Tracking-/blob/master/Technical%20project/Schematics/emiter_1.PNG" alt="drawing" width="200"/>
<img src="https://github.com/RarceD/TFM--Bluetooth-Beacon-Tracking-/blob/master/Technical%20project/Schematics/emiter_2.PNG" alt="drawing" width="200"/>


### Images master listener ###

<img src="https://github.com/RarceD/TFM--Bluetooth-Beacon-Tracking-/blob/master/Technical%20project/Schematics/receiver_2.PNG" alt="drawing" width="200"/>
<img src="https://github.com/RarceD/TFM--Bluetooth-Beacon-Tracking-/blob/master/Technical%20project/Schematics/receiver_1.PNG" alt="drawing" width="200"/>


## 3D Printing ##
The electronics need a case in order not to be touch by no people, thats why I have design a simple model in Freecad:
<img src="" alt="drawing" width="400"/>

## Visualizer ##

For making the fastest prototype of the visualization tool I have program a pygame console that is connected to mqtt and always listening to the master messages.
I'm using mqtt broker: 
```python 
 broker_address = "broker.mqttdashboard.com"
```
