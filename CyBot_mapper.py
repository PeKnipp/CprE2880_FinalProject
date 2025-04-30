import math
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.colors import ListedColormap
import tkinter as tk

class CyBotMapper:
    def __init__(self, master):
        # Store the master window for color changes
        self.master = master
        
        # Create a frame for the map
        self.map_frame = tk.Frame(master)
        self.map_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        
        # Initialize position tracking
        self.x = 0  # Start at origin (0,0)
        self.y = 0
        self.angle = 0  # in degrees
        
        # CyBot dimensions (in mm)
        # 13.25 inches = 336.55 mm diameter
        self.CyBot_diameter = 336.55  # 13.25 inches = 336.55 mm
        self.CyBot_radius = self.CyBot_diameter / 2  # Radius for circular representation
        
        # Scanner dimensions
        self.scanner_offset = 139.7  # 5.5 inches = 139.7 mm from center
        
        # Initialize map parameters - very large grid
        self.resolution = 10  # 10mm per cell
        self.map_size = 10000  # 100m x 100m map (very large)
        self.grid_size = self.map_size // self.resolution
        self.obstacle_map = np.zeros((self.grid_size, self.grid_size))
        
        # Map boundaries centered on origin
        self.min_x = -self.map_size // 2
        self.max_x = self.map_size // 2
        self.min_y = -self.map_size // 2
        self.max_y = self.map_size // 2
        
        # Create matplotlib figure
        self.fig, self.ax = plt.subplots(figsize=(10, 10))
        
        # Initialize plot elements
        self.CyBot_point, = self.ax.plot([], [], 'bo', markersize=10)
        self.CyBot_direction, = self.ax.plot([], [], 'b-', linewidth=2)
        
        # Create custom colormap for different obstacle types
        colors = ['white', 'black', 'red', 'blue', 'green']  # free space, boundary, bump, hole, scanned object
        self.obstacle_plot = self.ax.imshow(self.obstacle_map.T, 
                                          extent=[self.min_x, self.max_x, 
                                                 self.min_y, self.max_y],
                                          cmap=ListedColormap(colors), alpha=0.5)
        
        # Create canvas for matplotlib in tkinter
        self.canvas = FigureCanvasTkAgg(self.fig, master=self.map_frame)
        self.canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=1)
        
        # Initialize color change timer
        self.color_timer = None
        self.default_bg_color = master.cget("bg")  # Store the default background color
        
        # Initialize scan visualization elements
        self.scanner_point, = self.ax.plot([], [], 'ro', markersize=5)
        self.scanner_arc, = self.ax.plot([], [], 'r--', linewidth=1)
        
        # Lists to store objects for visualization
        self.scan_objects = []  # List to store scanned objects
        self.bump_objects = []  # List to store bump objects
        self.hole_objects = []  # List to store hole objects
        
        # Initialize animation
        self.animation_running = True
        self.animation_timer = None
        
        # Bind cleanup to window close event
        master.protocol("WM_DELETE_WINDOW", self.cleanup)
        
        # Set initial plot limits
        self.ax.set_xlim(self.min_x, self.max_x)
        self.ax.set_ylim(self.min_y, self.max_y)
        self.ax.set_title('CyBot Position and Obstacle Map')
        self.ax.grid(True)
        
        # Start animation after all initialization is complete
        self.start_animation()

    def change_window_color(self, color, duration=1000):
        """Change the window background color for a specified duration (in ms)"""
        # Cancel any existing color change timer
        if self.color_timer:
            self.master.after_cancel(self.color_timer)
        
        # Change the background color
        self.master.configure(bg=color)
        
        # Set a timer to revert to the default color
        self.color_timer = self.master.after(duration, lambda: self.master.configure(bg=self.default_bg_color))

    def update_position(self, distance_cm):
        """
        Update CyBot position based on movement
        distance_cm: Distance moved in centimeters
        """
        # Convert distance from cm to mm
        distance_mm = distance_cm * 10
        
        # Convert angle to radians
        rad_angle = math.radians(self.angle)
        
        # Update position based on current angle and distance
        self.x += distance_mm * math.cos(rad_angle)
        self.y += distance_mm * math.sin(rad_angle)

    def update_angle(self, angle_degrees):
        """
        Update CyBot angle by turning the specified amount
        angle_degrees: Amount to turn in degrees (positive for clockwise, negative for counterclockwise)
        """
        # Update angle by adding the turn amount
        self.angle += angle_degrees
        
        # Normalize angle to be between 0 and 360 degrees
        self.angle = self.angle % 360

    def handle_bump(self, direction):
        """
        Handle bump detection
        direction: 'LEFT' or 'RIGHT' indicating which side the bump was detected
        """
        # Calculate bump position (45 degrees from front on specified side)
        bump_angle = self.angle
        if direction == 'LEFT':
            bump_angle += 45
        elif direction == 'RIGHT':
            bump_angle -= 45
            
        # Convert to radians
        bump_rad = math.radians(bump_angle)
        
        # Calculate bump position (at front of CyBot)
        bump_x = self.x + self.CyBot_radius * math.cos(bump_rad)
        bump_y = self.y + self.CyBot_radius * math.sin(bump_rad)
        
        # Convert to grid coordinates
        grid_x = int((bump_x - self.min_x) / self.resolution)
        grid_y = int((bump_y - self.min_y) / self.resolution)
        
        # Calculate bump radius (15 cm = 150 mm)
        bump_radius_mm = 150
        bump_radius_cells = int(bump_radius_mm / self.resolution)
        
        # Create bump area (red circle)
        for dx in range(-bump_radius_cells, bump_radius_cells + 1):
            for dy in range(-bump_radius_cells, bump_radius_cells + 1):
                # Check if point is within the circle
                if dx*dx + dy*dy <= bump_radius_cells*bump_radius_cells:
                    nx, ny = grid_x + dx, grid_y + dy
                    if 0 <= nx < self.grid_size and 0 <= ny < self.grid_size:
                        # Mark as bump (value 2)
                        self.obstacle_map[nx, ny] = 2
        
        # Store bump for visualization
        self.bump_objects.append((bump_x, bump_y, bump_radius_mm))
        
        # Limit the number of stored bumps to prevent memory issues
        if len(self.bump_objects) > 50:
            self.bump_objects.pop(0)
            
        # Change window color to red for bump
        self.change_window_color("red", 1000)  # Red for 1 second

    def handle_hole(self, hole_type, location):
        """
        Handle hole detection
        hole_type: 'BOUNDARY' or 'HOLE'
        location: 'far_right', 'right', 'left', 'far_left'
        """
        # Determine hole angle based on location
        hole_angle = self.angle
        if location == 'far_right':
            hole_angle += 0
        elif location == 'right':
            hole_angle += 45
        elif location == 'left':
            hole_angle += 135
        elif location == 'far_left':
            hole_angle += 180
            
        # Convert to radians
        hole_rad = math.radians(hole_angle)
        
        # Calculate hole position (at front of CyBot)
        hole_x = self.x + self.CyBot_radius * math.cos(hole_rad)
        hole_y = self.y + self.CyBot_radius * math.sin(hole_rad)
        
        # Convert to grid coordinates
        grid_x = int((hole_x - self.min_x) / self.resolution)
        grid_y = int((hole_y - self.min_y) / self.resolution)
        
        # Update obstacle map based on hole type
        if 0 <= grid_x < self.grid_size and 0 <= grid_y < self.grid_size:
            if hole_type == 'BOUNDARY':
                # Mark field boundary with value 1 (black)
                self.obstacle_map[grid_x, grid_y] = 1
                # Store boundary for visualization
                self.hole_objects.append((hole_x, hole_y, 'BOUNDARY'))
            else:  # hole_type == 'HOLE'
                # Mark actual hole with value 3 (blue)
                self.obstacle_map[grid_x, grid_y] = 3
                # Store hole for visualization
                self.hole_objects.append((hole_x, hole_y, 'HOLE'))
        
        # Limit the number of stored holes to prevent memory issues
        if len(self.hole_objects) > 100:
            self.hole_objects.pop(0)
            
        # Change window color to blue for hole
        self.change_window_color("blue", 1000)  # Blue for 1 second
        
    def handle_scan_object(self, distance, angle, diameter):
        """
        Handle object data from scan
        distance: Distance to object in mm
        angle: Angle to object in degrees (relative to scanner arc)
        diameter: Diameter of the object in mm
        """
        # Convert scanner angle to radians
        scanner_rad = math.radians(self.angle)
        
        # Calculate scanner pivot point
        scanner_x = self.x + self.scanner_offset * math.cos(scanner_rad)
        scanner_y = self.y + self.scanner_offset * math.sin(scanner_rad)
        
        # Calculate object position relative to scanner
        # First convert scanner-relative angle to absolute angle
        absolute_angle = self.angle + angle
        absolute_rad = math.radians(absolute_angle)
        
        # Then calculate object position
        object_x = scanner_x + distance * math.cos(absolute_rad)
        object_y = scanner_y + distance * math.sin(absolute_rad)
        
        # Convert to grid coordinates
        grid_x = int((object_x - self.min_x) / self.resolution)
        grid_y = int((object_y - self.min_y) / self.resolution)
        
        # Calculate object radius in grid cells
        object_radius_cells = int(diameter / (2 * self.resolution))
        
        # Update obstacle map (value 4 for scanned objects)
        # Draw a circle with the object's diameter
        if 0 <= grid_x < self.grid_size and 0 <= grid_y < self.grid_size:
            # Mark the center point
            self.obstacle_map[grid_x, grid_y] = 4
            
            # Draw a circle around the center point based on the object's diameter
            for dx in range(-object_radius_cells, object_radius_cells + 1):
                for dy in range(-object_radius_cells, object_radius_cells + 1):
                    # Check if the point is within the circle
                    if dx*dx + dy*dy <= object_radius_cells*object_radius_cells:
                        nx, ny = grid_x + dx, grid_y + dy
                        if 0 <= nx < self.grid_size and 0 <= ny < self.grid_size:
                            self.obstacle_map[nx, ny] = 4
            
        # Store object for visualization
        self.scan_objects.append((object_x, object_y, diameter))
        
        # Limit the number of stored objects to prevent memory issues
        if len(self.scan_objects) > 100:
            self.scan_objects.pop(0)

    def update_plot(self, frame):
        # Update CyBot position
        self.CyBot_point.set_data([self.x], [self.y])
        
        # Update CyBot direction indicator
        direction_length = self.CyBot_diameter  # Use CyBot diameter for direction indicator
        rad_angle = math.radians(self.angle)
        end_x = self.x + direction_length * math.cos(rad_angle)
        end_y = self.y + direction_length * math.sin(rad_angle)
        self.CyBot_direction.set_data([self.x, end_x], [self.y, end_y])
        
        # Draw CyBot circle
        if hasattr(self, 'CyBot_circle'):
            self.CyBot_circle.remove()
        
        # Create and add circle
        self.CyBot_circle = plt.Circle((self.x, self.y), self.CyBot_radius, 
                                       facecolor='blue', alpha=0.5)
        self.ax.add_patch(self.CyBot_circle)
        
        # Update scanner pivot point and arc
        # Update scanner pivot point
        scanner_x = self.x + self.scanner_offset * math.cos(rad_angle)
        scanner_y = self.y + self.scanner_offset * math.sin(rad_angle)
        self.scanner_point.set_data([scanner_x], [scanner_y])
        
        # Update scanner arc (sweep area)
        # Create an arc of points representing the sensor sweep
        arc_angle_start = self.angle - 90  # -90 degrees from center
        arc_angle_end = self.angle + 90    # +90 degrees from center
        arc_points = 20
        arc_x = []
        arc_y = []
        
        for i in range(arc_points + 1):
            t = i / arc_points
            arc_angle = arc_angle_start + t * (arc_angle_end - arc_angle_start)
            arc_rad = math.radians(arc_angle)
            
            # Calculate point at the end of the sensor
            sensor_length = 500  # 500mm sensor range
            arc_x.append(scanner_x + sensor_length * math.cos(arc_rad))
            arc_y.append(scanner_y + sensor_length * math.sin(arc_rad))
            
        self.scanner_arc.set_data(arc_x, arc_y)
        
        # Draw scanned objects (green circles)
        # Remove any existing object circles
        for circle in getattr(self, 'scan_circles', []):
            circle.remove()
        
        # Create a new list to store object circles
        self.scan_circles = []
        
        # Draw each scanned object
        for obj in self.scan_objects:
            obj_x, obj_y, obj_diameter = obj
            # Create a circle for the object
            obj_circle = plt.Circle((obj_x, obj_y), obj_diameter/2, 
                                   facecolor='green', alpha=0.3, edgecolor='green')
            self.ax.add_patch(obj_circle)
            self.scan_circles.append(obj_circle)
        
        # Draw bump objects (red circles)
        # Remove any existing bump circles
        for circle in getattr(self, 'bump_circles', []):
            circle.remove()
        
        # Create a new list to store bump circles
        self.bump_circles = []
        
        # Draw each bump object
        for obj in self.bump_objects:
            obj_x, obj_y, obj_radius = obj
            # Create a circle for the bump
            bump_circle = plt.Circle((obj_x, obj_y), obj_radius/2, 
                                    facecolor='red', alpha=0.3, edgecolor='red')
            self.ax.add_patch(bump_circle)
            self.bump_circles.append(bump_circle)
        
        # Draw hole objects (black or blue dots)
        # Remove any existing hole points
        for point in getattr(self, 'hole_points', []):
            point.remove()
        
        # Create a new list to store hole points
        self.hole_points = []
        
        # Draw each hole object
        for obj in self.hole_objects:
            obj_x, obj_y, obj_type = obj
            # Create a point for the hole
            if obj_type == 'BOUNDARY':
                hole_point = self.ax.plot(obj_x, obj_y, 'ko', markersize=5)[0]
            else:  # obj_type == 'HOLE'
                hole_point = self.ax.plot(obj_x, obj_y, 'bo', markersize=5)[0]
            self.hole_points.append(hole_point)
        
        # Update obstacle map
        self.obstacle_plot.set_array(self.obstacle_map.T)
        
        # Return all plot elements that need to be updated
        return (self.CyBot_point, self.CyBot_direction, self.obstacle_plot, 
                self.CyBot_circle, self.scanner_point, self.scanner_arc)

    def start_animation(self):
        """Start the animation using a timer-based approach"""
        if self.animation_running:
            self.update_plot(None)
            self.animation_timer = self.master.after(100, self.start_animation)
            
    def stop_animation(self):
        """Stop the animation"""
        self.animation_running = False
        if self.animation_timer:
            self.master.after_cancel(self.animation_timer)
            self.animation_timer = None

    def cleanup(self):
        """Clean up resources when window is closed"""
        # First stop the animation to prevent callbacks after window destruction
        self.stop_animation()
        
        # Cancel any pending color change timers
        if self.color_timer:
            self.master.after_cancel(self.color_timer)
            self.color_timer = None
            
        # Cancel any pending animation timers
        if self.animation_timer:
            self.master.after_cancel(self.animation_timer)
            self.animation_timer = None
            
        # Clear any stored objects to free memory
        self.scan_objects = []
        self.bump_objects = []
        self.hole_objects = []
        
        # Use a small delay before destroying the window to ensure all callbacks are canceled
        self.master.after(10, self._destroy_window)
        
    def _destroy_window(self):
        """Actually destroy the window after a small delay"""
        if hasattr(self, 'master') and self.master.winfo_exists():
            self.master.destroy() 