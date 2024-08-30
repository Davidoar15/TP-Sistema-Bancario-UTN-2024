#include<iostream>
#include<string>
#include<cstring>
#include<ctime>
#include<stdlib.h>

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
int contarTransaccionesPorUsuario(char* username);
void listarTransaccionesPaginadas(char* username, int tamPagina);
void listarIngresosEgresosPorMes(char* username);
void mostrarTransaccionMaxima(char* username);
int obtenerFechaActual();
void mostrarClienteMasIngresosUltimos30Dias();

int main() {
    inicializarArchivoTransacciones();
	char username[25];
	int pagina;
	
	cout<<"Sistema de Analisis. Bienvenido"<<endl;
	cout<<endl;
	
    int opcion;
    do {
        cout<<"1. Listar transacciones de un cliente"<<endl;
        cout<<"2. Listar ingresos y egresos por mes de un cliente"<<endl;
        cout<<"3. Mostrar transaccion de monto maximo de todos los clientes"<<endl;
        cout<<"4. Mostrar cliente con mas ingresos en los ultimos 30 dias"<<endl;
        cout<<"5. Salir"<<endl;
        cout<<"Ingrese una opcion: ";
        cin>>opcion;

        switch (opcion) {
            case 1:
            	system("cls");
            	cout<<"Ingrese el username del cliente: ";
            	cin>>username;
            	
                listarTransaccionesPaginadas(username, 5);
                break;
            case 2: 
            	system("cls");
            	cout<<"Ingrese el username del cliente: ";
            	cin>>username;
            	
            	listarIngresosEgresosPorMes(username);
            	break;
            case 3:
            	system("cls");
            	cout<<"Ingrese el username del cliente: ";
				cin>>username;
            	
            	mostrarTransaccionMaxima(username);
            	break;
            case 4:
            	system("cls");
            	
            	mostrarClienteMasIngresosUltimos30Dias();
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

int contarTransaccionesPorUsuario(char* username) {
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

// Función para listar transacciones con paginación
void listarTransaccionesPaginadas(char* username, int tamPagina) {
	int totalTransacciones = contarTransaccionesPorUsuario(username);
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

// Función para listar la cantidad de ingresos y egresos por mes de un cliente
void listarIngresosEgresosPorMes(char* username) {
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
            cout<<endl;
        }
		
		cout<<endl;
		
        cout << "Egresos por mes: " << endl;
        for (int i = 0; i < 12; i++) {
            cout<<"Mes "<<i + 1<<": "<<egresosPorMes[i]<<" egresos"<<endl;
            cout<<endl;
        }
    } else {
        cerr<<"Error al abrir el archivo transacciones."<<endl;
    }
    
    cout<<endl;
}

// Función para mostrar la transacción de monto máximo de todos los clientes
void mostrarTransaccionMaxima(char* username) {
    FILE* arcTransacciones = fopen(ARCHIVO_TRANSACCIONES, "rb");

    if (arcTransacciones) {
        Transaccion transaccion;
        Transaccion maxTransaccion;
        bool encontrado = false;

        while (fread(&transaccion, sizeof(Transaccion), 1, arcTransacciones)) {
            if (strcmp(transaccion.username, username) == 0) {
                if (!encontrado || abs(transaccion.monto) > abs(maxTransaccion.monto)) {
                    maxTransaccion = transaccion;
                    encontrado = true;
                }
            }
        }
        fclose(arcTransacciones);

        if (encontrado) {
            cout<<"Transaccion con monto maximo para el usuario "<<username<<":"<<endl;
            cout<<"ID-Transaccion: "<<maxTransaccion.id<<endl;
            cout<<"Fecha: "<<maxTransaccion.fecha<<endl;
            cout<<"Monto: "<<maxTransaccion.monto<<endl;
        } else {
            cout<<"No se encontraron transacciones para el usuario "<<username<<"."<<endl;
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
        
        double maxIngresos = 0;
        int maxCantIngresos = 0;
        char clienteMaxIngresos[25] = "";

        while (fread(&transaccion, sizeof(Transaccion), 1, arcTransacciones)) {
            if (transaccion.fecha >= fechaLimite && transaccion.monto > 0) {
                double totalIngresos = transaccion.monto;
                int cantIngresos = 1;
                
                FILE* arcTemp = fopen(ARCHIVO_TRANSACCIONES, "rb");
                Transaccion tempTransaccion;
                
                while (fread(&tempTransaccion, sizeof(Transaccion), 1, arcTemp)) {
                    if (
						tempTransaccion.fecha >= fechaLimite && 
						tempTransaccion.monto > 0 &&
                        strcmp(transaccion.username, tempTransaccion.username) == 0 && 
                        tempTransaccion.id != transaccion.id
					) {
                        totalIngresos += tempTransaccion.monto;
                        cantIngresos++;
                    }
                }
                
                fclose(arcTemp);
                
                if (totalIngresos > maxIngresos) {
                    maxIngresos = totalIngresos;
                    maxCantIngresos = cantIngresos;
                    strcpy(clienteMaxIngresos, transaccion.username);
                }
        	}
    	}
    	
    	fclose(arcTransacciones);
    	
    	if (strlen(clienteMaxIngresos) > 0) {
            cout<<"El cliente con mas ingresos en los ultimos 30 dias es: "<<clienteMaxIngresos<<endl;
            cout<<"Cantidad de ingresos: "<<maxCantIngresos<<endl;
            cout<<"Total de ingresos: "<<maxIngresos<<endl;
        } else {
            cout<<"No hay transacciones en los ultimos 30 dias."<<endl;
        }
    } else {
        cerr<<"Error al abrir el archivo transacciones."<<endl;
    }
    
    cout<<endl;
}

