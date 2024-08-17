#include<iostream>
#include<string>
#include<cstring>
#include<ctime>

using namespace std;

struct Transaccion {
    int id;
    char username[25];
    int fecha; // Formato AAAAMMDD
    double monto;
};

const char* ARCHIVO_TRANSACCIONES = "transacciones.dat";

void inicializarArchivoTransacciones();
void ordenarTransaccionesPorFecha(Transaccion transacciones[], int n);
int contarTransacciones();
int contarTransaccionesPorUsuario(const char* username);
void listarTransacciones();
void listarTransaccionesPaginadas(const char* username, int pagina, int tamPagina);
void listarIngresosEgresosPorMes(const char* username);
void mostrarTransaccionMaxima();
int obtenerFechaActual();
void mostrarClienteMasIngresosUltimos30Dias();

int main() {
    inicializarArchivoTransacciones();
	char username[25];
	int pagina;
	
    int opcion;
    do {
        cout<<"1. Listar transacciones"<<endl;
        cout<<"2. Listar transacciones de un cliente"<<endl;
        cout<<"3. Listar ingresos y egresos por mes de un cliente"<<endl;
        cout<<"4. Mostrar transaccion de monto maximo de todos los clientes"<<endl;
        cout<<"5. Mostrar cliente con mas ingresos en los ultimos 30 dias"<<endl;
        cout<<"6. Salir"<<endl;
        cout<<"Ingrese una opcion: ";
        cin>>opcion;

        switch (opcion) {
            case 1:
            	system("cls");
                listarTransacciones();
                break;
            case 2:
            	system("cls");
            	cout<<"Ingrese el username del cliente: ";
            	cin>>username;
            	
            	cout<<"Ingrese el numero de pagina: ";
            	cin>>pagina;
            	
                listarTransaccionesPaginadas(username, pagina, 5);
                break;
            case 3: 
            	system("cls");
            	cout<<"Ingrese el username del cliente: ";
            	cin>>username;
            	
            	listarIngresosEgresosPorMes(username);
            	break;
            case 4:
            	system("cls");
            	mostrarTransaccionMaxima();
            	break;
            case 5:
            	system("cls");
            	mostrarClienteMasIngresosUltimos30Dias();
            	break;
            case 6:
				cout<<"Saliendo..."<<endl;
				break; 
            default:
                cout<<"Opcion no valida."<<endl;
        }
    } while (opcion != 6);

    return 0;
}

void inicializarArchivoTransacciones() {
	FILE* arcTransacciones = fopen(ARCHIVO_TRANSACCIONES, "rb");
	
	if (!arcTransacciones) {
		arcTransacciones = fopen(ARCHIVO_TRANSACCIONES, "wb");
		
		if (arcTransacciones) {
			cout<<"Archivo de transacciones creado exitosamente."<<endl;
			fclose(arcTransacciones);
		} else {
			cerr<<"Error al crear el archivo de transacciones."<<endl;
		}
	} else {
		fclose(arcTransacciones);
	}
}

// Función para ordenar transacciones por fecha usando burbuja
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

int contarTransacciones() {
    FILE* arcTransacciones = fopen(ARCHIVO_TRANSACCIONES, "rb");
    int contador = 0;

    if (arcTransacciones) {
        Transaccion transaccion;
        while (fread(&transaccion, sizeof(Transaccion), 1, arcTransacciones)) {
            contador++;
        }
        fclose(arcTransacciones);
    }

    return contador;
}

int contarTransaccionesPorUsuario(const char* username) {
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

void listarTransacciones() {
	FILE* arcTransacciones = fopen(ARCHIVO_TRANSACCIONES, "rb");
	
	if (arcTransacciones) {
		Transaccion transaccion;
		
		while (fread(&transaccion, sizeof(Transaccion), 1, arcTransacciones)) {
			cout<<"ID-Transaccion: "<<transaccion.id<<endl;
			cout<<"Username: "<<transaccion.username<<endl;
			cout<<"Fecha: "<<transaccion.fecha<<endl;
			cout<<"Monto: "<<transaccion.monto<<endl;
			cout<<"-------------------------"<<endl;
		}
		fclose(arcTransacciones);
	} else {
		cerr<<"Error al abrir el archivo transacciones."<<endl;
	}
	
	cout<<endl;
}

// Función para listar transacciones con paginación
void listarTransaccionesPaginadas(const char* username, int pagina, int tamPagina) {
	int totalTransacciones = contarTransaccionesPorUsuario(username);
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

        // Paginación
        int inicio = (pagina - 1) * tamPagina;
        int fin = inicio + tamPagina;

        if (inicio < contador) {
            for (int i = inicio; i < fin && i < contador; i++) {
                cout << "ID-Transaccion: " << transacciones[i].id << endl;
                cout << "Fecha: " << transacciones[i].fecha << endl;
                cout << "Monto: " << transacciones[i].monto << endl;
                cout << "-------------------------" << endl;
            }
        } else {
            cout << "No hay mas transacciones para mostrar en esta pagina." << endl;
        }
    } else {
        cerr << "Error al abrir el archivo transacciones." << endl;
    }
    
    cout<<endl;
}

// Función para listar la cantidad de ingresos y egresos por mes de un cliente
void listarIngresosEgresosPorMes(const char* username) {
    FILE* arcTransacciones = fopen(ARCHIVO_TRANSACCIONES, "rb");

    if (arcTransacciones) {
        int ingresosPorMes[12] = {0};
        int egresosPorMes[12] = {0};
        Transaccion transaccion;

        while (fread(&transaccion, sizeof(Transaccion), 1, arcTransacciones)) {
            if (strcmp(transaccion.username, username) == 0) {
                int mes = (transaccion.fecha / 100) % 100 - 1;
                if (transaccion.monto > 0) {
                    ingresosPorMes[mes]++;
                } else {
                    egresosPorMes[mes]++;
                }
            }
        }
        fclose(arcTransacciones);

		cout<<username<<endl;
		
		cout<<endl;
		
        cout<<"Ingresos por mes: "<<endl;
        for (int i = 0; i < 12; i++) {
            cout<<"Mes "<<i + 1<<": "<<ingresosPorMes[i]<<" ingresos"<<endl;
        }
		
		cout<<endl;
		
        cout << "Egresos por mes: " << endl;
        for (int i = 0; i < 12; i++) {
            cout<<"Mes "<<i + 1<<": "<<egresosPorMes[i]<<" egresos"<<endl;
        }
    } else {
        cerr<<"Error al abrir el archivo transacciones."<<endl;
    }
    
    cout<<endl;
}

// Función para mostrar la transacción de monto máximo de todos los clientes
void mostrarTransaccionMaxima() {
    FILE* arcTransacciones = fopen(ARCHIVO_TRANSACCIONES, "rb");

    if (arcTransacciones) {
        Transaccion transaccion;
        Transaccion maxTransaccion;
        bool primero = true;

        while (fread(&transaccion, sizeof(Transaccion), 1, arcTransacciones)) {
            if (primero || transaccion.monto > maxTransaccion.monto) {
                maxTransaccion = transaccion;
                primero = false;
            }
        }
        fclose(arcTransacciones);

        if (!primero) {  // Si hemos encontrado alguna transacción
            cout<<"Transaccion con monto maximo:"<<endl;
            cout<<"ID-Transaccion: "<<maxTransaccion.id<<endl;
            cout<<"Username: "<<maxTransaccion.username<<endl;
            cout<<"Fecha: "<<maxTransaccion.fecha<<endl;
            cout<<"Monto: "<<maxTransaccion.monto<<endl;
        } else {
            cout<<"No se encontraron transacciones."<<endl;
        }
    } else {
        cerr<<"Error al abrir el archivo transacciones."<<endl;
    }
    
    cout<<endl;
}

// Función para obtener la fecha actual en formato AAAAMMDD
int obtenerFechaActual() {
    time_t t = time(NULL);
    tm* tiempoLocal = localtime(&t);
    int anio = tiempoLocal->tm_year + 1900;
    int mes = tiempoLocal->tm_mon + 1;
    int dia = tiempoLocal->tm_mday;
    return anio * 10000 + mes * 100 + dia;
}

// Función para mostrar el cliente que más ingresos tuvo en los últimos 30 días
void mostrarClienteMasIngresosUltimos30Dias() {
    FILE* arcTransacciones = fopen(ARCHIVO_TRANSACCIONES, "rb");

        if (arcTransacciones) {
        Transaccion transaccion;
        
        const int fechaActual = obtenerFechaActual();
        const int fechaLimite = fechaActual - 100; // 100 = aproximadamente 30 dias
        
        double ingresosPorCliente[100] = {0}; // Asume un máximo de 100 clientes diferentes
        char clientes[100][25];
        int cantClientes = 0;

        while (fread(&transaccion, sizeof(Transaccion), 1, arcTransacciones)) {
            if (transaccion.fecha >= fechaLimite && transaccion.monto > 0) {
                bool clienteExistente = false;
                for (int i = 0; i < cantClientes; i++) {
                    if (strcmp(clientes[i], transaccion.username) == 0) {
                        ingresosPorCliente[i] += transaccion.monto;
                        clienteExistente = true;
                        break;
                    }
                }
                if (!clienteExistente) {
                    strcpy(clientes[cantClientes], transaccion.username);
                    ingresosPorCliente[cantClientes] = transaccion.monto;
                    cantClientes++;
                }
            }
        }
        fclose(arcTransacciones);

        int indiceMaximo = 0;
        for (int i = 1; i < cantClientes; i++) {
            if (ingresosPorCliente[i] > ingresosPorCliente[indiceMaximo]) {
                indiceMaximo = i;
            }
        }

        if (cantClientes > 0) {
            cout<<"El cliente con mas ingresos en los ultimos 30 dias es: "<<clientes[indiceMaximo]<<endl;
            cout<<"Ingresos: "<<ingresosPorCliente[indiceMaximo]<<endl;
        } else {
            cout<<"No hay transacciones en los ultimos 30 dias."<<endl;
        }
    } else {
        cerr<<"Error al abrir el archivo transacciones."<<endl;
    }
    
    cout<<endl;
}

