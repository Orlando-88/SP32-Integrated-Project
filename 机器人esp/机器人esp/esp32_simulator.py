import serial
import serial.tools.list_ports
import sys
import msvcrt

def calculate_checksum(cmd):
    checksum = 0
    for ch in cmd:
        checksum ^= ord(ch)
    return checksum

def build_message(command):
    checksum = calculate_checksum(command)
    return f"<{command}*{checksum:02X}>\n"

def list_ports():
    ports = serial.tools.list_ports.comports()
    if not ports:
        print("No serial ports found.")
        return None
    print("Available serial ports:")
    for i, p in enumerate(ports):
        print(f"  {i}: {p.device} - {p.description}")
    return ports

def select_port(ports):
    while True:
        try:
            idx = int(input("Select port number: "))
            if 0 <= idx < len(ports):
                return ports[idx].device
        except ValueError:
            pass
        print("Invalid selection.")

def print_menu():
    print("\n" + "=" * 50)
    print("  ESP32 Remote Control Simulator")
    print("=" * 50)
    print("  [1] MODE1:START  - Basic operation")
    print("  [2] MODE2:START  - Advanced operation")
    print("  [3] MODE3:START  - Precision control")
    print("  [4] MODE4:START  - High speed mode")
    print("  [5] MODE5:START  - Low power mode")
    print("  [0] MODE:STOP    - Stop all")
    print("  [m] Show this menu")
    print("  [q] Quit")
    print("=" * 50)
    print("You can also type custom commands and press Enter.")
    print("Examples: MODE1:START, MODE2:START, MODE:STOP, MODE1:START, TEST:HELLO")
    print("=" * 50)

def read_serial(ser):
    if ser.in_waiting > 0:
        line = ser.readline().decode('utf-8', errors='ignore').strip()
        if line:
            print(f"  << {line}")
            return line
    return None

def send_command(ser, command):
    msg = build_message(command)
    ser.write(msg.encode())
    print(f"  >> {msg.strip()}")

def main():
    print("ESP32 Serial Command Simulator")
    print("=" * 50)

    ports = list_ports()
    if not ports:
        input("Press Enter to exit.")
        return

    port = select_port(ports)

    baud = input(f"Baud rate [115200]: ").strip()
    baud = int(baud) if baud else 115200

    try:
        ser = serial.Serial(port, baud, timeout=0.1)
        print(f"Connected to {port} @ {baud} baud\n")
    except Exception as e:
        print(f"Failed to open {port}: {e}")
        input("Press Enter to exit.")
        return

    import threading
    def reader_thread():
        while ser.is_open:
            try:
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                if line:
                    print(f"  << {line}")
            except:
                break

    t = threading.Thread(target=reader_thread, daemon=True)
    t.start()

    print_menu()

    while True:
        if msvcrt.kvhit():
            key = msvcrt.getch()
            if key in (b'q', b'Q'):
                print("Quitting.")
                break
            elif key == b'1':
                send_command(ser, "MODE1:START")
            elif key == b'2':
                send_command(ser, "MODE2:START")
            elif key == b'3':
                send_command(ser, "MODE3:START")
            elif key == b'4':
                send_command(ser, "MODE4:START")
            elif key == b'5':
                send_command(ser, "MODE5:START")
            elif key == b'0':
                send_command(ser, "MODE:STOP")
            elif key in (b'm', b'M'):
                print_menu()
            elif key == b'\r':
                pass
            else:
                try:
                    ch = key.decode()
                    if ch.isprintable():
                        print(f"  (unknown key: {ch})")
                except:
                    pass

        if msvcrt.kbhit():
            line = ""
            while True:
                ch = msvcrt.getche()
                if ch in (b'\r', b'\n'):
                    print()
                    break
                elif ch == b'\x08':
                    if line:
                        line = line[:-1]
                        print('\b \b', end='', flush=True)
                else:
                    try:
                        line += ch.decode()
                    except:
                        pass

            if line.strip():
                send_command(ser, line.strip())

    ser.close()
    print("Disconnected.")

if __name__ == "__main__":
    main()
