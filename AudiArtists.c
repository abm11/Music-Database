#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PLAYCOUNT 0
#define ARTISTID 1
#define NAMELEN 65
#define BUFSIZE 66

struct artist
{
  int artist_id;
  char name[NAMELEN];
  int count;
  int dupl;
  struct artist *next;
};

struct play
{
  int p_id;
  int artist_id;
  int playcount;
  struct play *next;
};

void print_play(struct play *p) {
  if( p != NULL )
    printf("user: %d artist: %d count: %d\n",
	   p->p_id,
	   p->artist_id,
	   p->playcount);
  else
    printf("NULL\n");
}

void print_plays(struct play *p)
{
  while(p!=NULL) {
    print_play(p);
    p = p->next;
  }
}

void print_artist(struct artist *a) {
  if( a != NULL )
    printf("%s (%d) [%d] \n",
	   a->name,
	   a->artist_id,
	   a->count);
  else
    printf("NULL\n");
}

void print_artists(struct artist *a)
{
  while(a!=NULL) {
    print_artist(a);
    a = a->next;
  }
}

struct play *create_play(int id, int artist_id, int playcount) {
  struct play *p = (struct play *)malloc(sizeof(struct play));
  if( p != NULL ) {
    p->p_id = id;
    p->artist_id = artist_id;
    p->playcount = playcount;
  }
  return p;
}

void free_play(struct play *p) {
  if( p != NULL )
    free(p);
}

struct artist *create_artist(int id, char *name, int count, int dupl) {
  struct artist *a = (struct artist *)malloc(sizeof(struct artist));
  if( a != NULL ) {
    a->id = id;
    strcpy(a->name, name);
    a->count = count;
    a->dupl = dupl;
  }

  return a;
}

struct play *parse_play(char *line)
{
  int u, a, c;
  if( sscanf(line, "%d %d %d\n", &u, &a, &c) == 3 )
    return create_play(u,a,c);
  else {
    return NULL;
  }
}

struct artist *parse_artist(char *line)
{
  int id;
  char name[NAMELEN];
  if( sscanf(line, "%d\t%65[^\t\n]\n", &id, name) == 2 )
    return create_artist(id,name,0,-1);
  else {
    return NULL;
  }
}

struct play *add_play(struct play *head, struct play *newp)
{
  if( newp != NULL ) {
    newp->next = head;
    head = newp;
  }
  return head;
}

struct artist *add_artist(struct artist *head, struct artist *newp)
{
  if( newp != NULL ) {
    newp->next = head;
    head = newp;
  }
  return head;
}

struct artist *read_artists(char *fname)
{
  char buffer[BUFSIZE];
  struct artist *head = NULL;
  FILE *fp = fopen(fname, "r");
  if( fp == NULL ) {
    return NULL;
  }
  while( fgets(buffer, BUFSIZE, fp) != NULL )
    head = add_artist( head, parse_artist(buffer) );

  fclose( fp );
  return head;

}

struct play *read_plays(char *file_name)
{
  char buffer[BUFSIZE];
  struct play *head = NULL;
  FILE *fp = fopen(file_name, "r");
  if( fp == NULL ) {
    printf("Cannot open file %s\n", file_name);
    return NULL;
  }

  // We do not need to check the return value of parse
  // because add_play safely ignores NULL pointers added to the list.
  while( fgets(buffer, BUFSIZE, fp) != NULL )
    head = add_play( head, parse_play(buffer) );

  fclose( fp );
  return head;
}

void free_artists(struct artist *lst) {
  struct artist *tmp = NULL;
  while( lst != NULL ) {
    tmp = lst;
    lst = lst->next;
    free(tmp);
  }
}

void free_plays(struct play *lst) {
  struct play *tmp = NULL;
  while( lst != NULL ) {
    tmp = lst;
    lst = lst->next;
    free(tmp);
  }
}

struct play *find_middle_play(struct play *x)
{
  struct play *slow = x;
  struct play *fast = x;
  while( fast->next != NULL && fast->next->next != NULL ) {
    slow = slow->next;
    fast = fast->next->next;
  }
  return slow;
}

struct artist *find_middle_artist(struct artist *x)
{
  struct artist *slow = x;
  struct artist *fast = x;
  while( fast->next != NULL && fast->next->next != NULL ) {
    slow = slow->next;
    fast = fast->next->next;
  }
  return slow;
}

struct play *merge_plays(struct play *x, struct play *y)
{

  struct play *tmp  = NULL;
  struct play *head = NULL;
  struct play *curr = NULL;

  if( x == NULL )
    head = y;

  else if( y == NULL )
    head = x;

  else {

    while( x != NULL && y != NULL ) {

      // Swap x and y if x is not largest.
      if( x->artist_id > y->artist_id ) {
	tmp = y;
	y   = x;
	x   = tmp;
      }

      if( head == NULL ) { // First element?
	head = x;
	curr = x;
      } else {
	curr->next = x;
	curr = curr->next;
      }
      x = x->next;
    }

    // Either x or y is empty.
    if( x != NULL )
      curr->next = x;

    else if( y != NULL )
      curr->next = y;

  }

  return head;
}

int cmp_artists(struct artist *x, struct artist *y, int criterion)
{
  if( criterion == PLAYCOUNT )
    return x->count >= y->count;
  else if( criterion == ARTISTID )
    return x->id <= y->id;
  else {
    printf("Unknown comparison criterion %d\n", criterion);
    return 0;
  }
}

struct artist *merge_artists(struct artist *x, struct artist *y, int criterion)
{

  struct artist *tmp  = NULL;
  struct artist *head = NULL;
  struct artist *curr = NULL;

  if( x == NULL )
    head = y;

  else if( y == NULL )
    head = x;

  else {

    while( x != NULL && y != NULL ) {

      // Swap x and y if x is not largest.
      if( !cmp_artists(x, y, criterion) ) {
	tmp = y;
	y   = x;
	x   = tmp;
      }

      if( head == NULL ) { // First element?
	head = x;
	curr = x;
      } else {
	curr->next = x;
	curr = curr->next;
      }
      x = x->next;
    }

    // Either x or y is empty.
    if( x != NULL )
      curr->next = x;

    else if( y != NULL )
      curr->next = y;

  }

  return head;
}

struct artist *sort_artists(struct artist *x, int criterion)
{
  struct artist *y = NULL;
  struct artist *m = NULL;

  if ( x == NULL || x->next == NULL )
    return x;

  m = find_middle_artist(x);
  y = m->next;
  m->next = NULL;

  return merge_artists( sort_artists(x, criterion),
			sort_artists(y, criterion),
			criterion );
}

struct play *sort_plays(struct play *x)
{
  struct play *y = NULL;
  struct play *m = NULL;

  if ( x == NULL || x->next == NULL )
    return x;

  m = find_middle_play(x);
  y = m->next;
  m->next = NULL;

  return merge_plays( sort_plays(x), sort_plays(y) );
}


struct artist *update_counts(struct artist *artists, struct play *plays)
{

  struct artist *head   = NULL;

  plays   = sort_plays(plays);
  artists = sort_artists(artists, ARTISTID);
  head    = artists;

  while(artists != NULL && plays != NULL) {

    if( artists->id == plays->artist_id ) {
      artists->count += plays->playcount;
      plays   = plays->next;

    } else if( artists->id < plays->artist_id ) {
      artists = artists->next;

    } else {
      plays   = plays->next;
    }
  }

  return head;
}


int main(int argc, char **argv) {

  int k = 0;
  struct artist *curr = NULL;
  struct artist *artists = NULL;
  struct play *plays = NULL;

  if( argc != 4 ) {
    printf("Please input - k(the number of artists to play:The name of the artist data file:The name of the play data file\n", argv[0]);
    exit(1);
  }

  k = atoi(argv[1]);

  artists = read_artists(argv[2]);
  if( artists == NULL )
    exit(1);

  plays = read_plays(argv[3]);
  if( plays == NULL )
    exit(1);

  artists = update_counts(artists, plays);
  artists = sort_artists(artists, PLAYCOUNT);

  for( curr = artists;
       curr != NULL && k > 0;
       curr = curr->next, k--)
    print_artist( curr );

  free_artists(artists);
  free_plays(plays);
}
