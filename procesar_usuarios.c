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


/* ***
 *                    		PROCESAR USUARIOS
 * ***/

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


int procesar_usuarios(char* nom_archivo){
	
	FILE *archivo;
	archivo = fopen( nom_archivo,"r");
	if (!archivo){
		printf("Error al abrir el archivo\n");
		return -1;
	}
	
	hash_t* hash= hash_crear(NULL);
	
	char* str = NULL;
	size_t n;
	ssize_t largo_linea= getline(&str, &n, archivo);
	
	while(largo_linea!=-1){
		
		char** arreglo= split(str,',');
		int contador=0;
		char* usuario= arreglo[0];
		for(int i=1;arreglo[i]!=NULL;i++){
			contador=contador+1;
		}
		if(hash_pertenece(hash,usuario)){//osea ya está de antes
			void* cont = hash_obtener(hash,usuario);
			contador+= (int) cont;
		}
		hash_guardar(hash,usuario,(void*)contador);
		free(str);
		free_strv(arreglo);
		str = NULL;
		largo_linea=getline(&str,&n,archivo);
	}
	hash_iter_t* iter= hash_iter_crear(hash);
	while(! hash_iter_al_final(iter)){
		char* usuario= (char*) hash_iter_ver_actual(iter);
		int cant = (int)hash_obtener(hash,(void*)usuario);
		printf("%s: %d\n",usuario,(int)cant);
		hash_iter_avanzar(iter);
	}
	if (str){
		free(str);
	}
	hash_iter_destruir(iter);
	hash_destruir(hash);
	fclose(archivo);
	return 0;
		
}


	
int main(){
	procesar_usuarios("tweets.txt");
	return 0;
}