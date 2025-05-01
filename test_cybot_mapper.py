import tkinter as tk
from new_CyBot_mapper import CyBotMapper
import time

class CyBotTest:
    def __init__(self, root):
        self.root = root
        self.root.title("CyBot Mapper Test")
        self.root.geometry("800x800")
        
        # Initialize mapper
        self.mapper = CyBotMapper(root)
        
        # Create control frame
        self.control_frame = tk.Frame(root)
        self.control_frame.pack(side='bottom', fill='x', padx=10, pady=10)
        
        # Create step buttons
        self.create_buttons()
        
    def create_buttons(self):
        # Movement controls
        tk.Button(self.control_frame, text="Move Forward 500mm", 
                 command=lambda: self.mapper.update_bot_position(500)).pack(side='left', padx=5)
        tk.Button(self.control_frame, text="Turn Right 90°", 
                 command=lambda: self.mapper.update_bot_angle(90)).pack(side='left', padx=5)
        tk.Button(self.control_frame, text="Turn Left 90°", 
                 command=lambda: self.mapper.update_bot_angle(-90)).pack(side='left', padx=5)
        
        # Hazard detection controls
        tk.Button(self.control_frame, text="Scan Obstacles", 
                 command=self.scan_obstacles).pack(side='left', padx=5)
        tk.Button(self.control_frame, text="Detect Left Bump", 
                 command=lambda: self.mapper.add_bump("left")).pack(side='left', padx=5)
        tk.Button(self.control_frame, text="Detect Right Bump", 
                 command=lambda: self.mapper.add_bump("right")).pack(side='left', padx=5)
        tk.Button(self.control_frame, text="Detect Left Hole", 
                 command=lambda: self.mapper.add_hole("hole", "left")).pack(side='left', padx=5)
        tk.Button(self.control_frame, text="Detect Right Hole", 
                 command=lambda: self.mapper.add_hole("hole", "right")).pack(side='left', padx=5)
        tk.Button(self.control_frame, text="Detect Left Boundary", 
                 command=lambda: self.mapper.add_hole("boundary", "left")).pack(side='left', padx=5)
        tk.Button(self.control_frame, text="Detect Right Boundary", 
                 command=lambda: self.mapper.add_hole("boundary", "right")).pack(side='left', padx=5)
    
    def scan_obstacles(self):
        # Scan for obstacles at different angles
        for angle in range(0, 181, 30):
            self.mapper.add_scanned_obstacle(1000, angle, 100)  # 100mm diameter obstacles
            self.root.update()  # Update the display
            time.sleep(0.2)  # Small delay to see each scan

def main():
    root = tk.Tk()
    app = CyBotTest(root)
    root.mainloop()

if __name__ == "__main__":
    main() 