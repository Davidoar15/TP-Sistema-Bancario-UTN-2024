#include<iostream>
#include<string.h>
#include<cstring>
#include<stdlib.h>

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

void inicializarArchivoTransacciones();
int generarIdTransaccion();
bool validarUsuario(char* username, char* clave, Cliente& cliente);
bool comprobarSaldo(Cliente& cliente, double monto);
void actualizarSaldo(Cliente& cliente);
void agregarTransaccion();
void eliminarTransaccion();
void consultarSaldo();
int contarTransacciones(char* username);
void ordenarTransaccionesPorFecha(Transaccion transacciones[], int n);
void listarTransaccionesPaginadas(int tamPagina);

int main() {
    inicializarArchivoTransacciones();
	cout<<"Bienvenido al Gestor de Transacciones"<<endl;
	cout<<endl;

    int opcion;
    do {
        cout<<"1. Agregar transaccion"<<endl;
        cout<<"2. Mis transacciones"<<endl;
        cout<<"3. Consultar saldo"<<endl;
        cout<<"4. Eliminar transaccion por ID"<<endl;
        cout<<"5. Salir"<<endl;
        cout<<"Ingrese una opcion: ";
        cin>>opcion;

        switch (opcion) {
            case 1:
                system("cls");
                
                agregarTransaccion();
                break;
            case 2: 
            	system("cls");
            	
            	listarTransaccionesPaginadas(5);
            	break;
            case 3:
                system("cls");
                
                consultarSaldo();
                break;
            case 4:
                system("cls");
                
                eliminarTransaccion();
                break;
            case 5:
            	cout<<endl;
                cout<<"Saliendo..."<<endl;
                break;
            default:
                cout<<"Opcion no valida."<<endl;
                cout<<endl;
        }
    } while (opcion != 5);

    return 0;
}

void inicializarArchivoTransacciones() {
    FILE* arcTransacciones = fopen(ARCHIVO_TRANSACCIONES, "rb");

    if (!arcTransacciones) {
        arcTransacciones = fopen(ARCHIVO_TRANSACCIONES, "wb");

        if (arcTransacciones) {
            fclose(arcTransacciones);
        } else {
            cerr<<"Error al crear el archivo de transacciones."<<endl;
        }
    }
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

bool validarUsuario(char* username, char* clave, Cliente& cliente) {
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

bool comprobarSaldo(Cliente& cliente, double monto) {
    if (cliente.saldo < monto) {
        cout<<"Saldo insuficiente. No se puede realizar la transaccion."<<endl;
        cout<<endl;
        return false;
    }
    return true;
}

void actualizarSaldo(Cliente& cliente) {
	FILE* arcClientes = fopen(ARCHIVO_CLIENTES, "rb+");
    if (arcClientes) {
        Cliente clienteTemp;
        while (fread(&clienteTemp, sizeof(Cliente), 1, arcClientes)) {
            if (strcmp(clienteTemp.username, cliente.username) == 0) {
                fseek(arcClientes, -sizeof(Cliente), SEEK_CUR);
                fwrite(&cliente, sizeof(Cliente), 1, arcClientes);
                break;
            }
        }
        fclose(arcClientes);
    } else {
        cerr << "Error al abrir el archivo de clientes." << endl;
    }
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
    	cout<<endl;
        return;
    }
	
    Transaccion transaccion;
    transaccion.id = generarIdTransaccion();

    strcpy(transaccion.username, cliente.username);

    cout<<"Ingrese fecha (AAAAMMDD): ";
    cin>>transaccion.fecha;

    cout<<"Ingrese monto: ";
    cin>>transaccion.monto;

    char tipoTransaccion;
    cout<<"Ingreso o Egreso (I/E): ";
    cin>>tipoTransaccion;
    if (tipoTransaccion == 'E' || tipoTransaccion == 'e') {
    	if (!comprobarSaldo(cliente, transaccion.monto)) {
            return; // Si el saldo es insuficiente, no se realiza la transacción
        }
        
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

	cliente.saldo += transaccion.monto;
    actualizarSaldo(cliente);

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
        	if (transaccion.id == id && strcmp(transaccion.username, cliente.username) == 0) {
            	encontrado = true;
				cliente.saldo -= transaccion.monto;
        	} else {
            	fwrite(&transaccion, sizeof(Transaccion), 1, arcTransaccionesTemp);
        	}
        }

        fclose(arcTransacciones);
        fclose(arcTransaccionesTemp);

        remove(ARCHIVO_TRANSACCIONES);
        rename("temp.dat", ARCHIVO_TRANSACCIONES);

        if (encontrado) {
        	actualizarSaldo(cliente);
            cout<<"Transaccion eliminada exitosamente."<<endl;
        } else {
            cout<<"No se encontro la transaccion con el ID especificado o no pertenece al usuario."<<endl;
        }
    } else {
        cerr<<"Error al abrir los archivos de transacciones."<<endl;
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

int contarTransacciones(char* username) {
    FILE* arcTransacciones = fopen(ARCHIVO_TRANSACCIONES, "rb");
    int contador = 0;

    if (arcTransacciones) {
        Transaccion transaccion;
        while (fread(&transaccion, sizeof(Transaccion), 1, arcTransacciones)) {
            if (strcmp(transaccion.username, username) == 0) {
                contador++;
            }
        }
        fclose(arcTransacciones);
    }

    return contador;
}

void ordenarTransaccionesPorFecha(Transaccion transacciones[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (transacciones[j].fecha > transacciones[j + 1].fecha) {
                Transaccion temp = transacciones[j];
                transacciones[j] = transacciones[j + 1];
                transacciones[j + 1] = temp;
            }
        }
    }
}

void listarTransaccionesPaginadas(int tamPagina) {
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
    
	int totalTransacciones = contarTransacciones(cliente.username);
	if (totalTransacciones == 0) {
        cout<<"No hay transacciones para mostrar."<<endl;
        cout<<endl;
        
        return;
    }
	
	Transaccion* transacciones = new Transaccion[totalTransacciones]; //Array Dinamico
	
    FILE* arcTransacciones = fopen(ARCHIVO_TRANSACCIONES, "rb");

    if (arcTransacciones) {
        int contador = 0;
        Transaccion transaccion;

        // Filtrar por username y cargar en el array
        while (fread(&transaccion, sizeof(Transaccion), 1, arcTransacciones)) {
            if (strcmp(transaccion.username, username) == 0) {
                transacciones[contador++] = transaccion;
            }
        }
        fclose(arcTransacciones);

        // Ordenar por fecha
        ordenarTransaccionesPorFecha(transacciones, contador);

        int pagina = 1;
        int totalPaginas = (contador + tamPagina - 1) / tamPagina;

        while (true) {
            int inicio = (pagina - 1) * tamPagina;
            int fin = inicio + tamPagina;

            cout<<"Pagina "<<pagina<<" de "<<totalPaginas<<endl;
            cout<<"-------------------------"<<endl;

            if (inicio < contador) {
                for (int i = inicio; i < fin && i < contador; i++) {
                    cout<<"ID-Transaccion: "<<transacciones[i].id<<endl;
                    cout<<"Fecha: "<<transacciones[i].fecha<<endl;
                    cout<<"Monto: "<<transacciones[i].monto<<endl;
                    cout<<"-------------------------"<<endl;
                }

                if (pagina < totalPaginas) {
                    cout<<"Presione Enter para ver la siguiente pagina..."<<endl;
                    cin.ignore();
                    cin.get();
                    pagina++;
                } else {
                    cout<<"No hay mas paginas para mostrar."<<endl;
                    break;
                }
            } else {
                cout<<"No hay transacciones para mostrar en esta pagina."<<endl;
                break;
            }
        }
    } else {
        cerr<<"Error al abrir el archivo transacciones."<<endl;
    }
    
    delete[] transacciones;
    cout<<endl;
}

