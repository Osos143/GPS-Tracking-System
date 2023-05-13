# **GPS Distance Calculation & LED Indicator System**
This project utilizes the TM4C123GH6PM LaunchPad to implement a system that calculates the distance traveled using GPS coordinates. It also provides LED indicators to signify the proximity to the destination.


# **Functionality**

**1)** The GPS subsystem stores the coordinates of the start point.  
**2)** Once the destination point is reached, the GPS subsystem stores the coordinates of the end point and calculates the total distance traveled by the user.  
**3)** The system provides the following output indications:  
Stage 1: The built-in LED turns on (green) when the target destination is reached.
Stage 2: The built-in LED turns on (yellow) when the target destination is about to be reached (within 5 meters).
Stage 3: The built-in LED turns on (red) when the target destination is far away (distance > 5 meters).


# **Trajectory Criteria**

**1)** The total distance between the start and end points should be greater than 100 meters.  
**2)** The path from the start point to the end point should not be a straight line.  
**3)** The starting point can be selected freely using Google Maps.  
**4)** The software calculates the distance between the starting point and the end point and compares it with the distance obtained from Google Maps.  
**5)** The calculated distance should have an error margin of no more than 5% compared to the distance shown by Google Maps.
