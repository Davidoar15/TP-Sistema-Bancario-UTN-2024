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
bool validarUsuario(const char* username, const char* clave, Cliente& cliente);
void agregarTransaccion(const Cliente& cliente);
void consultarSaldo(const char* username);
void eliminarTransaccion(Cliente& cliente);
int contarTransacciones(const char* username);
void ordenarTransaccionesPorFecha(Transaccion transacciones[], int n);
void listarTransaccionesPaginadas(const char* username, int tamPagina);

int main() {
    inicializarArchivoTransacciones();
    
    char username[25], clave[25];
    Cliente cliente;

    bool usuarioValido = false;
    do {
        cout<<"Ingrese Username: ";
        cin.getline(username, 25);
    
        cout<<"Ingrese clave: ";
        cin.getline(clave, 25);

        if (!validarUsuario(username, clave, cliente)) {
            cout<<"Usuario o clave incorrectos. Intente de nuevo."<<endl;
            cout<<endl;
        } else {
            usuarioValido = true;
        }
    } while (!usuarioValido);

	system("cls");
	cout<<"Bienvenido "<<cliente.username<<endl;
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
                
                agregarTransaccion(cliente);
                break;
            case 2: 
            	system("cls");
            	
            	listarTransaccionesPaginadas(cliente.username, 5);
            	break;
            case 3:
                system("cls");
                
                consultarSaldo(cliente.username);
                break;
            case 4:
                system("cls");
                
                eliminarTransaccion(cliente);
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
    } else {
        fclose(arcTransacciones);
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

void agregarTransaccion(const Cliente& cliente) {
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
    Cliente clienteActualizado = cliente;
    clienteActualizado.saldo += transaccion.monto;

    FILE* arcClientes = fopen(ARCHIVO_CLIENTES, "rb+");
    if (arcClientes) {
        Cliente clienteTemp;
        while (fread(&clienteTemp, sizeof(Cliente), 1, arcClientes)) {
            if (strcmp(clienteTemp.username, transaccion.username) == 0) {
                fseek(arcClientes, -sizeof(Cliente), SEEK_CUR);
                fwrite(&clienteActualizado, sizeof(Cliente), 1, arcClientes);
                break;
            }
        }
        fclose(arcClientes);
    } else {
        cerr<<"Error al abrir el archivo de clientes."<<endl;
    }

    cout<<endl;
}

void consultarSaldo(const char* username) {
    FILE* arcClientes = fopen(ARCHIVO_CLIENTES, "rb");
    if (arcClientes) {
        Cliente cliente;
        bool clienteEncontrado = false;

        // Leer clientes del archivo
        while (fread(&cliente, sizeof(Cliente), 1, arcClientes)) {
            // Comparar username
            if (strcmp(cliente.username, username) == 0) {
                cout<<"Saldo disponible: $"<<cliente.saldo<<endl;
                clienteEncontrado = true;
                break;
            }
        }
        fclose(arcClientes);

        if (!clienteEncontrado) {
            cout<<"Cliente no encontrado."<<endl;
        }
    } else {
        cerr<<"Error al abrir el archivo de clientes."<<endl;
    }

    cout<<endl;
}

void eliminarTransaccion(Cliente& cliente) {
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
            } else if (strcmp(transaccion.username, cliente.username) == 0) {
                encontrado = true;
                // Revertir saldo
                if (transaccion.monto > 0) {
                    cliente.saldo -= transaccion.monto;
                } else {
                    cliente.saldo += abs(transaccion.monto);
                }
            }
        }

        fclose(arcTransacciones);
        fclose(arcTransaccionesTemp);

        remove(ARCHIVO_TRANSACCIONES);
        rename("temp.dat", ARCHIVO_TRANSACCIONES);

        if (encontrado) {
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

int contarTransacciones(const char* username) {
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

void listarTransaccionesPaginadas(const char* username, int tamPagina) {
	int totalTransacciones = contarTransacciones(username);
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
/*
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
bool validarUsuario(const char* username, const char* clave, Cliente& cliente);
void agregarTransaccion();
void consultarSaldo();
void eliminarTransaccion();

int main() {
	inicializarArchivoTransacciones();
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

void inicializarArchivoTransacciones() {
	FILE* arcTransacciones = fopen(ARCHIVO_TRANSACCIONES, "rb");
	
	if (!arcTransacciones) {
		arcTransacciones = fopen(ARCHIVO_TRANSACCIONES, "wb");
		
		if (arcTransacciones) {
			fclose(arcTransacciones);
		} else {
			cerr<<"Error al crear el archivo de transacciones."<<endl;
		}
	} else {
		fclose(arcTransacciones);
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
                // Revertir el saldo
                if (transaccion.monto > 0) {
                    cliente.saldo -= transaccion.monto; // Revertir ingreso
                } else {
                    cliente.saldo += abs(transaccion.monto); // Revertir egreso
                }
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
*/
