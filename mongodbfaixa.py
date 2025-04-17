from pymongo import MongoClient
from datetime import datetime

connection_string = f"mongodb+srv://" #Adicione sua conexão do seu banco MongoDB
client = MongoClient(connection_string)

BancodadosDb = client.tccDB
tccDB_col = BancodadosDb.placas

def inserir_placa(placa):
    agora = datetime.now()
    data = agora.strftime("%d/%m/%y")
    horas = agora.strftime("%H:%M:%S")
    placas = {
        "placa": placa,
        "data": data,
        "horas": horas
    }
    if tccDB_col.count_documents({"placa": placa}) > 0:
        print(f"A placa '{placa}' já existe")
    else:        
        tccDB_col.insert_one(placas)