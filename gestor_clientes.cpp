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

const char* ARCHIVO_CLIENTES = "clientes.dat";

void inicializarArchivoClientes();
int generarIdCliente();
bool existeCliente(const char* dni, const char* username);
void agregarCliente();
void mostrarClientes();

int main() {
    inicializarArchivoClientes();

    int opcion;
    do {
        cout<<"1. Agregar cliente"<<endl;
        cout<<"2. Mostrar clientes"<<endl;
        cout<<"3. Salir"<<endl;
        cout<<"Ingrese una opcion: ";
        cin>>opcion;

        switch (opcion) {
            case 1:
                agregarCliente();
                break;
            case 2:
                mostrarClientes();
                break;
            case 3:
                cout<<"Saliendo..."<<endl;
                break;
            default:
                cout<<"Opcion no valida."<<endl;
        }
    } while (opcion != 3);

    return 0;
}

void inicializarArchivoClientes() {
	FILE* arcClientes = fopen(ARCHIVO_CLIENTES, "rb");
	
	if (!arcClientes) {
		arcClientes = fopen(ARCHIVO_CLIENTES, "wb");
		
		if (arcClientes) {
			cout<<"Archivo de clientes creado exitosamente."<<endl;
		} else {
			cerr<<"Error al crear el archivo de clientes."<<endl;
		}
	}
	
	if (arcClientes) {
		fclose(arcClientes);
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
    	cout << "Error: Ya existe un cliente con mismo DNI o username." << endl;
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
}

void mostrarClientes() {
	FILE* arcClientes = fopen(ARCHIVO_CLIENTES, "rb");
	if (arcClientes) {
		Cliente cliente;
		
		while (fread(&cliente, sizeof(Cliente), 1, arcClientes)) {
			cout<<"ID-Cliente: " << cliente.id <<endl;
            cout<<"Nombre: " << cliente.nombre <<endl;
            cout<<"DNI: " << cliente.dni <<endl;
            cout<<"Username: " << cliente.username <<endl;
            cout<<"Saldo: " << cliente.saldo <<endl;
            cout<<"-------------------------" <<endl;
		}
		fclose(arcClientes);
	} else {
		cerr<<"Error al abrir el archivo de clientes."<<endl;
	}
}
