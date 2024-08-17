#include<iostream>
#include<string.h>
#include<cstring>

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
bool validarUsuario(const char* username, const char* clave, Cliente& cliente);
void agregarTransaccion();
void consultarSaldo();
void eliminarTransaccion();

int main() {
    int opcion;
    do {
        cout<<"1. Agregar transaccion"<<endl;
        cout<<"2. Consultar saldo"<<endl;
        cout<<"3. Eliminar transaccion por ID"<<endl;
        cout<<"4. Salir"<<endl;
        cout<<"Ingrese una opcion: ";
        cin>>opcion;

        switch (opcion) {
            case 1:
            	system("cls");
                agregarTransaccion();
                break;
            case 2:
            	system("cls");
                consultarSaldo();
                break;
            case 3:
            	system("cls");
                eliminarTransaccion();
                break;
            case 4:
                cout<<"Saliendo..."<< endl;
                break;
            default:
                cout<<"Opcion no valida."<<endl;
        }
    } while (opcion != 4);

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

bool validarUsuario(const char* username, const char* clave, Cliente& cliente) {
    FILE* arcClientes = fopen(ARCHIVO_CLIENTES, "rb");
    
    if (arcClientes) {
        while (fread(&cliente, sizeof(Cliente), 1, arcClientes)) {
            if (strcmp(cliente.username, username) == 0 && strcmp(cliente.clave, clave) == 0) {
                fclose(arcClientes);
                return true;
            }
        }
        fclose(arcClientes);
    }
    
    return false;
}

void agregarTransaccion() {
	char username[25], clave[25];
    
    cout<<"Ingrese Username: ";
    cin.ignore();
    cin.getline(username, 25);
    
    cout<<"Ingrese clave: ";
    cin.getline(clave, 25);
    
    Cliente cliente;
    if (!validarUsuario(username, clave, cliente)) {
    	cout<<"Usuario o clave incorrectos."<<endl;
        return;
    }
    
    Transaccion transaccion;
    transaccion.id = generarIdTransaccion();
    
    strcpy(transaccion.username, username);
    
    cout<<endl;
    
    cout<<"Sus Transacciones"<<endl;
    
    cout<<"Ingrese fecha (AAAAMMDD): ";
    cin>>transaccion.fecha;
    
    cout<<"Ingrese monto: ";
    cin>>transaccion.monto;

    char tipoTransaccion;
    cout<<"Ingreso o Egreso (I/E): ";
    cin>>tipoTransaccion;
    if (tipoTransaccion == 'E' || tipoTransaccion == 'e') {
        transaccion.monto = -transaccion.monto; // Convertir a negativo para egreso
    }

    FILE* arcTransacciones = fopen(ARCHIVO_TRANSACCIONES, "ab");
    if (arcTransacciones) {
        fwrite(&transaccion, sizeof(Transaccion), 1, arcTransacciones);
        fclose(arcTransacciones);
        
        cout<<"Transaccion realizada exitosamente."<<endl;
    } else {
        cerr<<"Error al abrir el archivo de transacciones."<<endl;
    }

    // Actualizar saldo del cliente
    cliente.saldo += transaccion.monto;

    FILE* arcClientes = fopen(ARCHIVO_CLIENTES, "rb+");
    if (arcClientes) {
        Cliente clienteTemp;
        while (fread(&clienteTemp, sizeof(Cliente), 1, arcClientes)) {
            if (strcmp(clienteTemp.username, transaccion.username) == 0) {
                // Mover el puntero de archivo hacia atrás para sobrescribir el registro del cliente
                fseek(arcClientes, -sizeof(Cliente), SEEK_CUR);
                fwrite(&cliente, sizeof(Cliente), 1, arcClientes);
                break;
            }
        }
        fclose(arcClientes);
    } else {
        cerr<<"Error al abrir el archivo de clientes."<<endl;
    }
    
    cout<<endl;
}

void consultarSaldo() {
    char username[25], clave[25];

    cout<<"Ingrese Username: ";
    cin.ignore();
    cin.getline(username, 25);
    
    cout<<"Ingrese clave: ";
    cin.getline(clave, 25);

    Cliente cliente;
    if (validarUsuario(username, clave, cliente)) {
        cout<<"Saldo disponible: $"<<cliente.saldo<<endl;
    } else {
        cout<<"Usuario o clave incorrectos. Intente de nuevo."<<endl;
    }
    
    cout<<endl;
}

void eliminarTransaccion() {
	char username[25], clave[25];
	
    cout<<"Ingrese Username: ";
    cin.ignore();
    cin.getline(username, 25);
    
    cout<<"Ingrese clave: ";
    cin.getline(clave, 25);
    
    Cliente cliente;
    if (!validarUsuario(username, clave, cliente)) {
        cout<<"Usuario o clave incorrectos."<<endl;
        return;
    }
	
    int id;
    cout<<"Ingrese el ID de la transaccion a eliminar: ";
    cin>>id;

    FILE* arcTransacciones = fopen(ARCHIVO_TRANSACCIONES, "rb");
    FILE* arcTransaccionesTemp = fopen("temp.dat", "wb");
    
    if (arcTransacciones && arcTransaccionesTemp) {
        Transaccion transaccion;
        bool encontrado = false;

        while (fread(&transaccion, sizeof(Transaccion), 1, arcTransacciones)) {
            if (transaccion.id != id) {
                fwrite(&transaccion, sizeof(Transaccion), 1, arcTransaccionesTemp);
            } else if (strcmp(transaccion.username, username) == 0) {
                encontrado = true;
                cliente.saldo -= transaccion.monto; // Revertir el saldo
            }
        }
        
        fclose(arcTransacciones);
        fclose(arcTransaccionesTemp);

        remove(ARCHIVO_TRANSACCIONES);
        rename("temp.dat", ARCHIVO_TRANSACCIONES);

        if (encontrado) {
            // Actualizar saldo en el archivo de clientes
            FILE* arcClientes = fopen(ARCHIVO_CLIENTES, "rb+");
            if (arcClientes) {
                while (fread(&cliente, sizeof(Cliente), 1, arcClientes)) {
                    if (strcmp(cliente.username, username) == 0) {
                        fseek(arcClientes, -sizeof(Cliente), SEEK_CUR);
                        fwrite(&cliente, sizeof(Cliente), 1, arcClientes);
                        break;
                    }
                }
                fclose(arcClientes);
            } else {
                cerr<<"Error al abrir el archivo de clientes."<<endl;
            }

            cout<<"Transaccion eliminada exitosamente."<<endl;
        } else {
            cout<<"No se encontro la transaccion con el ID especificado o no pertenece al usuario."<<endl;
        }
    } else {
        cerr<<"Error al abrir los archivos de transacciones."<<endl;
    }
    
    cout<<endl;
}

