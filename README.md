<h1>SerialRealTime</h1>

<h2>SerialRealTime : Class to create real-time writer serial on Qt using QThread technology</h2>

*Copyright (c) CNES and contributors. All rights reserved.*

<p>To be able to control a measuring bench using the GRBL system, I wrote a serial class that allows to send commands to manage the movement. 
        Attention, I do not manage the reading, being messages of non-fixed sizes it would be necessary to remodel the whole code to be able to read.</p>
        
        - class MainWindow : Manages the display and user signals.
        - Class SerialPort : Class managing the serial port and sending messages using another thread than the main one
