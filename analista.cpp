#include <iostream>
#include <string>

using namespace std;

struct Transaccion {
    int id;
    char username[25];
    int fecha; // Formato AAAAMMDD
    double monto;
};

const char* ARCHIVO_TRANSACCIONES = "transacciones.dat";

void inicializarArchivoTransacciones();
void listarTransacciones();

int main() {
    inicializarArchivoTransacciones();

    int opcion;
    do {
        cout<<"1. Listar transacciones"<<endl;
        cout<<"2. Salir"<<endl;
        cout<<"Ingrese una opcion: ";
        cin>>opcion;

        switch (opcion) {
            case 1:
                listarTransacciones();
                break;
            case 2:
                cout<<"Saliendo..."<<endl;
                break;
            default:
                cout<<"Opcion no valida."<<endl;
        }
    } while (opcion != 2);

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

void listarTransacciones() {
	FILE* arcTransacciones = fopen(ARCHIVO_TRANSACCIONES, "rb");
	
	if (arcTransacciones) {
		Transaccion transaccion;
		
		while (fread(&transaccion, sizeof(Transaccion), 1, arcTransacciones)) {
			cout<<"ID-Transaccion: "<<transaccion.id<<endl;
			cout<<"Username: "<<transaccion.username<<endl;
			cout<<"Fecha: "<<transaccion.fecha<<endl;
			cout<<"Monto: "<<transaccion.id<<endl;
			cout<<"-------------------------"<<endl;
		}
		fclose(arcTransacciones);
	} else {
		cerr<<"Error al abrir el archivo transacciones."<<endl;
	}
}
