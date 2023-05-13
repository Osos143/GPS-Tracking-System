# **GPS Distance Calculation & LED Indicator System**
This project is an implementation of a GPS tracking system using the TM4C123G LaunchPad microcontroller. The program stores the coordinates of the start point and end point, calculates the total distance traveled, and displays the progress towards the destination using built-in LEDs. Additionally, the program features an LCD display that shows the traveled distance and dynamically updates the remaining distance while in motion. The path from the start point to the end point is designed to follow a non-straight line that closely resembles a provided baseline path. To ensure accuracy, the program compares the calculated distance with the distance obtained from Google Maps, maintaining an error margin of <= 5%.


# **Functionality**

**1)** The GPS subsystem stores the coordinates of the start point.  
**2)** Once the destination point is reached, the GPS subsystem stores the coordinates of the end point and calculates the total distance traveled by the user.  
**3)** The system provides the following output indications:  
Stage 1: The built-in LED turns on (green) when the target destination is reached.  
Stage 2: The built-in LED turns on (yellow) when the target destination is about to be reached (within 5 meters).  
Stage 3: The built-in LED turns on (red) when the target destination is far away (distance > 5 meters).

## **Hardware Requirements**
- TM4C123G LaunchPad microcontroller
- GPS module that communicates using UART protocol
- Built-in LEDs
- Breadboard
- Jumper wires
- LCD display

## **Software Requirements**
- Keil uVision4 IDE
- Tera Term
- Google Maps


# **Trajectory Criteria**

**1)** The total distance between the start and end points should be greater than 100 meters.  
**2)** The path from the start point to the end point should not be a straight line.  
**3)** The starting point can be selected freely using Google Maps.  
**4)** The software calculates the distance between the starting point and the end point and compares it with the distance obtained from Google Maps.  
**5)** The calculated distance should have an error margin of no more than 5% compared to the distance shown by Google Maps.
