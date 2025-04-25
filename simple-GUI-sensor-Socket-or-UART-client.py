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

import CyBot_Plot_Sensor_Scan_Values_From_File as valFromFile
from CyBot_mapper import CyBotMapper

##### START Define Functions  #########

# Main: Mostly used for setting up, and starting the GUI
def main():
        global window  # Made global so quit function (send_quit) can access
        window = tk.Tk() # Create a Tk GUI Window

        # Create mapper instance
        global mapper
        mapper = CyBotMapper(window)

        # Last command label  
        global Last_command_Label  # Made global so that Client function (socket_thread) can modify
        Last_command_Label = tk.Label(text="Last Command Sent: ") # Creat a Label
        Last_command_Label.pack() # Pack the label into the window for display

        # Quit command Button
        quit_command_Button = tk.Button(text ="Press to Quit", command = send_quit)
        quit_command_Button.pack()  # Pack the button into the window for display

        # Cybot Scan command Button
        scan_command_Button = tk.Button(text ="Press to Scan", command = send_scan)
        scan_command_Button.pack() # Pack the button into the window for display

        # scan_command_Button = tk.Button(text ="Press to Turn", command = send_turn)
        # scan_command_Button.pack() # Pack the button into the window for display

        # scan_command_Button = tk.Button(text ="Press to Move", command = send_move)
        # scan_command_Button.pack() # Pack the button into the window for display

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
        
        gui_send_message = "quit\n"   # Update the message for the Client to send
        time.sleep(1)
        window.destroy() # Exit GUI

# Scan Button action.  Tells the client to send a scan request to the Cybot
def send_scan():
        global gui_send_message # Command that the GUI has requested sent to the Cybot
        
        gui_send_message = "M\n"   # Update the message for the Client to send

# def send_turn():
#         global gui_send_message # Command that the GUI has requested sent to the Cybot
#         global turnAmount
#         global turnDirection
#         gui_send_message = "T\n"   # Update the message for the Client to send

#         turn_window = tk.Toplevel()
#         turn_window.title("Turn Control")

#         # Function to handle the turn action
#         def handle_turn(direction):
#                 degrees = degrees_entry.get()
#                 if degrees.isdigit() and int(degrees) > 0 and int(degrees) < 91:
#                         turnAmount = int(degrees)
#                         turnDirection = direction
#                         # Here you can add the logic to handle the turn
                        
#                         turn_window.destroy()  # Close the top window after action
#                 else:
#                         messagebox.showerror("Input Error", "Please enter a valid number of degrees. (Between 0 and 91)")

#         # Create a label for instructions
#         label = tk.Label(turn_window, text="Enter degrees to turn:")
#         label.pack(pady=10)

#         # Create an entry for degrees input
#         degrees_entry = tk.Entry(turn_window)
#         degrees_entry.pack(pady=10)

#         # Create buttons for left and right turns
#         left_button = tk.Button(turn_window, text="Turn Left", command=lambda: handle_turn("Left"))
#         left_button.pack(side=tk.LEFT, padx=20, pady=20)

#         right_button = tk.Button(turn_window, text="Turn Right", command=lambda: handle_turn("Right"))
#         right_button.pack(side=tk.RIGHT, padx=20, pady=20)

# def send_move():
#         global gui_send_message # Command that the GUI has requested sent to the Cybot
#         global moveAmount #in mm
#         global moveDirection
#         gui_send_message = "8\n"   # Update the message for the Client to send

#         move_window = tk.Toplevel()
#         move_window.title("Move Control")

#         # Function to handle the move action
#         def handle_move(direction):
#                 centimenters = centimenters_entry.get()
#                 if centimenters.isdigit() and int(centimenters) > 0:
#                         moveAmount = int(centimenters) * 10
#                         moveDirection = direction
#                         # Here you can add the logic to handle the move
                        
#                         move_window.destroy()  # Close the top window after action
#                 else:
#                         messagebox.showerror("Input Error", "Please enter a valid number of degrees. (Between 0 and 91)")

#         # Create a label for instructions
#         label = tk.Label(move_window, text="Enter centimeters to move:")
#         label.pack(pady=10)

#         # Create an entry for degrees input
#         centimenters_entry = tk.Entry(move_window)
#         centimenters_entry.pack(pady=10)

#         # Create buttons for left and right turns
#         left_button = tk.Button(move_window, text="Move Forward", command=lambda: handle_move("Forward"))
#         left_button.pack(side=tk.LEFT, padx=20, pady=20)

#         right_button = tk.Button(move_window, text="Move Backward", command=lambda: handle_move("Backward"))
#         right_button.pack(side=tk.RIGHT, padx=20, pady=20)

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

        # A little python magic to make it more convient for you to adjust where you want the data file to live
        # Link for more info: https://towardsthecloud.com/get-relative-path-python 
        absolute_path = os.path.dirname(__file__) # Absoult path to this python script
        relative_path = "./"   # Path to sensor data file relative to this python script (./ means data file is in the same directory as this python script)
        full_path = os.path.join(absolute_path, relative_path) # Full path to sensor data file
        filename = 'GUITESTING_Sensor_Data.txt' # Name of file you want to store sensor data from your sensor scan command

        # TCP Socket BEGIN (See Echo Client example): https://realpython.com/python-sockets/#echo-client-and-server
        HOST = "192.168.1.1"  # The server's hostname or IP address
        PORT = 288        # The port used by the server
        cybot_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # Create a socket object
        cybot_socket.connect((HOST, PORT))   # Connect to the socket  (Note: Server must first be running)
                      
        cybot = cybot_socket.makefile("rbw", buffering=0)  # makefile creates a file object out of a socket:  https://pythontic.com/modules/socket/makefile
        # TCP Socket END

        # Send initial message
        send_message = "Hello\n"
        gui_send_message = "wait\n"  # Initialize GUI command message to wait                                

        cybot.write(send_message.encode())
        print("Sent to server: " + send_message) 

        # Create a separate thread for continuous data reception
        def receive_data_thread():
            while send_message != 'quit\n':
                if cybot_socket.fileno() != -1:  # Check if socket is still open
                    try:
                        # Set a timeout for the socket
                        cybot_socket.settimeout(0.1)  # 100ms timeout
                        rx_message = cybot.readline()
                        if rx_message:
                            data = rx_message.decode().strip()
                            print("Received: " + data)
                            
                            # Update map regardless of command state
                            if data.startswith('Distance'):
                                distance = int(data.split()[1])
                                mapper.update_position(distance)
                            elif data.startswith('Angle'):
                                angle = int(data.split()[1])
                                mapper.angle = angle
                            elif data.startswith('BUMP'):
                                direction = data.split()[1]
                                mapper.handle_bump(direction)
                            elif data.startswith('HOLE'):
                                _, hole_type, direction = data.split()
                                mapper.handle_hole(hole_type, direction)
                    except socket.timeout:
                        pass  # No data received within timeout
                    except Exception as e:
                        print(f"Error in receive thread: {e}")
                        break

        # Start the receive thread
        receive_thread = threading.Thread(target=receive_data_thread)
        receive_thread.daemon = True
        receive_thread.start()

        # Main command loop
        while send_message != 'quit\n':
                # Update the GUI to display command being sent to the CyBot
                command_display = "Last Command Sent:\t" + send_message
                Last_command_Label.config(text = command_display)  
        
                # Check if a sensor scan command has been sent
                if (send_message == "M\n") or (send_message == "m\n"):
                        print("Requested Sensor scan from Cybot:\n")
                        rx_message = bytearray(1) # Initialize a byte array

                        # Create or overwrite existing sensor scan data file
                        file_object = open(full_path + filename,'w')

                        while (rx_message.decode() != "END\n"):
                                rx_message = cybot.readline()
                                file_object.write(rx_message.decode())
                                print(rx_message.decode())

                        file_object.close()
                        valFromFile.plot()

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
