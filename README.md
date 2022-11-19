#Overview
soqt (pronounced as 'so cute') is a Qt based GUI application that aims to graphically set up, represent and control different sockets and connections available on a device (currently some of the serial connections use direct linux api calls and as such this is not truely cross platform. In future this may be improved). Allowing a user to simply and quickly create rules to control data flow in their computer using conventional ports.

##Use Cases
  * Port conversion - Changing one port type to another, i.e., an old application was developed to only support serial. But your particular scenario requires that this application uses UDP. You can use this application to create a virtual serial port for said application to connect to, a UDP port that sends and receives to where you need the data to go to and then create a connection between the two so that any serial data will be sent over UDP and any UDP data will become Serial
  * Data passthrough, similar to above, but turning a port into another, sending physical serial data over TCP or UDP or whatever.
  * Port Logging - You can set this application as an 'in the middle' application between your applications output and wherever that output was meant to go. Data (via soqt) can be sent to the required direction, and the data can be split and sent to a file, so theres a log of all data sent and received
  * Data muxing/demuxing - One port to many or many to one
  * Fake interfaces for testing - Custom rules can be created allowing you to set up a 'fake' interface to test your applications.

##Details
On launching the application, you will be greeted with a designer in the centre, available ports on the left and direct port monitoring on the right (each of these views can be hidden or shown, see the 'view' drop down). By selecting a connection type on the left, a representation will be dropped into the designer view. Useing the tools at the top, the user can move around the port represenation, create connections or just update the fields inside the port representation.

The user can create connections between ports by using the connection tool at the top of the designer and clicking and drawing from one port to another. If the recieving port is willing to accept the connection, the arrow will change colour, releasing the left click will create a connection FROM the initial port TO the receiving port. During runtime, any data received by the initial port will be forwarded and sent out by the receiving port. Connections must be unique i.e., there can only be one connection from one port to another (reverse will also work as that is a unique direction).

The design can be saved to file to save you from having to re create the connections everytime

##Supported ports
  * UDP (Sending and Receiving)
  * TCP (Server and Client)
  * Serial
  * File IO (Reading and writing)
  * Virtual Serial

###Extra ports
  * Tasks - 'Port' that is just a collection of rules, such as, waiting for a particular packet of data and then responding with a user configured packet. Allows creating 'fake' interfaces quickly
  * Gate - Just a repeater, may be useful when drawing up your connections

#License
GPLv3 but this is just a fun side project that I figured would have benefits to others. Use and modify as you want, just remember that this uses Qt open source licensing. See [Qt's website](https://www.qt.io/download-open-source) for more information