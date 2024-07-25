#include<iostream>
#include<string.h>

using namespace std;

struct Cliente {
    int id;
    char nombre[50];
    char dni[10];
    char username[25];
    char clave[25];
    double saldo;
};

struct Transaccion {
    int id;
    char username[25];
    int fecha; // Formato AAAAMMDD
    double monto;
};

const char* ARCHIVO_CLIENTES = "clientes.dat";
const char* ARCHIVO_TRANSACCIONES = "transacciones.dat";

int generarIdTransaccion();
void agregarTransaccion();
void consultarSaldo();

int main() {
    int opcion;
    do {
        cout<<"1. Agregar transaccion"<<endl;
        cout<<"2. Consultar saldo"<<endl;
        cout<<"3. Salir"<<endl;
        cout<<"Ingrese una opcion: ";
        cin>>opcion;

        switch (opcion) {
            case 1:
                agregarTransaccion();
                break;
            case 2:
                consultarSaldo();
                break;
            case 3:
                cout<<"Saliendo..."<< endl;
                break;
            default:
                cout<<"Opcion no valida."<<endl;
        }
    } while (opcion != 3);

    return 0;
}

int generarIdTransaccion() {
    FILE* arcTransacciones = fopen(ARCHIVO_TRANSACCIONES, "rb");
    if (!arcTransacciones) {
        return 1;
    }
    
    Transaccion transaccion;
    int maxId = 0;
    
    while (fread(&transaccion, sizeof(Transaccion), 1, arcTransacciones)) {
        if (transaccion.id > maxId) {
            maxId = transaccion.id;
        }
    }
    fclose(arcTransacciones);
    
    return maxId + 1;
}

void agregarTransaccion() {
    Transaccion transaccion;
    
    transaccion.id = generarIdTransaccion();
    cin.ignore(); // Ignorar el salto de linea pendiente en el bufer
    
    cout<<"Ingrese Username: ";
    cin.getline(transaccion.username, 25);
    
    cout<<"Ingrese fecha: ";
    cin>>transaccion.fecha;
    
    cout<<"Ingrese monto: ";
    cin>>transaccion.monto;

    FILE* arcTransacciones = fopen(ARCHIVO_TRANSACCIONES, "ab");
    if (arcTransacciones) {
        fwrite(&transaccion, sizeof(Transaccion), 1, arcTransacciones);
        fclose(arcTransacciones);
        
        cout<<"Transaccion agregada exitosamente."<<endl;
    } else {
        cerr<<"Error al abrir el archivo de transacciones."<<endl;
    }

    // Actualizar saldo del cliente
    FILE* arcClientes = fopen(ARCHIVO_CLIENTES, "rb+");
    if (arcClientes) {
        Cliente cliente;
        
        while (fread(&cliente, sizeof(Cliente), 1, arcClientes)) {
            if (strcmp(cliente.username, transaccion.username) == 0) {
                cliente.saldo += transaccion.monto;
                
                fseek(arcClientes, -sizeof(Cliente), SEEK_CUR);
                
                fwrite(&cliente, sizeof(Cliente), 1, arcClientes);
                
                break;
            }
        }
        fclose(arcClientes);
    } else {
        cerr<<"Error al abrir el archivo de clientes."<<endl;
    }
}

void consultarSaldo() {
    char username[25];
    char clave[25];

    cout<<"Ingrese Username: ";
    cin.ignore();
    cin.getline(username, 25);
    
    cout<<"Ingrese clave: ";
    cin.getline(clave, 25);

    FILE* arcClientes = fopen(ARCHIVO_CLIENTES, "rb");
    if (arcClientes) {
        Cliente cliente;
        bool encontrado = false;
        
        while (fread(&cliente, sizeof(Cliente), 1, arcClientes)) {
            if (strcmp(cliente.username, username) == 0 && strcmp(cliente.clave, clave) == 0) {
                cout<<"Saldo disponible: $"<<cliente.saldo<<endl;
                encontrado = true;
                break;
            }
        }
        
        if (!encontrado) {
            cout<<"Usuario o clave incorrectos."<<endl;
        }
        fclose(arcClientes);
    } else {
        cerr<<"Error al abrir el archivo de clientes."<<endl;
    }
}
