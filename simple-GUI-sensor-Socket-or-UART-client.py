# Author: Phillip Jones
# Date: 10/30/2023
# Description: Client starter code that combines: 1) Simple GUI, 2) creation of a thread for
#              running the Client socket in parallel with the GUI, and 3) Simple recieven of mock sensor 
#              data for a server/cybot.for collecting data from the cybot.

# General Python tutorials (W3schools):  https://www.w3schools.com/python/

import time # Time library   
# Socket library:  https://realpython.com/python-sockets/  
# See: Background, Socket API Overview, and TCP Sockets  
import socket
import tkinter as tk # Tkinter GUI library
# Thread library: https://www.geeksforgeeks.org/how-to-use-thread-in-tkinter-python/
import threading
import os  # import function for finding absolute path to this python script
import math
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.colors import ListedColormap

from new_CyBot_mapper import CyBotMapper

##### START Define Functions  #########

# Main: Mostly used for setting up, and starting the GUI
def main():
        global window  # Made global so quit function (send_quit) can access
        window = tk.Tk() # Create a Tk GUI Window
        window.title("CyBot Control Interface")
        window.geometry('1300x1200')

        # Create main container frame
        main_frame = tk.Frame(window)
        main_frame.pack(side=tk.RIGHT, expand=False)

        # Create control panel frame on the right
        control_frame = tk.Frame(main_frame)
        control_frame.pack(side=tk.RIGHT, expand=False)

        # Create mapper instance
        global mapper
        mapper = CyBotMapper(window)  # Pass the window instead of the frame

        # Last command label  
        global Last_command_Label  # Made global so that Client function (socket_thread) can modify
        Last_command_Label = tk.Label(control_frame, text="Last Command Sent: ") # Creat a Label
        Last_command_Label.pack(pady=5) # Pack the label into the window for display
        
        # Last received command label
        global Last_received_Label  # Made global so that Client function (socket_thread) can modify
        global Last_received
        Last_received_Label = tk.Label(control_frame, text="Last Command Received: ") # Create a Label
        Last_received = tk.Label(control_frame, text="")
        Last_received_Label.pack(pady=5) # Pack the label into the window for display
        Last_received.pack(pady=5)

        # Create button frame
        button_frame = tk.Frame(control_frame)
        button_frame.pack(pady=10)

        # Quit command Button
        quit_command_Button = tk.Button(button_frame, text="Press to Quit", command=send_quit)
        quit_command_Button.pack(pady=5)  # Pack the button into the window for display

        # Cybot Scan command Button
        scan_command_Button = tk.Button(button_frame, text="Press to Scan", command=send_scan)
        scan_command_Button.pack(pady=5) # Pack the button into the window for display

        # Cybot Stop Scan command Button
        scan_command_Button = tk.Button(button_frame, text="Stop Scan", command=stop_scan)
        scan_command_Button.pack(pady=5) # Pack the button into the window for display

        window.bind("<KeyPress>", key_press)
        window.bind("<KeyRelease>", key_release)

        # Create a Thread that will run a fuction assocated with a user defined "target" function.
        # In this case, the target function is the Client socket code
        my_thread = threading.Thread(target=socket_thread) # Creat the thread
        my_thread.start() # Start the thread

        # Start event loop so the GUI can detect events such as button clicks, key presses, etc.
        window.mainloop()

# Quit Button action.  Tells the client to send a Quit request to the Cybot, and exit the GUI
def send_quit():
        global gui_send_message # Command that the GUI has requested be sent to the Cybot
        global window  # Main GUI window
        
        gui_send_message = "h\n"   # Update the message for the Client to send
        time.sleep(1)
        window.destroy() # Exit GUI

# Scan Button action.  Tells the client to send a scan request to the Cybot
def send_scan():
        global gui_send_message # Command that the GUI has requested sent to the Cybot
        
        gui_send_message = "M\n"   # Update the message for the Client to send

def stop_scan():
        global gui_send_message # Command that the GUI has requested sent to the Cybot

        gui_send_message = "q\n"   # Update the message for the Client to send


def key_press(event):
        global gui_send_message

        if (event.char == 'w'):
                gui_send_message = "w\n"

        elif (event.char == 'a'):
               gui_send_message = "a\n"
        
        elif (event.char == 's'):
                gui_send_message = "s\n"

        elif (event.char == 'd'):
                gui_send_message = "d\n"

def key_release(event):
        global gui_send_message

        gui_send_message = " \n"    

# Client socket code (Run by a thread created in main)
def socket_thread():
        # Define Globals
        global Last_command_Label # GUI label for displaying the last command sent to the Cybot
        global gui_send_message   # Command that the GUI has requested be sent to the Cybot
        global mapper  # CyBot mapper instance

        # TCP Socket BEGIN (See Echo Client example): https://realpython.com/python-sockets/#echo-client-and-server
        HOST = "192.168.1.1"  # The server's hostname or IP address
        PORT = 288        # The port used by the server
        cybot_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # Create a socket object
        cybot_socket.connect((HOST, PORT))   # Connect to the socket  (Note: Server must first be running)
                      
        cybot = cybot_socket.makefile("rbw", buffering=0)  # makefile creates a file object out of a socket:  https://pythontic.com/modules/socket/makefile

        # Send initial message
        send_message = "Hello\n"
        gui_send_message = "wait\n"  # Initialize GUI command message to wait                                

        cybot.write(send_message.encode())
        print("Sent to server: " + send_message) 

        # Create a separate thread for continuous data reception
        def receive_data_thread():
            while send_message != 'h\n':
                if cybot_socket.fileno() != -1:  # Check if socket is still open
                    try:
                        # Set a timeout for the socket
                        # cybot_socket.settimeout(0.1)  # 100ms timeout
                        rx_message = cybot.readline()
                        if rx_message:
                            data = rx_message.decode().strip()
                            print("Received: " + data)
                            
                            # Update the Last received command label
                            Last_received.config(text=data)
                            
                            # Update map regardless of command state
                            if data.startswith('DISTANCE'):
                                distance = float(data.split()[1])
                                mapper.update_bot_position(distance)
                            elif data.startswith('ANGLE'):
                                angle = float(data.split()[1])
                                mapper.update_bot_angle(angle)
                            elif data.startswith('BUMP'):
                                direction = data.split()[1]
                                mapper.add_bump(direction)
                            elif data.startswith('HOLE'):
                                _, hole_type, direction = data.split()
                                mapper.add_hole(hole_type, direction)
                            # Handle scan object data
                            elif data.startswith('OBJECT'):
                                try:
                                    parts = data.split()
                                    object_distance = float(parts[1])
                                    object_angle = float(parts[2])
                                    object_diameter = float(parts[3])
                                    mapper.add_scanned_obstacle(object_distance, object_angle, object_diameter)
                                except (ValueError, IndexError) as e:
                                    print(f"Error parsing scan data: {e}")
                    except socket.timeout:
                        # Just continue on timeout, no need to print anything
                        continue
                    except Exception as e:
                        print(f"Error in receive thread: {e}")
                        # Don't break the thread on errors, just continue
                        continue

        # Start the receive thread
        receive_thread = threading.Thread(target=receive_data_thread)
        receive_thread.daemon = True
        receive_thread.start()

        # Main command loop
        while send_message != 'h\n':
                # Update the GUI to display command being sent to the CyBot
                command_display = "Last Command Sent:\t" + send_message
                Last_command_Label.config(text = command_display)  
        
                # Check if a sensor scan command has been sent
                if (send_message == "M\n") or (send_message == "m\n"):
                        print("Requested Sensor scan from Cybot:\n")

                # Wait for new command from GUI
                while gui_send_message == "wait\n": 
                        time.sleep(0.01)  # Reduced sleep time to 10ms for better responsiveness
                send_message = gui_send_message
                gui_send_message = "wait\n"

                # Send the command
                cybot.write(send_message.encode())
      
        print("Client exiting, and closing file descriptor, and/or network socket\n")
        time.sleep(1)
        cybot.close()
        cybot_socket.close()

##### END Define Functions  #########

### Run main ###
main()
