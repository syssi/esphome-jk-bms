""" test area for using construct lib to decode a jk serial packet """
try:
    import construct as cs
    import serial
    import sys
    import argparse
    import codecs
    import time

except ImportError:
    print("You are missing dependencies")
    print("To install use:")
    print("    python -m pip install 'construct'")

def get_bit_value(number, position):
    # Shift the bit we're interested in to the lowest position
    shifted_bit = number >> position
    # Apply a mask to get only the bit we're interested in
    bit_value = shifted_bit & 1
    return bit_value

def read_serial_port(serial_port, baud_rate):
    with serial.Serial(serial_port, baud_rate, timeout=10) as ser:
        timestr = time.strftime("%Y%m%d_%H%M%S")
        file = codecs.open(timestr+".log", "w", "utf-8")
        data=""
        buffer_full=0

        while True:
            if buffer_full==0:
               data=""
            # Read all available bytes from the serial port
            partial_data = ser.read_all()
            while partial_data:
                 longitud_datos = len(partial_data)
                 if longitud_datos>=256:
                     buffer_full=1
                 else:
                     buffer_full=0
                 # Hacer algo con los datos parciales
                 # ...
                 data = data + partial_data.hex()
                 partial_data = ser.read_all()

#            print ("DATA:"+data)
#            print ("BUFF:"+str(buffer_full))

            if data and buffer_full==0:
                hex_data = data   #.hex()
#                print(hex_data, end='\n')
                file.write(hex_data)
                file.write('\n')
                sys.stdout.flush()
                bytes_data = bytes.fromhex(hex_data)
                first_byte = bytes_data[0]
                if first_byte <= 0x0f:
                     address=first_byte
                print("["+str(hex(address)) + "]:" + str(hex_data))

        # If there is no more data, exit the loop
        sys.stdout.flush()


# print(len(response))



if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Just an example",formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("-a", "--archive", action="store_true", help="archive mode")
    parser.add_argument("-v", "--verbose", action="store_true", help="increase verbosity")
    parser.add_argument("--input-file", help="Source location")
    args = parser.parse_args()
    config = vars(args)
    print(config)
    if (config['input_file']==None):
       # Serial port configuration (adjust the port and baud rate based on your setup)
       serial_port = '/dev/ttyUSB0'  # Change this to the serial port you are using
       baud_rate = 115200  # Change this to the baud rate of your device
       read_serial_port(serial_port, baud_rate)
    else:
       # Using readlines()
       file1 = open(config['input_file'], 'r')
       Lines = file1.readlines()
       count = 0
       # Strips the newline character
       for line in Lines:
           count += 1
           text=line.strip()
           ##print("Line{}: {}".format(count, text))

           bytes_data = bytes.fromhex(text)
           first_byte = bytes_data[0]
           if first_byte <= 0x0f:
                address=first_byte
           print("["+str(hex(address)) + "]:" + str(line))

