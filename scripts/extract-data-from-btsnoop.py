import argparse
import json
import os

def extract_btatt_values(json_data):
  extracted_data = []

  for entry in json_data:
    if '_source' in entry and 'layers' in entry["_source"] and 'btatt' in entry["_source"]["layers"] and 'btatt.value' in entry["_source"]["layers"]["btatt"]:
        btatt_value = entry["_source"]["layers"]["btatt"]["btatt.value"]
        extracted_data.append(''.join(btatt_value))
  return extracted_data

def append_values(values): #beginng of a packet is "55:aa:eb:90", append every packet after that till we hit "55:aa:eb:90" again
  appended_values = []
  current_packet = None
  
  for value in values:
      if value.startswith("55:aa:eb:90"):
          if current_packet: #add the previus combined value
              appended_values.append(''.join(current_packet))
          current_packet = [value]
      else:
          current_packet.append(':' + value)

  if current_packet and len(current_packet) == 4:
      appended_values.append(''.join(current_packet))
      
  return appended_values
         

def values_to_vertical_string(data):
  hex_values = []
  output_string = ""
  for value in data:
    hex_values.append(value.split(":"))
  matrix = [[0 for _ in range(len(hex_values))] for _ in range(len(hex_values[0]))] 
  
  for i_col, row in enumerate(hex_values):

    for i_row, hex_val in enumerate(row):
      if i_row >= 300:
        break
      # print(i_row)
      # print(i_col)
      matrix[i_row][i_col] = hex_val +" "

  for i, col in enumerate(matrix):
    output_string += str(i).zfill(3) + " \t"
    print("col")
    print(col)
    for row in col:
      output_string += str(row)
    output_string += "\n"
  return output_string

def load_json_from_file(file_path):
    with open(file_path, 'r') as file:
        data = json.load(file)
    return data
  
def save_to_file(file_name, data):
  with open(file_name + "_output", 'w') as file:
      file.write(data)
      file.close()

parser = argparse.ArgumentParser(description='')
parser.add_argument('file_path', type=str, help='Path to JSON-Datei')
args = parser.parse_args()
file_path = args.file_path

json_data = load_json_from_file(file_path)
btatt_values = extract_btatt_values(json_data)
appended_values = append_values(btatt_values)
vertival_string = values_to_vertical_string(appended_values)

file_name, file_extension = os.path.splitext(file_path)
save_to_file(file_name, vertival_string)