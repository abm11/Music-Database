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
  char artist_name[NAMELEN];
  int playcount;
  int alternate_artist_id;
  struct artist *next;
};

struct play
{
  int user_id;
  int p_artist_id;
  int p_playcount;
  struct play *next;
};

struct artist *create_artist(int artist_id, char *artist_name,int playcount, int alternate_artist_id)
{
    struct artist *p=(struct artist *)malloc(sizeof(struct artist));
    if(p!=NULL)
        {
        p->artist_id = artist_id;
        strcpy(p->artist_name, artist_name);
        p->playcount = playcount;
        p->alternate_artist_id = alternate_artist_id;
        }
    return p;
}

struct play *create_play(int user_id, int p_artist_id,int p_playcount)
{
    struct play *p=(struct play *)malloc(sizeof(struct artist));
    if(p!=NULL)
        {
        p->user_id = user_id;
        p->p_artist_id = p_artist_id;
        p->p_playcount = p_playcount;
        }
    return p;
}



struct artist *add_artist(struct artist *head, struct artist *newp)
{

  if( head == NULL )
    return newp;

  else if( newp == NULL )
    return head;

  else {
    newp->next = head;
    return newp;
  }
}

struct play *add_play(struct play *head, struct play *newp)
{

  if( head == NULL )
    return newp;

  else if( newp == NULL )
    return head;

  else {
    newp->next = head;
    return newp;
  }
}



struct artist *parse_artist(char *line)
{
    int artist_id; char artist_name[NAMELEN];
    if( sscanf(line, "%d\t%65[^\t\n]\n", &artist_id, artist_name)== 2)
        {
            return create_artist(artist_id, artist_name, 0, -1);
        }
    else
        return NULL;
}

struct play *parse_play(char *line)
{
  int a, b, c;
  if( sscanf(line, "%d %d %d\n", &a, &b, &c) == 3 )
    return create_play(a,b,c);
  else {
    return NULL;
  }
}



struct artist *read_artists(char *fname)
{
  char buffer[BUFSIZE];
  struct artist *head = NULL;
  int artist_id; char *artist_name;
  FILE *fp = fopen(fname, "r");
  if( fp == NULL ) {
    printf("Cannot open file %s\n", fname);
    return NULL;
  }

  while( !feof(fp) ) {
    if( fgets(buffer, BUFSIZE, fp) != NULL )
      head = add_artist( head, parse_artist(buffer) );
  }

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

  while( fgets(buffer, BUFSIZE, fp) != NULL )
    head = add_play( head, parse_play(buffer) );

  fclose( fp );
  return head;
}


void print_artist(struct artist *a) {
  if( a != NULL )
    printf("%s (%d) [%d] \n", a->artist_name, a->artist_id, a->playcount);
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


void free_artist(struct artist *p) {
  if( p != NULL )
    free(p);
}

void free_artists(struct artist *p) {
  struct artist *tmp;
  if( p != NULL ) {
    tmp = p;
    p = p->next;
    free_artist(tmp);
  }
}



void free_play(struct play *p) {
  if( p != NULL )
    free(p);
}

void free_plays(struct play *p) {
  struct play *tmp;
  if( p != NULL ) {
    tmp = p;
    p = p->next;
    free_play(tmp);
  }
}



struct play *find_middle_play(struct play *x)
{
  struct play *slow = x;
  struct play *fast = x;
  while( fast->next != NULL && fast->next->next != NULL )
    {
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
      if( x->p_artist_id > y->p_artist_id ) {
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

struct play *sort_plays(struct play *head)
{
    struct play *m=NULL;
    struct play *x=NULL;
    struct play *y=NULL;

    if (head==NULL||head->next==NULL)
    return head;

    x=head;
    m=find_middle_play(head);
    y=m->next;
    m->next=NULL;
    return merge_plays(sort_plays(x), sort_plays(y));
}



int compare_artists(struct artist *a, struct artist *b, int criterion)
{
  if( criterion == PLAYCOUNT )
    return a->playcount >= b->playcount;
  else if( criterion == ARTISTID )
    return a->artist_id <= b->artist_id;
  else {
    printf("ERROR comparison fail %d\n", criterion);
    return 0;
  }
}

struct artist *find_middle_artist(struct artist *x)
{
  struct artist *slow = x;
  struct artist *fast = x;
  while( fast->next != NULL && fast->next->next != NULL )
    {
        slow = slow->next;
        fast = fast->next->next;
    }
  return slow;
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


      if( !compare_artists(x, y, criterion) ) {
	tmp = y;
	y   = x;
	x   = tmp;
      }

      if( head == NULL ) {
	head = x;
	curr = x;
      } else {
	curr->next = x;
	curr = curr->next;
      }
      x = x->next;
    }


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

  if (x==NULL||x->next==NULL)
    return x;

  m=find_middle_artist(x);
  y=m->next;
  m->next=NULL;

  return merge_artists(sort_artists(x, criterion), sort_artists(y, criterion), criterion);
}



struct artist *update_counts(struct artist *a, struct play *p)
{
    struct artist *head  = NULL;

    p = sort_plays(p);
    a = sort_artists(a, ARTISTID);
    head = a;

    while(a != NULL && p != NULL)
    {
        if(a->artist_id==p->p_artist_id)
        {
            a->playcount += p->p_playcount;
            p   = p->next;
        }
        else if( a->artist_id < p->p_artist_id )
        {
            a=a->next;
        }
        else
            {
                p=p->next;
            }
    }
return head;
}

int main(int argc, char **argv)
{
    int k = 0; struct artist *present = NULL; struct artist *artists = NULL; struct play *plays = NULL;

    if(argc!= 4)
    {
        printf("INPUT - The number of artists to list:The artist file name:The plays file name\n", argv[0]);
        exit(1);
    }

    k=atoi(argv[1]);

    artists=read_artists(argv[2]);
    if(artists==NULL)
        exit(0);

    plays=read_plays(argv[3]);
    if(plays==NULL)
        exit(0);

    artists=update_counts(artists, plays);
    artists=sort_artists(artists, PLAYCOUNT);

    for(present=artists;present!=NULL && k>0;present=present->next, k--)
        print_artist(present);

    free_plays(plays);
    free_artists(artists);
}
