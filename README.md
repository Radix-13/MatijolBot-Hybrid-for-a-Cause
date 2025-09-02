# MatijolBot-Hybrid-for-a-Cause
This is an river-cleaning robot can move on both water and land, efficiently collecting waste and pollution. Its amphibious design makes it easily deployable, offering an eco-friendly solution for pollution control in hard-to-reach areas
# Abstract : 
An amphibious robot called MatiJolBot is created to collect waste from rivers and their banks in an efficient manner. In difficult-to-reach areas, the robot tackles pressing environmental issues brought on by river pollution by switching between aquatic and land navigation with ease. Everything about MatiJolBot (concept, process, workflow) is described in this project focusing on the environment and also the costing as one vehicle can do multiple tasks. 
# System Architecture: 
The robot is powered by a 3S Li-ion battery pack (≈12 V) that splits into three branches: one motor driver for the left and right wheel DC motors (allowing operation on both road and river surfaces), another motor driver for the conveyor belt that collects floating or roadside waste, and a buck converter supplying 5 V to the Arduino Uno, Bluetooth module, and sensors. The Arduino initializes the system, interprets Bluetooth commands for movement (forward, backward, left, right, stop) and conveyor control, and enforces safety logic. A single ultrasonic sensor is mounted at the backside to prevent collisions when reversing, while an ACS712 current sensor monitors conveyor motor load, stopping it if jamming or overload occurs. Manual reset ensures safe recovery after faults. This integrated design enables the robot to function as an amphibious cleaner, navigating smoothly on roads or rivers while safely operating its conveyor-based trash collection system.
#Cicuit
<img width="612" height="347" alt="Image" src="https://github.com/user-attachments/assets/f85e4148-8b1b-4cbb-b789-e706ea48cc1b" />


