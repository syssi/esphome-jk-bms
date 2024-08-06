import argparse
import json
import os


def extract_btatt_values(json_data):
    extracted_data = []

    for entry in json_data:
        if (
            "_source" in entry
            and "layers" in entry["_source"]
            and "btatt" in entry["_source"]["layers"]
            and "btatt.value" in entry["_source"]["layers"]["btatt"]
        ):
            btatt_value = entry["_source"]["layers"]["btatt"]["btatt.value"]
            extracted_data.append("".join(btatt_value))
    return extracted_data


def append_values(
    values, frame_type: int
):  # beginng of a packet is "55:aa:eb:90", append every packet after that till we hit "55:aa:eb:90" again
    list_of_frame_values = []
    current_packet = None

    start_value = "55:aa:eb:90"
    start_value_with_frame_type = start_value
    if not frame_type == 0:
        start_value_with_frame_type += ":" + str(frame_type).zfill(2)

    for index_of_value, value in enumerate(values):
        if value.startswith(
            start_value_with_frame_type
        ):  # 55:aa:eb:90 or 55:aa:eb:90:[01,02,03]
            current_packet = value
            if index_of_value + 1 < len(values):
                for value_to_append in values[index_of_value + 1 :]:
                    # 55:aa:eb:90 start of the next frame or "aa:55" command response??
                    if value_to_append.startswith(
                        start_value
                    ) or value_to_append.startswith("aa:55"):
                        list_of_frame_values.append("".join(current_packet))
                        break
                    else:
                        current_packet += ":" + value_to_append

    return list_of_frame_values


def values_to_vertical_string(data):
    hex_values = []
    output_string = ""
    for value in data:
        hex_values.append(value.split(":"))
    if hex_values == []:
        return ""
    matrix = [
        [0 for _ in range(len(hex_values))]
        for _ in range(max(len(inner_list) for inner_list in hex_values))
    ]

    for i_col, row in enumerate(hex_values):
        for i_row, hex_val in enumerate(row):
            # if i_row >= 300:
            #   break
            matrix[i_row][i_col] = hex_val + " "

    for i, col in enumerate(matrix):
        output_string += str(i).zfill(3) + " \t"
        for row in col:
            output_string += str(row)
        output_string += "\n"
    return output_string


def load_json_from_file(file_path):
    with open(file_path) as file:
        data = json.load(file)
    return data


def save_to_file(file_name, data):
    with open(file_name + ".txt", "w") as file:
        file.write(data)
        file.close()


parser = argparse.ArgumentParser(description="")
parser.add_argument("file_path", type=str, help="Path to JSON-Datei")
parser.add_argument(
    "--frame_type",
    type=int,
    default=0,
    help="Frame Type 0 -> All, 1 -> settingsframe, 2 -> cell info frame, 3 device info frame",
)
args = parser.parse_args()
file_path = args.file_path
frame_type = args.frame_type

json_data = load_json_from_file(file_path)
btatt_values = extract_btatt_values(json_data)
appended_values = append_values(btatt_values, frame_type)
vertival_string = values_to_vertical_string(appended_values)

file_name, file_extension = os.path.splitext(file_path)
save_to_file(file_name, vertival_string)
print("saved to " + file_name + "_output.log")
