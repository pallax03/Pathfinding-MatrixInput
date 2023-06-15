/* COMANDI USATI IN AMBIENTE LINUX */
/* gcc -std=c90 -Wall -Wpedantic main.c -o main */
/* valgrind ./main file1.in */

/*
    IL PROGETTO è SEMPLIFICATO!!!! (NON VIENE PRESO IL CAMMINO MINIMO OTTIMALE CONSIDERATA LA PIOGGIA)!
    Realizzazione del progetto 2022/2023 - Algoritmi e Strutture Dati Classe A
    Il progetto consiste nel calcolare il numero di cammino minimi 
    dalla mappa di una città, rappresentata da una matrice n x m (rows, columns).

    Riassunto del programma:
    leggo in input il file specificato, e creo una matrice di char (rows x columns),
    utilizzo poi la matrice per creare un grafo di n nodi = (rows * columns):
    l'elemento della matrice[0][0] = nodo 0, matrice[0][1] = nodo 1, matrice[r][c] = nodo (r*columns)+c,
    ogni nodo rappresenta una cella della matrice, ma solamente le celle contenenti '0'
    ovvero i marciapiedi sono collegati fra loro nelle direzioni:
    {N = nord, E = est, O = ovest, S = sud} 
    esempio: 

    matrice:
            01
            00

    hanno i seguenti archi (src, dst):
    nodo (0): (0, 2) -> NULL
    nodo (1): NULL (é un palazzo)
    nodo (2): (2, 0) -> (2, 3) -> NULL
    nodo (3): (3, 2) -> NULL

    la funzione BFS ( non è modificata ed è quella fornita dal docente Marzolla ), 
    calcola la distanza minima dal nodo di partenza al nodo di destinazione.

    ho strutturato il programma nel modo seguente:
    - DICHIARAZIONI:
        - struttura del grafo e dichiarazione delle funzioni che lo gestiscono, 
        - dichiarazione delle funzioni che leggono il file, gestistocono la matrice e il grafo per 
        memorizzare la mappa della città. (sono le funzioni che ho creato da 0).
        -funzioni usate per calcolare il BFS, liste e gestione di esse.
    - DEFINE DELLE COSTANTI USATE:
        - costanti delle direzioni e variabili dei loro valori in ASCII, per poter essere facilmente convertibili da int a char.
        - variabili e costanti usate per memorizzare i nodi bagnati.

    - MAIN, e successivamente tutte le funzioni dichiarate sopra in ordine di dichiarazione.
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* struct Graph del compito e funzioni delle liste fornite dal Docente: Moreno Marzolla, non sono state modificate*/
/* struttura arco */
typedef struct Edge {
    int src;            /* nodo sorgente        */
    int dst;            /* nodo destinazione    */
    struct Edge *next;
} Edge;

/* struttura grafo */
typedef struct {
    int n;              /* numero di nodi               */
    int m;              /* numero di archi              */
    Edge **edges;       /* array di liste di adiacenza  */
    int *in_deg;        /* grado entrante dei nodi      */
    int *out_deg;       /* grado uscente dei nodi       */
} Graph;

/* Crea un nuovo grafo con `n` nodi. Il tipo di grafo é non orientato */
Graph *graph_create(int n);

/* Restituisce il numero di nodi del grafo */
int graph_n_nodes(const Graph *g);

/* Aggiunge un nuovo arco (src, dst). Si puo' assumere che l'arco non esista già. */
void graph_add_edge(Graph *g, int src, int dst);

/* Restituisce un puntatore al primo arco della lista di adiacenza
   associata al nodo `v` (`NULL` se la lista è vuota) */
Edge *graph_adj(const Graph *g, int v);

/* Libera tutta la memoria occupata dal grafo e dalle liste di adiacenza */
void graph_destroy(Graph *g);

/*
    legge dal file passato come argomento, la matrice,
    creando una matrice di char, utile per scorrere gli elementi,
    (La matrice è stata riempita prendendo solo i caratteri compresi tra 0 e 9),
    per creare successivamente gli archi ai nodi specificati (quelli contenenti '0')
    al termine della creazione degli archi dealloca la matrice.
    Restituisce il puntatore del grafo.
*/
Graph *read_from_file(FILE *f);

/*
    Dato il grafo, la matrice, le righe totali e le colonne totali.
    scorre ogni cella della matrice, crea gli archi nelle direzioni {N, S, E, O},
    Ma solamente per i nodi corrispondenti ai marciapiedi (primo if nei for controlla
    se la cella è un marciapiede check_sidewalks_from_matrix()).
    Prima di controllare una direzione, controllo se gli indici possono spostarsi
    senza causare un evento "Out of bounds".
    Se la Cella adiacente (secondo le direzioni fornite) è un marciapiede,
    crea l'arco dalla cella[i][j] = nodo get_node_index_from_matrix_indexs(), alla cella specificata.
    Restituisce il puntatore del grafo.  
*/
Graph *graph_create_relations_from_matrix(Graph *g, char **matrix, int rows, int columns);

/*
    Controlla se la cella della matrice corrisponde al valore del marciapiede = '0'.
    Se non è un marciapiede restituisce -1.
    Restituisce il numero del nodo corrispondente alla cella[r][c] (get_node_index_from_matrix_indexs()), 
    (rows corrisponde al numero di righe totali).
*/
int check_sidewalks_from_matrix(char **matrix, int r, int c, int columns);

/*
    Converte gli indici usati nella matrice nel numero del nodo.
    esempio il vettore wet_nodes ogni cella di 'i', i corrisponde al nodo (i nodi vanno da 0 a (rows*columns)-1).
    restituisce l'intero dato dalla formula: (r*columns)+c.
*/
int get_node_index_from_matrix_indexs(int i, int j, int columns);

/*
    Controlla se il marciapiede é bagnato,
    oppure no controllando se ha affianco dei palazzi.
    Restituisce 1 se è BAGNATO, 0 se NON_BAGNATO, mentre se il nodo è un palazzo
    interrompe la funzione restituendo un carattere speciale 'PALAZZO'.
    I valori sono memorizzati in un vettore di n elementi che corrispondono al
    numero dei nodi del grafo, il vettore é dichiarato globalmente per essere usato in tutte le funzioni
    senza essere passato. (wet_nodes).
*/
int check_wet_sidewalk(char **matrix, int r, int c);

/* Libera tutta la memoria occupata dalla matrice usata per leggere la mappa della città */
void matrix_destroy(char **matrix, int rows);

/* Visita il grafo g usando l'algoritmo di visita in ampiezza (BFS)
   partendo dal nodo sorgente s.
   Restituisce il numero di nodi visitati (incluso s).
   
    s = nodo sorgente
    d = al termine dell'esecuzione, d[i] e' la distanza del nodo `i` dalla sorgente `s`, dove la distanza
        e' intesa come minimo numero di archi che collegano la sorgente al nodo `i`. Questo array 
        deve essere allocato dal chiamante e contenere `n` elementi (uno per ogni nodo del grafo) 
    
    p = al termine dell'esecuzione, `p[i]` è il predecessore del nodo `i` lungo il cammino minimo
        da `s` a `i`. Nel caso in cui `i` non sia raggiungibile dalla sorgente, `p[i] = -1`.
        Anche questo array deve essere allocato dal chiamante e avere lunghezza `n`.
*/
int BFS(const Graph *g, int s, int *d, int *p);

/*  È una funzione ricorsiva che scorre tutto il vettore p riempito dalla BFS
    p contiene i nodi in ordine da quello di destinazione a quello di partenza, 
    che corrispondono ai cammini minimi usati per attraversare il grafo, 
    ricorsivamente conta quanti nodi impiega per arrivare a quello di partenza.    
    Restituisce quindi il numero totale dei cammini minimi impiegati.
*/
int count_path(int s, int d, const int *p, int counter);

/*
    utilizza lo stesso modo di scorrimento dei cammini minimi come la count_path(),
    ma non è ricorsiva.
    Conta i nodi che sono bagnati controllando se nel vettore wet_nodes[] é presente BAGNATO,
    esso è un vettore parallelo ai nodi del grafo, cioè l'indice 0 corrisponde al nodo 0 e cosi a seguire
    fino a (graph_n_nodes() - 1).
    La matrice è stata creata e riempita al momento di lettura del file di input.
*/ 
int count_wet_nodes(int s, int d, int* p);

/*  Il funzionamento é lo stesso della count_path(), però stampa la direzione, in lettere,
    dei cammini minimi, realizzandola ricorsivamente così anche se é in ordine decrescente,
    gli elementi vengono stampati al "ritorno della funzione" quindi il vettore si "ribalta".
    utilizzo la funzione stampa_direzione(), per sapere la direzione dal nodo1 al nodo2.
*/
void print_path(int s, int d, const int *p);

/*
    Forniti due nodi stampa la lettera della direzione dal nodo di partenza: node,
    al nodo successivo: next_node.
    la differenza fra il nodo al nodo successivo corrisponde alla lettera:

    N = (columns*-1) -> poichè spostarsi a Nord, nella matrice corrisponde 
                a spostarsi delle colonne totali * -1 (-1 poichè [r-1][c])

    S =  columns     -> poichè spostarsi a Sud, nella matrice corrisponde
                a spostarsi delle colonne totali (poichè [r+1][c])

    E =     1     -> poichè spostarsi a Est, nella matrice corrisponde 
                a spostarsi di 1 (poichè [r][c+1])

    O =    -1     -> poichè spostarsi a Ovest, nella matrice corrisponde
                a spostarsi di -1 (poichè [r][c-1])
*/
void stampa_direzione(int node, int next_node);


/* constanti e strutture dati utili per calcolare la BFS del grafo */
const int NODE_UNDEF = -1;
typedef enum { WHITE, GREY, BLACK } Color; /* colori dei nodi */

/* struct List e funzioni utili per calcolare la BFS sono quelle fornite dal Docente: Marzolla, e non sono state modificate */
typedef int ListInfo;

typedef struct ListNode {
    ListInfo val;
    struct ListNode *succ, *pred;
} ListNode;

typedef struct {
    int length;
    ListNode *sentinel;
} List;

/* Crea una lista, inizialmente vuota. */
List *list_create( void );

/* 
    Crea un nuovo nodo contenente il valore v. I puntatori
    al nodo successivo e precedente puntano entrambi al nodo appena
    creato.
*/
ListNode *list_new_node(int v);

/* Restituisce il puntatore al primo nodo di `L`; se `L` è vuota,
   restituisce un puntatore alla sentinella */
ListNode *list_first(const List *L);

/* Restituisce l'indirizzo di memoria della sentinella di L */
ListNode *list_end(const List *L);

/* Concatena due nodi pred e succ: succ diventa il successore di pred. */
static void list_join(ListNode *pred, ListNode *succ);

/* Restituisce (un valore diverso da zero) se la lista è vuota, 0 altrimenti */
int list_is_empty(const List *L);

/* Crea un nuovo nodo contenente k, e lo inserisce immediatamente dopo il nodo n di L */
static void list_insert_after(List *L, ListNode *n, ListInfo k);

/* Inserisce un nuovo nodo contenente k alla fine della lista */
void list_add_last(List *L, ListInfo k);

/* Restituisce il successore del nodo `n`; se `n` è l'ultimo nodo
   della lista, restituisce un puntatore alla sentinella. */
ListNode *list_succ(const ListNode *n);

/* Restituisce il predecessore del nodo `n`; se `n` è il primo nodo
   della lista, restituisce un puntatore alla sentinella. */
ListNode *list_pred(const ListNode *n);

/* Rimuove il nodo `n` dalla lista, liberando anche la memoria occupata da `n`. */
void list_remove(List *L, ListNode *n);

/* Rimuove il primo nodo della lista e ne restituisce il valore */
ListInfo list_remove_first(List *L);

/* Svuota la lista L; Libera la memoria occupata da tutti i nodi di `L`. */
void list_clear(List *L);

/* 
    Distrugge la lista `L` e libera tutta la memoria occupata dai nodi
    di `L` e dalla struttura puntata da `L`.
*/
void list_destroy(List *L);

/* 
    constanti usate per identificare le direzioni usate, 
    e per convertire facilmente i valori da intero a char.
    ( Tabella ASCII 0 (intero) = 48 (char) ).
*/
#define ASCIItoDEC 48   

/* servono per capire la direzione effettuata per passare dal nodo1 al nodo2. */
int N;      /* (columns*-1) */ 
int S;      /*  columns     */
int E = 1;  /*  1        */
int O =-1;  /* -1        */

/* valori interi corrispondenti alla lettera in ASCII */
#define character_N 78
#define character_S 83
#define character_E 69
#define character_O 79

/*
    costanti e variabili usate per identificare i nodi BAGNATI
*/
#define PALAZZO 219-ASCIItoDEC                  /* Memorizzo un carattere speciale, ma é superfluo */
typedef enum{NON_BAGNATA, BAGNATA} stato_cella; /* utilizzo enum per creare delle costanti bagnata (1) e non bagnata (0) */
char *wet_nodes;  /* vettore che indica quali nodi sono Bagnati dalla pioggia, l'indice corrisponde al nodo (nodo 0 = wet_nodes[0]) */

/*
    Il programma prende come argomenti la riga di comando effettuata quando si esegue:
    - il nome del file eseguibile (argv[0]) e il nome del file di input specificato (argv[1])

    Inizializza le variabili usate:
    grafo, vettori che contengono i nodi usati nel cammino minimo calcolato, e variabili di appoggio da passare alle funzioni:
    nodo di partenza = src = 0.
    n = numero dei nodi totali del grafo.
    dst = nodo di destinazione = rows*columns.
    counter = memorizza il numero di nodi usati per il cammino minimo.
*/
int main( int argc, char *argv[] ) 
{
    Graph *g;
    int *p, *d;
    int src = 0, n, dst, counter=0;

    /* Prendo il nome del file passato dall'argomento e lo apro, effettuando il controllo se il file é valido e se riesce ad aprirlo. */
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

    /* 
        Riempio il grafo con il file letto, e successivamente chiudo il file di input (deallocandosi).
    */
    g = read_from_file(filein);
    fclose(filein);
    /*
        Inizializzo le varibili per eseguire la BFS:
        n = numero di nodi del grafo, per risparmiare la variabile la uso anche per memorizzare il valore restituito dalla BFS.
        p = è un vettore di interni di dimensione nodi totali del grafo, `p[d]` è il predecessore del nodo `d` lungo il cammino minimo da `s` a `d`
        d = d[i] e' la distanza del nodo `i` dalla sorgente `s`, dove la distanza e' intesa come minimo numero di archi che collegano la sorgente al nodo `i`.
        dst = l'indice del l'ultimo nodo (il nodo di destinazione quello in basso a destra).
    */
    n = graph_n_nodes(g);
    p = (int*)malloc( n * sizeof(*p) );
    d = (int*)malloc( n * sizeof(*d) );
    dst = n-1;
    n = BFS(g, src, d, p);
    printf("%d\n", n);
    /*
        Stampando poi i parametri richiesti: (se il vettore che nell'elemento di destinazione ha come predecessore dal nodo di destinazione
        nei cammini minimi un nodo < 0, significa che non esiste un percorso minimo,
        e il programma stampa -1 -1 come richiesto da consegna).
        
        count_path() = numero totale dei cammini minimi impiegati.
        count_wet_nodes() = numero totale dei cammini minimi impiegati, che sono bagnati.
        print_path() = stampo tutta la sequenza di lettere dei cammini minimi ottenuti dalla BFS().
    */
    if (p[dst] < 0) printf("%d %d\n", -1, -1);
    else
    {
        printf("%d %d\n", count_path(src, dst, p, counter), count_wet_nodes(src, dst, p));
        print_path(src, dst, p);
    }

    /*
        prima di terminare il programma libero tutti i blocchi di memoria allocati per la BFS,
        il vettore dei nodi bagnati, e infine dealloco il grafo. 
    */
    free(p);
    free(d);
    free(wet_nodes);
    graph_destroy(g);

    return EXIT_SUCCESS;
}

/* FUNZIONI che gestiscono la fase di input del programma. */
Graph *read_from_file(FILE *f)
{
    Graph *g;
    char tmp_char;
    int i, j;
    int rows=0, columns = 0;
    char **matrix;          /* matrice dove memorizzo tutta la mappa della città, verrà deallocata poi alla fine di questa funzione. */

    /* Leggo il numero delle righe e delle colonne della matrice, se ha qualche problema termino il programma. */
    if (2 != fscanf(f, "%d %d", &rows, &columns)) {
        fprintf(stderr, "ERRORE durante la lettura (righe e colonne)\n");
        abort();
    }

    /*
        Queste variabili servono per sapere se da un nodo all'altro è andato a nord, sud, ovest o est.
        memorizzo il numero delle righe totali poichè avendo una matrice (10*10), se un cammino minimo é dal nodo 0 al nodo 10,
        la direzione è sud cioé il numero delle righe, la direzione nord é l'opposto.
        Tutto questo poichè il numero dei nodi va da 0 a (row*columns)-1. quindi devo convertire l'indice della matrice, in un indice per un vettore.
    */
    N = (columns * -1);
    S = columns;

    g = graph_create(rows*columns);

    /*
        Legge tutta la matrice contenuta nel file (non considerando i valori '\n', ' ', o simili; Ma solamente i numeri da 0 a 9).
        una volta che memorizza il valore nella matrice, controlla se é bagnata oppure no, memorizzandolo nel vettore wet_nodes.
    */
    wet_nodes = (char*)malloc(sizeof(char)*graph_n_nodes(g));
    matrix=(char**) malloc(rows * sizeof(char*));
    for (i = 0; i < rows; i++)
    {
        matrix[i] = (char*) malloc(columns * sizeof(char));
        for (j = 0; j < columns; j++)
        {
            fscanf(f, "%c", &tmp_char);
            if(tmp_char >= (0+ASCIItoDEC) && tmp_char <= (9+ASCIItoDEC))
            {   
                matrix[i][j] = tmp_char;
                wet_nodes[get_node_index_from_matrix_indexs(i, j, columns)] = check_wet_sidewalk(matrix, i, j);
            }
            else {j--;}
        }
    }

    g = graph_create_relations_from_matrix(g, matrix, rows, columns);

    matrix_destroy(matrix, rows);

    return g;
}

int get_node_index_from_matrix_indexs(int i, int j, int columns)
{
    return (i*columns)+j;
}

int check_wet_sidewalk(char **matrix, int r, int c)
{
    int i, caselle_bagnate;
    if(matrix[r][c] != (0+ASCIItoDEC)) return PALAZZO;
    
    /*
        Controlla se nella riga della cella passata ci sono dei palazzi (itera 
        dalla cella iniziale nella stessa riga fino alla posizione della (cella - 1))
        per poi controllare se la cella è coperta
        contando la distanza delle celle per l'altezza del palazzo
        se la posizione della cella rientra nella distanza coperta dal palazzo,
        la casella non è bagnata altrimenti lo è
    */

    for (i = 0; i < c; i++)
    {
        if(matrix[r][i] != (0+ASCIItoDEC))
        {
            caselle_bagnate = (matrix[r][i] - ASCIItoDEC) + i;
            if(c<=caselle_bagnate)
                return NON_BAGNATA;
        }
    }
    return BAGNATA;
}

Graph *graph_create_relations_from_matrix(Graph *g, char **matrix, int rows, int columns)
{
    int i, j;
    int dst;        /* nodo di destinazione per creare l'arco con la cella di indice[i][j] */

    /*
        Scorre tutta la mappa della città.
        Controllo sempre che le direzioni non escano fuori dalla matrice,
        e per ogni direzione *possibile* controllo se l'adiacente nella direzione che controllo
        è un marciapiede, se lo è creo l'arco con src il nodo [i][j] e come dst il nodo di quella direzione.
    */
    for(i = 0; i < rows; i++)
    {
        for (j = 0; j < columns; j++)  
        {
            if(check_sidewalks_from_matrix(matrix, i, j, columns)!=-1)
            {
                /* S: [r+1][c] */
                if(i!=(rows-1))
                {
                    dst = check_sidewalks_from_matrix(matrix, i+1, j, columns);
                    if(dst!=-1)
                    {
                        graph_add_edge(g, get_node_index_from_matrix_indexs(i, j, columns), dst);
                    }
                }

                /* E: [r][c+1] */
                if(j!=(columns-1))
                {
                    dst = check_sidewalks_from_matrix(matrix, i, j+1, columns);
                    if(dst!=-1)
                    {
                        graph_add_edge(g, get_node_index_from_matrix_indexs(i, j, columns), dst);
                    }
                }

                /* O: [r][c-1] */
                if(j!=0)
                {
                    dst = check_sidewalks_from_matrix(matrix, i, j-1, columns);
                    if(dst!=-1)
                    {
                        graph_add_edge(g, get_node_index_from_matrix_indexs(i, j, columns), dst);
                    }
                }

                /* N: [r-1][c] */
                if(i!=0)
                {
                    dst = check_sidewalks_from_matrix(matrix, i-1, j, columns);
                    if(dst!=-1)
                    {
                        graph_add_edge(g, get_node_index_from_matrix_indexs(i, j, columns), dst);
                    }
                }
                
            }
        } 
    }
    return g;
}

int check_sidewalks_from_matrix(char **matrix, int r, int c, int columns)
{
    if(matrix[r][c] == (0+ASCIItoDEC))
    {
        return get_node_index_from_matrix_indexs(r, c, columns);
    }
    return -1;
}

void matrix_destroy(char **matrix, int rows) 
{
    int i;
    for (i = 0; i < rows; i++) 
    {
        free(matrix[i]);
    }
    free(matrix);
}

/* FUNZIONI CHE GESTISCONO I GRAFI */
Graph *graph_create(int n)
{
    int i;
    Graph *g = (Graph*)malloc(sizeof(*g));

    g->n = n;
    g->m = 0;
    g->edges = (Edge**)malloc(n * sizeof(Edge*));
    g->in_deg = (int*)malloc(n * sizeof(*(g->in_deg)));
    g->out_deg = (int*)malloc(n * sizeof(*(g->out_deg)));
    for (i=0; i<n; i++) {
        g->edges[i] = NULL;
        g->in_deg[i] = g->out_deg[i] = 0;
    }
    return g;
}

int graph_n_nodes(const Graph *g)
{
    return g->n;
}

void graph_add_edge(Graph *g, int src, int dst)
{
    Edge *new_edge = (Edge*)malloc(sizeof(Edge));

    new_edge->src = src;
    new_edge->dst = dst;
    
    new_edge->next = g->edges[src];
    g->edges[src] = new_edge;
    g->in_deg[dst]++;
    g->out_deg[src]++;
    g->m++;
}

Edge *graph_adj(const Graph *g, int v)
{
    return g->edges[v];
}

void graph_destroy(Graph *g)
{
    int i;

    for (i=0; i<g->n; i++) {
        Edge *edge = g->edges[i];
        while (edge != NULL) {
            Edge *next = edge->next;
            free(edge);
            edge = next;
        }
        g->edges[i] = NULL; /* e' superfluo */
    }
    free(g->edges);
    free(g->in_deg);
    free(g->out_deg);
    g->n = 0;
    g->edges = NULL;
    free(g);
}

/* FUNZIONI CHE GESTISCONO LA BFS */
int BFS(const Graph *g, int s, int *d, int *p)
{
    const int n = graph_n_nodes(g);
    List *q;
    int nvisited = 0;
    int i;
    Color *color = (Color*)malloc(n * sizeof(*color));

    for (i=0; i<n; i++) {
        d[i] = -1;
        p[i] = NODE_UNDEF;
        color[i] = WHITE;
    }

    d[s] = 0;
    color[s] = GREY;
    q = list_create();
    list_add_last(q, s);

    while ( ! list_is_empty(q) ) {
        const int u = list_remove_first(q);
        const Edge *edge;
        nvisited++;
        for (edge = graph_adj(g, u); edge != NULL; edge = edge->next) {
            const int v = edge->dst;
            assert(u == edge->src);
            if (color[v] == WHITE) { /* il nodo v non è ancora stato visitato */
                color[v] = GREY;
                d[v] = d[u] + 1;
                p[v] = u;
                list_add_last(q, v);
            }
        }
        color[u] = BLACK;
    }
    list_destroy(q);
    free(color);
    return nvisited;
}

int count_path(int s, int d, const int *p, int counter)
{
    if (s == d)
        return 1;

    counter = count_path(s, p[d], p, counter);
    return counter+1;
}

int count_wet_nodes(int s, int d, int* p)
{
    int count=0;
    /* if(wet_nodes[(sizeof(wet_nodes)/sizeof(wet_nodes[0]))-1]==BAGNATA) count++; */
    do
    {
        if(wet_nodes[d] == BAGNATA) count++;
        d = p[d]; 
    } while (s != d);
    if(wet_nodes[d] == BAGNATA) count++;
    return count;
}

void print_path(int s, int d, const int *p)
{
    if (s == d)
    {
        stampa_direzione(s, d);
    }
    else 
    {
        print_path(s, p[d], p);
        stampa_direzione(d, p[d]);
    }
}

void stampa_direzione(int node, int next_node)
{
    if((node - next_node) == N)
    {
        printf("%c", character_N);
    }
    else if((node - next_node) == S)
    {
        printf("%c", character_S);
    }
    else if((node - next_node) == E)
    {
        printf("%c", character_E);
    }
    else if((node - next_node) == O)
    {
        printf("%c", character_O);
    }
}

/* FUNZIONI CHE GESTISCONO LE LISTE */
List *list_create( void )
{
    List *L = (List*)malloc(sizeof(List));

    L->length = 0;
    L->sentinel = list_new_node(0); /* il valore contenuto nel nodo sentinella è irrilevante */
    return L;
}

ListNode *list_new_node(int v)
{
    ListNode *r = (ListNode *)malloc(sizeof(ListNode));
    r->val = v;
    r->succ = r->pred = r;
    return r;
}

ListNode *list_first(const List *L)
{
    return L->sentinel->succ;
}

ListNode *list_end(const List *L)
{
    return L->sentinel;
}

static void list_join(ListNode *pred, ListNode *succ)
{
    pred->succ = succ;
    succ->pred = pred;
}

int list_is_empty(const List *L)
{
    return (list_first(L) == list_end(L));
}

static void list_insert_after(List *L, ListNode *n, ListInfo k)
{
    ListNode *new_node, *succ_of_n;

    new_node = list_new_node(k);
    succ_of_n = list_succ(n);
    list_join(n, new_node);
    list_join(new_node, succ_of_n);
    L->length++;
}

void list_add_last(List *L, ListInfo k)
{
    list_insert_after(L, L->sentinel->pred, k);
}

ListNode *list_succ(const ListNode *n)
{
    return n->succ;
}

ListNode *list_pred(const ListNode *n)
{
    return n->pred;
}

void list_remove(List *L, ListNode *n)
{
    list_join(list_pred(n), list_succ(n));
    free(n);
    L->length--;
}

ListInfo list_remove_first(List *L)
{
    ListNode *first;
    ListInfo result;

    first = list_first(L);
    result = first->val;
    list_remove(L, first);
    return result;
}

void list_clear(List *L)
{
    ListNode *node;

    node = list_first(L);
    while (node != list_end(L)) {
        ListNode *succ = list_succ(node);
        free(node);
        node = succ;
    }
    L->length = 0;
    L->sentinel->pred = L->sentinel->succ = L->sentinel;
}

void list_destroy(List *L)
{
    list_clear(L);
    free(L->sentinel);
    free(L);
}