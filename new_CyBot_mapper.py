import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Circle, Arrow
from matplotlib.animation import FuncAnimation

class CyBotMapper:
    def __init__(self, window):
        # Store window reference
        self.master = window
        
        # Initialize bot state
        self.bot_x = 0  # Starting x position (mm)
        self.bot_y = 0  # Starting y position (mm) 
        self.bot_angle = 90  # Starting angle (degrees) - facing up
        self.bot_radius = 168.275  # Bot radius in mm (13.25 inches = 336.55mm diameter, so radius = 168.275mm)

        # Create figure and axis
        self.fig, self.ax = plt.subplots(figsize=(8, 8))
        self.ax.set_xlim(-2500, 2500)  # Field dimensions in mm (about 100 inches = 2540mm)
        self.ax.set_ylim(-2500, 2500)
        self.ax.grid(True)
        
        # Initialize empty lists for obstacles
        self.obstacles_x = []
        self.obstacles_y = []
        self.holes = []  # Will store tuples of (x, y, type, direction)
        self.bumps = []  # Will store tuples of (x, y, direction)

        # Create bot representation
        self.bot_circle = Circle((self.bot_x, self.bot_y), self.bot_radius, 
                               fill=True, color='blue', alpha=0.5)
        self.direction_arrow = Arrow(self.bot_x, self.bot_y, 
                                   self.bot_radius*np.cos(np.radians(self.bot_angle)),
                                   self.bot_radius*np.sin(np.radians(self.bot_angle)),
                                   width=self.bot_radius/2, color='red')
        
        self.ax.add_patch(self.bot_circle)
        self.ax.add_patch(self.direction_arrow)

        # Embed plot in tkinter window
        from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
        self.canvas = FigureCanvasTkAgg(self.fig, master=window)
        self.canvas.draw()
        self.canvas.get_tk_widget().pack(side='left', fill='both', expand=True)

    def update_bot_position(self, distance_mm):
        """Update bot position based on distance moved in current direction (in mm)"""
        # Calculate new position using current angle and distance
        dx = distance_mm * np.cos(np.radians(self.bot_angle))
        dy = distance_mm * np.sin(np.radians(self.bot_angle))
        
        self.bot_x += dx
        self.bot_y += dy
        
        # Update bot circle position
        self.bot_circle.center = (self.bot_x, self.bot_y)
        
        # Update direction arrow
        self._update_direction_arrow()
        
        self.canvas.draw()

    def update_bot_angle(self, angle_change):
        """Update bot angle based on change in angle"""
        self.bot_angle += angle_change * 4
        self.bot_angle = (self.bot_angle  % 360)
        # Update direction arrow
        self._update_direction_arrow()
        
        self.canvas.draw()

    def _update_direction_arrow(self):
        """Helper function to update direction arrow"""
        # Remove old arrow
        self.direction_arrow.remove()
        # Create new arrow
        self.direction_arrow = Arrow(self.bot_x, self.bot_y,
                                   self.bot_radius*np.cos(np.radians(self.bot_angle)),
                                   self.bot_radius*np.sin(np.radians(self.bot_angle)),
                                   width=self.bot_radius/2, color='red')
        self.ax.add_patch(self.direction_arrow)

    def add_scanned_obstacle(self, distance, scan_angle, diameter):
        """Add obstacle based on distance and angle from bot (all measurements in mm)"""
        # Convert scan angle (0-180) to global angle
        global_angle = (self.bot_angle - 90 + scan_angle) % 360
        
        # Calculate scanner position (101.6mm above bot center - 4 inches = 101.6mm)
        scanner_x = self.bot_x + 101.6 * np.cos(np.radians(self.bot_angle))
        scanner_y = self.bot_y + 101.6 * np.sin(np.radians(self.bot_angle))
        
        # Calculate obstacle position relative to scanner
        x = scanner_x + (distance*10) * np.cos(np.radians(global_angle))
        y = scanner_y + (distance*10) * np.sin(np.radians(global_angle))
        
        self.obstacles_x.append(x)
        self.obstacles_y.append(y)
        
        # Draw obstacle with specified diameter in green
        obstacle_circle = Circle((x, y), diameter*5, color='green', fill=True, alpha=0.5)
        self.ax.add_patch(obstacle_circle)
        self.canvas.draw()

    def add_bump(self, direction):
        """Add bump (low object) based on direction (left/right)"""
        # Determine bump position relative to bot
        if direction == "LEFT":
            angle_offset = 45 
        elif direction == "RIGHT":
            angle_offset = -45
        global_angle = (self.bot_angle + angle_offset) % 360
        
        # Place bump at bot radius distance
        x = self.bot_x + self.bot_radius * np.cos(np.radians(global_angle))
        y = self.bot_y + self.bot_radius * np.sin(np.radians(global_angle))
        
        self.bumps.append((x, y, direction))
        
        # Draw bump as red circle with 15cm (150mm) diameter
        bump_circle = Circle((x, y), 75, color='red', fill=True, alpha=0.5)  # 150mm diameter = 75mm radius
        self.ax.add_patch(bump_circle)
        
        # Flash window red
        orig_color = self.master.cget('bg')
        self.master.configure(bg='red')
        self.master.after(500, lambda: self.master.configure(bg=orig_color))
        
        self.canvas.draw()

    def add_hole(self, hole_type, direction):
        """Add hole based on type (boundary/hole) and direction"""
        # Convert direction to angle offset
        direction_offsets = {
            "FAR_LEFT": 80,
            "LEFT": 10,
            "RIGHT": -10,
            "FAR_RIGHT": -80
        }
        
        angle_offset = direction_offsets.get(direction, 0)
        global_angle = (self.bot_angle + angle_offset) % 360
        
        # Place hole at bot radius distance
        x = self.bot_x + self.bot_radius * np.cos(np.radians(global_angle))
        y = self.bot_y + self.bot_radius * np.sin(np.radians(global_angle))
        
        self.holes.append((x, y, hole_type, direction))
        
        # Different markers for different hole types
        if hole_type == "BOUNDARY":
            self.ax.plot(x, y, 'bx', markersize=10)  # blue X
        else:  # regular hole
            self.ax.plot(x, y, 'bo', markersize=10)  # blue circle
            
        # Flash window blue
        orig_color = self.master.cget('bg')
        self.master.configure(bg='blue')
        self.master.after(500, lambda: self.master.configure(bg=orig_color))
            
        self.canvas.draw()

