#include <stdio.h>
#include <unistd.h>

#define memBlock_size sizeof(struct memBlock)

/*
* Definicion de la estructura del bloque de memoria
*/

struct memBlock {
	int size;
	int allocated;
	int offset;
	struct memBlock * siguiente;
	struct memBlock * anterior;
};
typedef struct memBlock memBlock;
typedef memBlock * memBlock_ptr;

static struct memBlock * head = NULL;
static struct memBlock * ultimo = NULL;
static struct memBlock * puntero =NULL;
static int indice =0;

/*
* Usamos politica first fist para buscar el bloque de memoria
* siguiente disponible, empezando por el primer bloque (head)
* del heap
*/
struct memBlock * first_fit(size_t size){
	struct memBlock * actual = head;
	while(actual)
	{
		if(actual->allocated==0 && actual->size>=size)
			return actual;
		actual = actual->siguiente;
	}
	return NULL;
}

/*
* Funcion que divide el bloque de memoria 
*/
void split(struct memBlock * header, size_t size)
{
	int newSize = (header->size-size)-memBlock_size;
	char * pInicial = (char *) header;
	int pOffset = memBlock_size/sizeof(char);
	struct memBlock * newHeader = (struct memBlock * ) (pInicial + pOffset + size);
	newHeader->size = newSize;
	newHeader->allocated = 0;
	newHeader->siguiente = header->siguiente;
	newHeader->anterior = header;
	header->siguiente = newHeader;
	header->size = size;
	return;
}

void * malloc ( size_t size)
{
	size_t total = memBlock_size + size;
	void * block;
	struct memBlock * header;
	if(size==0)
	{
		return NULL;
	}
	header = first_fit(size);
	if(header)
	{
		header->allocated = 1;
		if(header->size-size>memBlock_size+4)
		{
			split(header,size);
		
		}
		return (void *) (header+1);
	}
	block = sbrk(total);
	if (block == (void*) -1) 
	{
		printf("%s\n","Fallo sbrk()" );
		return NULL;
	}
	
	header = (struct memBlock *)block;
	header->size = size;
	header->allocated = 1;
	header->siguiente = NULL;
	if (head==NULL)
	{
		head = header;
	}
	if (ultimo!=NULL)
	{
		struct memBlock * temp;
		ultimo->siguiente = header;
		header->anterior = ultimo;
		header->siguiente = NULL;
	}
	
	ultimo = header;

	return (void*)(header + 1);
}

void free (void * block)
{
	struct memBlock * bloqueActual, * bloqueAnterior, * bloqueSiguiente;

	if (!block)
		return;

	bloqueActual = (struct memBlock *)block - 1;
	bloqueAnterior = bloqueActual->anterior;
	bloqueSiguiente = bloqueActual->siguiente;
	bloqueActual->allocated = 0;

	if(!(bloqueSiguiente==NULL) && (bloqueSiguiente->allocated==0))
	{
		bloqueActual->size = bloqueActual->size + memBlock_size + bloqueSiguiente->size; 
		bloqueActual->siguiente = bloqueSiguiente->siguiente;
		bloqueSiguiente->siguiente->anterior = bloqueActual;
	}

	if(!(bloqueAnterior==NULL) && (bloqueAnterior->allocated==0))
	{
		bloqueAnterior->size = bloqueAnterior->size + memBlock_size + bloqueActual->size;
		bloqueAnterior->siguiente = bloqueActual->siguiente;
		bloqueSiguiente->anterior = bloqueAnterior;
	}

	return;
}

void show()
{
	
	struct memBlock * puntero = head;
	printf("%s\n","--------------");
	int indice = 1;
	while(puntero!=ultimo)
	{
		printf("%s%d%s%d\n ","Bloque de memoria: ", indice," Tamaño: ", puntero->size );
		puntero = puntero->siguiente;
		indice++;
	}
	printf("%s\n","--------------" );
	
	return;


}
int main(int argc, char const *argv[])
{
	void * ptr1 = malloc(32);
	void * ptr2 = malloc(64);
	void * ptr3 = malloc(128);
	void * ptr4 = malloc(256);
	void * ptr5 = malloc(16);
	void * ptr6 = malloc(512);
	void * ptr7 = malloc(2048);
	
	show();

	free(ptr1);
	free(ptr2);
	
	show();
	
	void * ptr8 = malloc(50);
	show();
	free(ptr4);
	free(ptr6);
	show();

	void * ptr9 = malloc(400);
	show();	
}