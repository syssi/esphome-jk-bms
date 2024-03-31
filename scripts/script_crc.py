
def crc16_c(data : bytearray):
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

#    return format(crc & 0xFFFF,'04X')


# Lista de mensajes con CRC
mensajes = [
    "001016200001059a",
    "011016200001020000d6f1",
    "011016200001044b",
    "021016200001020000c201",
    "031016200001020000cf91",
    "041016200001020000e9a1",
    "051016200001020000e431",
    "061016200001020000f0c1",
    "071016200001020000fd51",
    "081016200001020000bca1",
    "091016200001020000b131",
    "0a1016200001020000a5c1",
    "0b1016200001020000a851",
    "0c10162000010200008e61",
    "0d101620000102000083f1",
    "0e10162000010200009701",
    "0f10162000010200009a91",
    "0010161e00016456",
    "0110161e0001020000d22f",
    "0010161C0001020000DE5D",
    "0110161C0001020000D3CD"

]

# Verificar CRC de cada mensaje
for mensaje in mensajes:
    mensaje=mensaje.upper()
    mensaje_sin_crc = mensaje[:-4]
    bytes_mensaje = bytes.fromhex(mensaje_sin_crc)

    crc_calculado = crc16_c(bytes_mensaje)
    crc_esperado = mensaje[-4:]

    if crc_calculado == crc_esperado:
        print(f"El CRC del mensaje {mensaje} es correcto.")
    else:
        print(f"El CRC del mensaje {mensaje} es incorrecto. CRC calculado: {crc_calculado}, CRC esperado: {crc_esperado}")
