import mongodbfaixa
import cv2
import mqtt
from pathlib import Path
from PIL import Image
from pytesseract import pytesseract


camera = cv2.VideoCapture(0, cv2.CAP_DSHOW)


def tesseract(img_name, type):
    path_to_tesseract = r'C:\Program Files\Tesseract-OCR\tesseract.exe' #Adicione aonde seu tesseract foi instalado na sua maquina 
    image_path = f'{img_name}.jpg'
    pytesseract.tesseract_cmd = path_to_tesseract
    text = pytesseract.image_to_string(Image.open(image_path))
    text = text.replace("\n", '')
    if len(text) == 8 and text[:3].isalpha() and text[3:4].isascii() and text[4:].isnumeric() or len(text) == 7 and text[:3].isalpha() and text[3:4].isnumeric() and text[4:5].isalpha() and text[5:].isnumeric():
        if type == 'INSERT':
            mongodbfaixa.inserir_placa(text)
    else:
        print("A placa não foi lida corretamente: " + text)

def entradaLeitura():
    if mqtt.topic == 'esp/faixa/carro' and mqtt.payload == '1':
                mqtt.client.loop_stop()
                cv2.imwrite('test1.jpg', image)
                tesseract('test1', 'INSERT')
                print('leu entrada')


while True:
    _, image = camera.read()
    cv2.imshow('Text detection', image)
    entradaLeitura()
    mqtt.client.loop_start()
    if cv2.waitKey(1) & 0xFF == ord('e'):
        break


camera.release()
cv2.destroyAllWindows()