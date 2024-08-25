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

const char* ARCHIVO_CLIENTES = "clientes.dat";

void inicializarArchivoClientes();
int generarIdCliente();
bool existeCliente(const char* dni, const char* username);
void agregarCliente();

int main() {
    inicializarArchivoClientes();

	cout<<"Sea bienvenido a nuestro registro del Banco"<<endl;
	cout<<endl;

    int opcion;
    do {
        cout<<"1. Agregar cliente"<<endl;
        cout<<"2. Salir"<<endl;
        cout<<"Ingrese una opcion: ";
        cin>>opcion;

        switch (opcion) {
            case 1:
            	system("cls");
            	
                agregarCliente();
                break;
            case 2:
            	cout<<endl;
                cout<<"Saliendo..."<<endl;
                break;
            default:
                cout<<"Opcion no valida."<<endl;
                cout<<endl;
        }
    } while (opcion != 2);

    return 0;
}

void inicializarArchivoClientes() {
	FILE* arcClientes = fopen(ARCHIVO_CLIENTES, "rb");
	
	if (!arcClientes) {
		arcClientes = fopen(ARCHIVO_CLIENTES, "wb");
		
		if (arcClientes) {
			fclose(arcClientes);
		} else {
			cerr<<"Error al crear el archivo de clientes."<<endl;
		}
	}
}

int generarIdCliente() {
	FILE* arcClientes = fopen(ARCHIVO_CLIENTES, "rb");
	
	if (!arcClientes) {
		return 1;
	}
	
	Cliente cliente;
	int maxId = 0;
	
	while (fread(&cliente, sizeof(Cliente), 1, arcClientes)) {
		if (cliente.id > maxId) {
			maxId = cliente.id;
		}
	}
	fclose(arcClientes);
	
	return maxId + 1;
}

bool existeCliente(const char* dni, const char* username) {
	FILE* arcClientes = fopen(ARCHIVO_CLIENTES, "rb");
	
	if (!arcClientes) {
		return false;
	}
	
	Cliente cliente;
	
	while (fread(&cliente, sizeof(Cliente), 1, arcClientes)) {
		if (strcmp(cliente.dni, dni) == 0 || strcmp(cliente.username, username) == 0) {
			fclose(arcClientes);
			return true;
		}
	}
	fclose(arcClientes);
	
	return false;
}

void agregarCliente() {
	Cliente cliente;
	
	cliente.id = generarIdCliente();
	cin.ignore();
	
	cout<<"Ingrese nombre: ";
	cin.getline(cliente.nombre, 50);
	
	cout<<"Ingrese DNI: ";
	cin.getline(cliente.dni, 20);
	
	cout<<"Ingrese un nombre de usuario (Username): ";
	cin.getline(cliente.username, 25);
	
	cout<<"Ingrese clave para hacer Transacciones: ";
	cin.getline(cliente.clave, 25);
	
	if (existeCliente(cliente.dni, cliente.username)) {
    	cout<<"Error: Ya existe un cliente con mismo DNI o username."<<endl;
    	cout<<endl;
    	return;
    }
	
	cliente.saldo = 10000.0;
	
	FILE* arcClientes = fopen(ARCHIVO_CLIENTES, "ab");
	if (arcClientes) {
		fwrite(&cliente, sizeof(Cliente), 1, arcClientes);
		fclose(arcClientes);
		
		cout<<"Cliente registrado exitosamente."<<endl;
	} else {
		cerr<<"Error al abrir el archivo de clientes."<<endl;
	}
	
	cout<<endl;
}

