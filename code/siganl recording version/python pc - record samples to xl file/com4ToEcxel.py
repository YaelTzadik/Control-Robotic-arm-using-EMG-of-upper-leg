import serial
import serial.tools.list_ports
import pandas as pd
from datetime import datetime
import keyboard
import os

# List available USB ports
available_ports = list(serial.tools.list_ports.comports())
print("Available USB ports:")
for port in available_ports:
    print(f" - {port.device}")

# Define the USB port and baud rate
USB_PORT = 'COM9'  # Change this to your specific port
BAUD_RATE = 9600  # Set the baud rate to 115200

# Flag to control reading
reading = False

# Data list to store captured data
data_list = []

# Counter for file naming
file_counter = 1
prev_timestamp = datetime.now()
try:
    # Create a serial object
    ser = serial.Serial(USB_PORT, BAUD_RATE, timeout=1)
    print(f"Connected to {USB_PORT} at {BAUD_RATE} baud rate")

    print("Press 's' to start reading data and 'd' to stop.")

    while True:
        if keyboard.is_pressed('s'):
            if not reading:
                reading = True
                prev_timestamp = datetime.now()
                print("Started reading data.")

        if keyboard.is_pressed('d'):
            if reading:
                reading = False
                print("Stopped reading data.")

                # Save the data to an Excel file
                if data_list:
                    # Create column names dynamically
                    max_parts = max(len(row) for row in data_list)
                    columns = ['Timestamp'] + [f'Column {i}' for i in range(1, max_parts)]

                    # Generate unique filename
                    filename = f"sample{file_counter}.xlsx"
                    file_counter += 1

                    # Check if file already exists and increment counter if necessary
                    while os.path.exists(filename):
                        filename = f"sample{file_counter}.xlsx"
                        file_counter += 1

                    df = pd.DataFrame(data_list, columns=columns)
                    df.to_excel(filename, index=False)
                    print(f"Data saved to {filename}")

                    # Clear data_list after saving
                    data_list = []

        if reading and ser.in_waiting > 0:
            data = ser.readline().decode('utf-8').rstrip()
            timestamp = datetime.now()
            print(f"Received data: {data} at {timestamp}")
            data_parts = data.split(',')  # Split data by comma
            diff_timestamp = (timestamp - prev_timestamp).total_seconds()*(10**6) #in micro seconds
            data_list.append([diff_timestamp] + data_parts)
            prev_timestamp = timestamp

except serial.SerialException as e:
    print(f"Error: {e}")

except KeyboardInterrupt:
    print("\nExiting program")
