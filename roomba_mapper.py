import math
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.colors import ListedColormap
import tkinter as tk

class RoombaMapper:
    def __init__(self, master):
        # Store the master window for color changes
        self.master = master
        
        # Initialize position tracking
        self.x = 0
        self.y = 0
        self.angle = 0  # in degrees
        
        # Roomba dimensions (in mm)
        self.roomba_diameter = 304.8  # 1 foot = 304.8 mm
        self.roomba_radius = self.roomba_diameter / 2  # Radius for circular representation
        
        # Initialize map parameters
        self.resolution = 10  # 10mm per cell
        self.initial_map_size = 1000  # Start with 1m x 1m map
        self.map_size = self.initial_map_size
        self.grid_size = self.map_size // self.resolution
        self.obstacle_map = np.zeros((self.grid_size, self.grid_size))
        
        # Track map boundaries
        self.min_x = 0
        self.max_x = 0
        self.min_y = 0
        self.max_y = 0
        
        # Create matplotlib figure
        self.fig, self.ax = plt.subplots(figsize=(8, 8))
        self.update_map_display()
        
        # Initialize plot elements
        self.roomba_point, = self.ax.plot([], [], 'bo', markersize=10)
        self.roomba_direction, = self.ax.plot([], [], 'b-', linewidth=2)
        
        # Create custom colormap for different obstacle types
        colors = ['white', 'black', 'red', 'yellow']  # free space, obstacle, boundary, hole
        self.obstacle_plot = self.ax.imshow(self.obstacle_map.T, 
                                          extent=[-self.map_size/2, self.map_size/2, 
                                                 -self.map_size/2, self.map_size/2],
                                          cmap=ListedColormap(colors), alpha=0.5)
        
        # Create canvas for matplotlib in tkinter
        self.canvas = FigureCanvasTkAgg(self.fig, master=master)
        self.canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=1)
        
        # Start animation
        self.anim = FuncAnimation(self.fig, self.update_plot, interval=100, blit=True)
        
        # Initialize color change timer
        self.color_timer = None
        self.default_bg_color = master.cget("bg")  # Store the default background color

    def change_window_color(self, color, duration=1000):
        """Change the window background color for a specified duration (in ms)"""
        # Cancel any existing color change timer
        if self.color_timer:
            self.master.after_cancel(self.color_timer)
        
        # Change the background color
        self.master.configure(bg=color)
        
        # Set a timer to revert to the default color
        self.color_timer = self.master.after(duration, lambda: self.master.configure(bg=self.default_bg_color))

    def update_map_display(self):
        """Update the map display based on current boundaries"""
        # Add padding around the current boundaries
        padding = 500  # 500mm padding
        self.min_x = min(self.min_x, self.x) - padding
        self.max_x = max(self.max_x, self.x) + padding
        self.min_y = min(self.min_y, self.y) - padding
        self.max_y = max(self.max_y, self.y) + padding
        
        # Calculate new map size
        width = self.max_x - self.min_x
        height = self.max_y - self.min_y
        self.map_size = max(width, height)
        
        # Ensure map size is a multiple of resolution
        self.map_size = ((self.map_size + self.resolution - 1) // self.resolution) * self.resolution
        self.grid_size = self.map_size // self.resolution
        
        # Create new obstacle map if size changed
        if self.grid_size != self.obstacle_map.shape[0]:
            new_map = np.zeros((self.grid_size, self.grid_size))
            # Copy existing data to new map
            old_center = self.initial_map_size // 2
            new_center = self.map_size // 2
            offset_x = new_center - old_center
            offset_y = new_center - old_center
            
            # Calculate overlap region
            old_size = self.obstacle_map.shape[0]
            overlap_size = min(old_size, self.grid_size)
            
            # Copy data from old map to new map
            for i in range(overlap_size):
                for j in range(overlap_size):
                    old_x = i - old_center // self.resolution
                    old_y = j - old_center // self.resolution
                    new_x = i - new_center // self.resolution
                    new_y = j - new_center // self.resolution
                    
                    if (0 <= old_x < old_size and 0 <= old_y < old_size and
                        0 <= new_x < self.grid_size and 0 <= new_y < self.grid_size):
                        new_map[new_x, new_y] = self.obstacle_map[old_x, old_y]
            
            self.obstacle_map = new_map
        
        # Update plot limits
        self.ax.set_xlim(self.min_x, self.max_x)
        self.ax.set_ylim(self.min_y, self.max_y)
        self.ax.set_title('Roomba Position and Obstacle Map')
        self.ax.grid(True)

    def update_position(self, distance):
        # Convert distance from mm to map units
        distance = distance * self.resolution / 1000  # Convert mm to map units
        
        # Update position based on current angle and distance
        rad_angle = math.radians(self.angle)
        self.x += distance * math.cos(rad_angle)
        self.y += distance * math.sin(rad_angle)
        
        # Check if we need to resize the map
        if (self.x < self.min_x + 200 or self.x > self.max_x - 200 or
            self.y < self.min_y + 200 or self.y > self.max_y - 200):
            self.update_map_display()
        
        # Add obstacle to map if distance is less than max range
        if distance < 5000:  # 5m max range
            obstacle_x = int(self.x + distance * math.cos(rad_angle))
            obstacle_y = int(self.y + distance * math.sin(rad_angle))
            
            # Convert to grid coordinates
            grid_x = int((obstacle_x - self.min_x) / self.resolution)
            grid_y = int((obstacle_y - self.min_y) / self.resolution)
            
            # Update obstacle map
            if 0 <= grid_x < self.grid_size and 0 <= grid_y < self.grid_size:
                self.obstacle_map[grid_x, grid_y] = 1

    def handle_bump(self, direction):
        # Add bump location to obstacle map
        bump_distance = 100  # 100mm from center of Roomba
        rad_angle = math.radians(self.angle)
        
        if direction == 'FAR_LEFT':
            bump_angle = self.angle + 90
        elif direction == 'FAR_RIGHT':
            bump_angle = self.angle - 90
        elif direction == 'LEFT':
            bump_angle = self.angle + 45
        elif direction == 'RIGHT':
            bump_angle = self.angle - 45
            
        bump_rad = math.radians(bump_angle)
        bump_x = int(self.x + bump_distance * math.cos(bump_rad))
        bump_y = int(self.y + bump_distance * math.sin(bump_rad))
        
        # Convert to grid coordinates
        grid_x = int((bump_x - self.min_x) / self.resolution)
        grid_y = int((bump_y - self.min_y) / self.resolution)
        
        # Update obstacle map
        if 0 <= grid_x < self.grid_size and 0 <= grid_y < self.grid_size:
            self.obstacle_map[grid_x, grid_y] = 1
            
        # Change window color to red for bump
        self.change_window_color("red", 1000)  # Red for 1 second

    def handle_hole(self, hole_type, direction):
        # Add hole location to obstacle map
        hole_distance = 100  # 100mm from center of Roomba
        rad_angle = math.radians(self.angle)
        
        # Determine hole angle based on direction
        if direction == 'FAR_LEFT':
            hole_angle = self.angle + 90
        elif direction == 'FAR_RIGHT':
            hole_angle = self.angle - 90
        elif direction == 'LEFT':
            hole_angle = self.angle + 45
        elif direction == 'RIGHT':
            hole_angle = self.angle - 45
            
        hole_rad = math.radians(hole_angle)
        hole_x = int(self.x + hole_distance * math.cos(hole_rad))
        hole_y = int(self.y + hole_distance * math.sin(hole_rad))
        
        # Convert to grid coordinates
        grid_x = int((hole_x - self.min_x) / self.resolution)
        grid_y = int((hole_y - self.min_y) / self.resolution)
        
        # Update obstacle map based on hole type
        if 0 <= grid_x < self.grid_size and 0 <= grid_y < self.grid_size:
            if hole_type == 'BOUNDARY':
                # Mark field boundary with a different value (2)
                self.obstacle_map[grid_x, grid_y] = 2
            else:  # hole_type == 'HOLE'
                # Mark actual hole with value 3
                self.obstacle_map[grid_x, grid_y] = 3
                
        # Change window color to blue for hole
        self.change_window_color("blue", 1000)  # Blue for 1 second

    def update_plot(self, frame):
        # Update Roomba position
        self.roomba_point.set_data([self.x], [self.y])
        
        # Update Roomba direction indicator and size
        direction_length = self.roomba_diameter  # Use Roomba diameter for direction indicator
        rad_angle = math.radians(self.angle)
        end_x = self.x + direction_length * math.cos(rad_angle)
        end_y = self.y + direction_length * math.sin(rad_angle)
        self.roomba_direction.set_data([self.x, end_x], [self.y, end_y])
        
        # Draw Roomba circle
        if hasattr(self, 'roomba_circle'):
            self.roomba_circle.remove()
        
        # Create and add circle
        self.roomba_circle = plt.Circle((self.x, self.y), self.roomba_radius, 
                                       facecolor='blue', alpha=0.5)
        self.ax.add_patch(self.roomba_circle)
        
        # Update obstacle map
        self.obstacle_plot.set_array(self.obstacle_map.T)
        self.obstacle_plot.set_extent([self.min_x, self.max_x, self.min_y, self.max_y])
        
        return self.roomba_point, self.roomba_direction, self.obstacle_plot, self.roomba_circle