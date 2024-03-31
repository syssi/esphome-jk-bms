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

def crc16_jk_modbus(data : bytearray):
    crc = 0xFFFF
    for i in range(len(data)):
        crc ^= data[i]
        for j in range(8):
            if ((crc & 0x1) == 1):
                crc = int((crc / 2)) ^ 40961
            else:
                crc = int(crc / 2)

    # Invierte el orden de los bytes
    crc_bytes = crc.to_bytes(2, byteorder='little')  # Convertir a bytes (little-endian)
    crc_reversed = int.from_bytes(crc_bytes, byteorder='big')  # Convertir de nuevo a entero (big-endian)
    return format(crc_reversed & 0xFFFF, '04X')

    return format(crc & 0xFFFF,'04X')


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

        settings_asked=0
        start_time = time.time()
        address=0

        while True:
            if buffer_full==0:
               data=""
            # Read all available bytes from the serial port
            partial_data = ser.read_all()
            now = time.time()
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
                print("("+str(now-start_time)+")["+str(hex(address)) + "]:" + (str(hex_data)).upper())
                start_time=now
            if (settings_asked==0 and time.time()-start_time>0.20):
                print ("SOLICITANDO..........................................")
                # Secuencia de bytes en hexadecimal
                #byte_sequence_hex = "011016200001020000D6F1"
                byte_sequence_hex = "0110161E0001020000"           #OK 01
                byte_sequence_hex = "011016200001020000"           #OK 02
                byte_sequence_hex = "0110161C0001020000"           #OK 03

                byte_sequence_hex = byte_sequence_hex + crc16_jk_modbus(byte_sequence_hex)

#                byte_sequence_hex = "011016200001020000D6F1"           #OK 02

                # Convierte la secuencia de bytes hexadecimal a bytes
                byte_sequence = bytes.fromhex(byte_sequence_hex)

                # Escribe la secuencia de bytes en el puerto serie
                ser.write(byte_sequence)

                settings_asked=1


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




#011016200001020000D6F1
