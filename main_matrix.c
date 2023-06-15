/* PROGETTO VECCHIO NON FINITO CON STRUTTURA DI MATRICI */


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define ASCIItoDEC 48
#define NON_BAGNATA 0
#define BAGNATA 1
#define PALAZZO 219-ASCIItoDEC

#define LETTERA_N 78
#define LETTERA_E 69
#define LETTERA_O 79
#define LETTERA_S 83

typedef enum{S, E, O, N} Direzioni;


typedef struct 
{
    Lista *prev;
    Cella *cella_bivio;
    Direzioni m;
    Lista *next; 
} Lista;



typedef struct Cella
{
    Cella *parent;              /* memorizza la cella parent */

    char key;                   /* calore della cella */
    int casella_bagnata;        /* indica con 1 se è bagnata invece con 0 se è asciutta */
    /* posizione della cella nella matrice */
    int i;
    int j;
    
    Cella *lista_bivi[sizeof(Direzioni)];          /* vettore di celle bivi del parent */
} Cella;

typedef struct
{
    int rows, columns;
    Cella **matrix;
} Campo;

Campo *read_from_file(FILE *f, Campo *c);
Campo *create_matrix(Campo *c);
int controllaBagnata(Campo *c, int i, int j);

int CellaSuccessiva(Campo *c, int row, int column, Cella *parent);

Lista *pila_pop(Lista *p);
Lista *pila_push(Lista *p, Cella *c);

int main( int argc, char *argv[] ) 
{
    Campo *c;

    FILE *filein = stdin;

    if (argc != 2) {
        fprintf(stderr, "Invocare il programma con: %s test{N}.in\n", argv[0]);
        return EXIT_FAILURE;
    }
    filein = fopen(argv[1], "r");
    if (filein == NULL) {
        fprintf(stderr, "Can not open %s\n", argv[1]); 
        return EXIT_FAILURE;
    }

    c = (Campo*) malloc(sizeof(Campo));
    c = read_from_file(filein, c);



    return EXIT_SUCCESS;
}

Campo *read_from_file(FILE *f, Campo *c)
{
    char tmp_char;
    int i, j, m;
    int rows, columns;
    if (2 != fscanf(f, "%d %d", &rows, &columns)) {
        fprintf(stderr, "ERRORE durante la lettura (righe e colonne)\n");
        abort();
    }
    c->columns = columns;
    c->rows = rows;
    c = create_matrix(c);
    printf("%d %d\n", c->rows, c->columns);
    for(i = 0; i < c->rows; i++)
    {
        for (j = 0; j < c->columns; j++)
        {
            fscanf(f, "%c", &tmp_char);
            if(tmp_char >= (0+ASCIItoDEC) && tmp_char <= (9+ASCIItoDEC))
            {
                c->matrix[i][j].i=i;
                c->matrix[i][j].j=j;
                c->matrix[i][j].parent=NULL;
                c->matrix[i][j].key = tmp_char;
                c->matrix[i][j].casella_bagnata = controllaBagnata(c, i, j);    /* uso questa funzione per identificare se quella cella è bagnata (1) o no (0), se la cella è un palazzo =   */
                for (m = 0; m < sizeof(Direzioni); m++)
                {
                    c->matrix[i][j].lista_bivi[m] = NULL;
                }
                printf("%c", (c->matrix[i][j].casella_bagnata+ASCIItoDEC));
            }
            else {j--;}
        }
        printf("\n");
    }

    return c;
}

Campo *create_matrix(Campo *c)
{
    int i, j;
    c->matrix = (Cella**) malloc(c->rows * sizeof(Cella*));
    for(i = 0; i < c->rows; i++)
    {
        c->matrix[i] = (Cella*) malloc(c->columns * sizeof(Cella));
        for (j = 0; j < c->columns; j++)
        {
            c->matrix[i][j].key = j+ASCIItoDEC;
        }
    }
    return c;
}

int controllaBagnata(Campo *c, int row, int column)
{
    int i, caselle_bagnate;
    if(c->matrix[row][column].key != (0+ASCIItoDEC)) return PALAZZO;
    for (i = 0; i < column; i++)
    {
        if(c->matrix[row][i].key != (0+ASCIItoDEC))
        {
            caselle_bagnate = (c->matrix[row][i].key - ASCIItoDEC) + i;
            if(column<=caselle_bagnate)
            {
                return NON_BAGNATA;                /* la casella è bagnata può interrompere il ciclo */
            }
        }
    }
    return BAGNATA;
}

int controllaCellaLibera(int key)
{
    if(key == (0+ASCIItoDEC)) return 0; 
    return 1;
}

/* 
    restituisce il numero di bivi trovati
*/
int CellaSuccessiva(Campo *c, int row, int column, Cella *parent) 
{
    const int min_rows=0, min_columns=0;
    int numero_bivi=0;
    int i;

    if(row-1>=min_rows) /* può andare BOT = S */
    {
        if(controllaCellaLibera(c->matrix[row-1][column].key)==0) 
        {
            c->matrix[row][column].lista_bivi[S]=&(c->matrix[row-1][column]);
            numero_bivi++;
        }
    }

    if(column+1<=(c->columns-1)) /* può andare RIGHT = E */
    {
        if(controllaCellaLibera(c->matrix[row][column+1].key)==0) 
        { 
            c->matrix[row][column].lista_bivi[E]=&(c->matrix[row][column+1]);
            numero_bivi++;
        }
    }


    if(column-1>=min_columns) /* può andare LEFT = O */
    {
        if(controllaCellaLibera(c->matrix[row][column-1].key)==0) 
        { 
            c->matrix[row][column].lista_bivi[O]=&(c->matrix[row][column-1]);
            numero_bivi++;
        }
    }

    if(row+1<=(c->rows-1)) /* può andare TOP = N */
    {
        if(controllaCellaLibera(c->matrix[row+1][column].key)==0) 
        { 
            c->matrix[row][column].lista_bivi[N]=&(c->matrix[row+1][column]);
            numero_bivi++;
        }
    }

    for (i = 0; i < numero_bivi; i++)   /* bisogna controllare anche che quella successiva non sia quella parent */
    {
        if(c->matrix[row][column].lista_bivi[i] == parent) 
        {
            numero_bivi--;
            c->matrix[row][column].lista_bivi[i] = NULL;
            return numero_bivi;
        }
    }
    

    return numero_bivi;
}

void BFS(Cella *cella_attuale, Cella *parent, Lista *pila_lista_bivi, Lista *pila_direzioni)        /* questo bivio passato deve essere una pila */
{
    Cella *tmp;
    int numero_bivi;
    int i, j, m, k;

    numero_bivi = CellaSuccessiva(cella_attuale,cella_attuale->i,cella_attuale->j, parent);    

    cella_attuale->parent = &(parent);          /* da ricontrollare */

    if(numero_bivi>=2) /* bisogna controllare ogni direzione */
    {
        for (m = 0; m < sizeof(Direzioni); m++)
        {   
            if(cella_attuale->lista_bivi[m] != NULL)
            {
                pila_lista_bivi = pila_push(pila_lista_bivi , cella_attuale);
                BFS(cella_attuale->lista_bivi[m], cella_attuale, pila_lista_bivi->cella_bivio, m);
                m=-1;                       /* ricomincio il ciclo pr fare in modo che ricontrolli tutte le posizioni */
            }
        }
    }
    
    if(numero_bivi==1)  /* quando c'è solo un bivio bisogna tirare dritto e dare il parent di quello che aveva il bivio */
    {
        for (m = 0; m < sizeof(Direzioni); m++)
        {   
            if(cella_attuale->lista_bivi[m] != NULL)
            {
                pila_lista_bivi->m = m;     /* questo é quello che devi aggiungere */
                BFS(cella_attuale->lista_bivi[m], cella_attuale, pila_lista_bivi, m);
            }
        }
    }

    if(numero_bivi==0)  /* torna immediatamente al nodo dove c'erano più di 2 bivi cancellando la direzione appena controllata */
    {
        pila_lista_bivi->cella_bivio->lista_bivi[m] = NULL;
        if(pila_lista_bivi->cella_bivio != NULL)
        {
            tmp = pila_lista_bivi->cella_bivio;
            pila_lista_bivi = pila_pop(pila_lista_bivi); /* da pensare bene */
            BFS(tmp, tmp->parent, pila_lista_bivi->cella_bivio, m);
        }
    }
}

void CercaCamminiMinimi(Campo *c)
{
    char sequenza_cammini[100];

    /* ricordarsi che il primo elemento é tutto 0 e null per i puntatori */
}

Lista *pila_pop(Lista *p)
{
    Cella *c = p->cella_bivio;
    p = p->next;
    free(p->prev);
    return p;
}

Lista *pila_push(Lista *p, Cella *c)
{
    Lista *new_pila = (Lista *)malloc(sizeof(Lista));
    new_pila->cella_bivio = c;
    new_pila->next = p;
    p->prev = new_pila;
    p = new_pila;
    return p;
}

Lista *coda_dequeue()
{

}

Lista *coda_enqueue()
{
    
}