#define _XOPEN_SOURCE 500
#define _POSIX_C_SOURCE 200809L

#include "heap.h"
#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define TAM_INICIAL 200;
#define TAM_ARREGLO 100;

/* ******************************************************************
 *                             Split
 * *****************************************************************/

int cant_parametro(const char* str, char sep){
	
	int pos=0;
	int contador=0;
	while( str[pos]!='\0'){
		if(str[pos]==sep)contador++;
		pos++;
	}
	return contador;
}

int cant(const char *str, char sep){
	int i=0;
	while(str[i]!= sep && str[i]!='\0'){
		i++;
	}
	return i;
}

char** split (const char* str, char sep){
	
	if(sep=='\0') return NULL;
	int tam_arreglo=cant_parametro(str,sep)+1;
	char** arreglo_aux= malloc(sizeof(char*)*(tam_arreglo+1));
	if(arreglo_aux==NULL)return NULL;
	const char* puntero = str;
	for(int i=0;i<tam_arreglo;i++){
			int bytes = cant(puntero,sep);
			char* cadena = malloc(sizeof(char)*(bytes+1));
			memcpy(cadena,puntero,bytes);
			cadena[bytes]='\0';
			arreglo_aux[i]=cadena;
			puntero+=(bytes+1);
		}
	arreglo_aux[tam_arreglo]=NULL;
	return arreglo_aux;
}


void free_strv(char *strv[]){
	int i = 0;
	while(strv[i] != NULL){
		free(strv[i]);
		i++;
	}
	free(strv);
}

/* ******************************************************************
 *                         NODO HEAP
 * *****************************************************************/
 
typedef struct nodo_h{
	
	char* clave;
	void* dato;	
	
} nodo_h_t; 
 
nodo_h_t* nodo_h_crear(char* clave, void* dato){
	
	nodo_h_t* nodo= malloc(sizeof(nodo_h_t));
	nodo->clave= clave;
	nodo->dato=dato;
	return nodo;
}

int comparar_nodos (nodo_h_t* nodo_1, nodo_h_t* nodo_2){
	if (nodo_1->dato < nodo_2->dato)return 1;
	if(nodo_1->dato == nodo_2->dato)return 0;
	else{
		return -1;
		} //ver de dar vuelta
}

void* nodo_dato(nodo_h_t* nodo){
	return nodo->dato;
}

char* nodo_clave(nodo_h_t* nodo){
	return nodo->clave;
}

/* ******************************************************************
 *                        PROCESAR USUARIOS
 * *****************************************************************/
 
int comparar_nodos_inverso (nodo_h_t* nodo_1, nodo_h_t* nodo_2);
void ordenar_nombres(void** arreglo, int largo, cmp_func_t cmp);
int comparar_menor_mayor(const void *a, const void *b);

int procesar_usuarios(char* nom_archivo){
	
	FILE* archivo;
	archivo = fopen(nom_archivo,"r");
	if (!archivo){
		printf("Error al abrir el archivo\n");
		return -1;
	}
	
	hash_t* hash= hash_crear(NULL);
	
	char* str = NULL;
	size_t n;
	ssize_t largo_linea = getline(&str, &n, archivo);
	
	while(largo_linea!=-1){
		
		char** arreglo = split(str,',');
		int contador = 0;
		char* usuario = arreglo[0];
		for(int i=1; arreglo[i] != NULL; i++){
			contador += 1;
		}
		int* tweets;
		if (hash_pertenece(hash,usuario)){//osea ya está de antes
			tweets = hash_obtener(hash,usuario);
			*(int*)tweets = *(int*)tweets + contador;
		}
		else{
			tweets = malloc(sizeof(int*));
			*(int*)tweets = 0;
			*(int*)tweets = *(int*)tweets + contador;
		}
		hash_guardar(hash, usuario, tweets);
		free(str);
		free_strv(arreglo);
		str = NULL;
		largo_linea = getline(&str, &n, archivo);
	}
  
	hash_iter_t* iter = hash_iter_crear(hash);
	void** arreglo = malloc(sizeof(void*) * hash_cantidad(hash));
	for(int i=0; !hash_iter_al_final(iter); i++){
		char* clave = (char*)hash_iter_ver_actual(iter);
		void* dato = hash_obtener(hash, clave);
		nodo_h_t* nodo = nodo_h_crear(clave, dato);
		arreglo[i] = nodo;
		hash_iter_avanzar(iter);
	}
	
	ordenar_nombres(arreglo, (int)hash_cantidad(hash), comparar_menor_mayor);
	
	if (str){
		free (str);
	}
	for (int i = 0; i != hash_cantidad(hash); i++){
		free(arreglo[i]);
	}
	free(arreglo);
	hash_iter_destruir(iter);
	hash_destruir(hash);
	fclose(archivo);
	return 0;
}
  
void ordenar_nombres(void** arreglo, int largo, cmp_func_t cmp){
	heap_t* heap = heap_crear_arr((void*)arreglo, largo, cmp);
	// Encolo los elementos
	int cantidad = 0;
	// Imprimo
	while (!heap_esta_vacio(heap)){
		void* nodo = heap_desencolar(heap);
		if (*(int*)nodo_dato((nodo_h_t*)nodo) != cantidad){
			printf("\n");
			printf("%i: %s", *(int*)nodo_dato((nodo_h_t*)nodo), (char*)nodo_clave((nodo_h_t*)nodo));
			cantidad = *(int*)nodo_dato((nodo_h_t*)nodo);
		}
		else{
			printf(", %s", (char*)nodo_clave(nodo));
		}
		free(nodo_dato((nodo_h_t*)nodo));
	}
	printf("\n");
	heap_destruir(heap, NULL);
}

int comparar_menor_mayor(const void *a, const void *b){
	return comparar_nodos_inverso((nodo_h_t*)a,(nodo_h_t*)b);
}
//Para que compare de menor a mayor, no se si esto cree un heap de menores ajajaj
int comparar_nodos_inverso(nodo_h_t* nodo_1, nodo_h_t* nodo_2){
	if (*(int*)nodo_1->dato > *(int*)nodo_2->dato)return -1;
	if(*(int*)nodo_1->dato == *(int*)nodo_2->dato)return 0;
	else{
		return 1;
		} //ver de dar vuelta
}

int main(int argc, char* argv[]){
	procesar_usuarios(argv[1]);
	return 0;
}
