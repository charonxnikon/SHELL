                        /*    by Nikon    */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

enum {size_def = 255};

struct chunk{
   char mass[size_def];
   int size_mass;
   struct chunk *next;
};

struct chunk_list{
   struct chunk *word;
   struct chunk_list *next;
};

struct ch_ch_list{
   struct chunk_list *chlist; 
   struct ch_ch_list *next;
};

struct main_list{
   char *mystr; 
   struct main_list *next;
};

struct m_m_list{
   struct main_list *mlist;
   struct m_m_list *next;
};

struct info_{
   int letter, quotes, amper;
   int nextsy;
   int larrow, rarrow;
   int afn, lastc; 
};

struct file_names{
   char *st_in;
   char *st_out;
};

struct pid_list{
   int pid; 
   struct pid_list *next;
};

void
expand_main(struct main_list **main, struct main_list **last)
{
   struct main_list *tmp;
   tmp = malloc(sizeof(struct main_list));
   tmp->next = NULL;
   if (*main == NULL){
      *main = tmp;
      *last = tmp;
   }else{
      (*last)->next = tmp;
      *last = tmp;
   }
}

void 
expand_m_m(struct m_m_list **main, struct m_m_list **last)
{
   struct m_m_list *tmp; 
   tmp = malloc(sizeof(struct m_m_list));
   tmp->next = NULL; 
   tmp->mlist = NULL;
   if (*main == NULL){
      *main = tmp; 
      *last = tmp;
   }else{
      (*last)->next = tmp; 
      *last = tmp;
   }
}

void
expand_chunklist(struct chunk_list **first,
                 struct chunk_list **last)
{
   struct chunk_list *tmp;
   tmp = malloc(sizeof(struct chunk_list));
   tmp->next = NULL;
   tmp->word = NULL;
   if (*first == NULL){
      *first = tmp;
      *last = tmp;
   }else{
      (*last)->next = tmp;
      *last = tmp;
   }
}

void
expand_chunk(struct chunk **first, struct chunk **last)
{
   struct chunk *tmp;
   tmp = malloc(sizeof(struct chunk));
   tmp->next = NULL;
   if (*first == NULL){
      *first = tmp;
      *last = tmp;
   }else{
      (*last)->next = tmp;
      *last = tmp;
   }
}

void 
expand_ch_ch(struct ch_ch_list **first, struct ch_ch_list **last)
{
   struct ch_ch_list *tmp; 
   tmp = malloc(sizeof(struct chunk));
   tmp->next = NULL;
   tmp->chlist = NULL; 
   if (*first == NULL){
      *first = tmp; 
      *last = tmp; 
   }else{
      (*last)->next = tmp; 
      *last = tmp; 
   }
}

void
expand_plist(struct pid_list **p, struct pid_list **last, int pid)
{
   struct pid_list *tmp;
   tmp = malloc(sizeof(struct pid_list));
   tmp->pid = pid;
   tmp->next = NULL; 
   if (*p == NULL){
      *p = tmp; 
      *last = tmp;
   }else{ 
      (*last)->next = tmp; 
      *last = tmp;
   }
}

int
sum_size(struct chunk *first)
{
   int count = 0;
   struct chunk *tmp;
   tmp = first;
   while (tmp){
      count += tmp->size_mass;
      tmp = tmp->next;
   }
   return count;
}

void
ch_2str(char **mystr, struct chunk *first)
{
   int i, j = 0;
   struct chunk *tmp;
   tmp = first;
   if (tmp){
      *mystr = malloc(sum_size(first)+1);
      while (tmp){
         for (i = 0;i<tmp->size_mass;i++,j++){
            if (i<tmp->size_mass){
               (*mystr)[j] = (tmp->mass)[i];
            }
            if (i == size_def-1){
               i = 0;
            }
         }
         tmp = tmp->next;
      }
      (*mystr)[j] = '\0';
   }
}

void
chlist_2main(struct main_list **m, struct chunk_list *list)
{
   struct chunk_list *tmp;
   struct main_list *last_m;
   tmp = list;
   while (tmp){
      expand_main(m, &last_m);
      ch_2str(&(last_m->mystr), tmp->word);
      tmp = tmp->next;
   }
}

struct m_m_list *
ch_ch2m_m(struct ch_ch_list *ch)
{
   struct m_m_list *last_m_m = NULL, *mainn = NULL; 
   while (ch){
      expand_m_m(&mainn, &last_m_m);
      chlist_2main(&(last_m_m->mlist), ch->chlist);
      ch = ch->next;
   }
   return mainn; 
}

int
quantity_str(struct main_list *main)
{
   int count = 0;
   struct main_list *tmp;
   tmp = main;
   for (; tmp; count++){
      tmp = tmp->next;
   }
   return count;
}

char **
main_2mpoint(struct main_list *main)
{
   int i;
   struct main_list *tmp;
   char **m_point;
   tmp = main;
   m_point = malloc((quantity_str(main)+1)*sizeof(*m_point));
   for(i = 0;tmp;i++){
      m_point[i] = tmp->mystr;
      tmp = tmp->next;
   }
   m_point[i] = NULL;
   return m_point;
}

void
free_chunk(struct chunk **first)
{
   struct chunk *tmp;
   while (*first){
      tmp = *first;
      *first = (*first)->next;
      free(tmp);
   }
   *first = NULL;
}

void
last_ncreate(struct file_names *fn, struct info_ *f,
             struct chunk *first)
{
   if (f->larrow == 1 || f->larrow == 2){
      if (!(fn->st_out)){
         ch_2str(&(fn->st_out), first);
      }
   }
   if (f->rarrow == 1 && !(fn->st_in)){
      ch_2str(&(fn->st_in), first);
   }
}

void
create_names(struct file_names *fn, struct info_ *f)
{
   int i, c; 
   struct chunk *fname = NULL, *last = NULL; 
   for(i = 0; (c = getchar()) != EOF && c != '\n'; i++){
      if (c == ' ' && fname){
         break;
      }
      switch (c){
         case ' ':
            i--;
            break; 
         case '<':
            free_chunk(&fname);
            (f->rarrow)++;
            return;
         case '&':
            i--;
            (f->amper)++;
            f->nextsy = 0;
            break;
         case '>':
            if (f->larrow == 2){
               free_chunk(&fname);
               (f->larrow)++;
               return;
            }
            (f->larrow)++;  
            i--;
            break;     
         default:
            (f->nextsy)++;
            if (!(i <= size_def && fname)){
               i = 0; 
               expand_chunk(&fname, &last);
            }
            last->size_mass = i+1;
            (last->mass)[i] = c;
            break;
         }
   }
   last_ncreate(fn, f, fname);
   free_chunk(&fname);
   if (c == EOF || c == '\n'){
      f->afn = 1;
   }
}

void
init_names(struct file_names *fn, struct info_ *f)
{
   if (f->larrow == 1 || f->larrow == 2){
      if (!(fn->st_out) ||
         (f->rarrow == 1 && fn->st_out)){
         create_names(fn, f);
      }else{ 
         f->larrow = 3;
      }
   }else{
      if (f->rarrow == 1){
         create_names(fn, f);
      }
   }
}

void
free_chunk_list(struct chunk_list **list)
{
   struct chunk_list *tmp;
   while (*list){
      tmp = *list;
      free_chunk(&(tmp->word));
      *list = (*list)->next;
      free(tmp);
   }
   *list = NULL;
}

void 
free_ch_ch(struct ch_ch_list **list)
{
   struct ch_ch_list *tmp; 
   while (*list){
      tmp = *list; 
      free_chunk_list(&((*list)->chlist));
      *list = (*list)->next; 
      free(tmp);
   }
   *list = NULL;
}

void
free_main(struct main_list **main)
{
   struct main_list *tmp;
   while (*main){
      tmp = *main;
      free(tmp->mystr);
      *main = (*main)->next;
      free(tmp);
   }
   *main = NULL;
}

void 
free_m_m(struct m_m_list **main)
{
   struct m_m_list *tmp; 
   while (*main){
      tmp = *main; 
      free_main(&((*main)->mlist)); 
      *main = (*main)->next; 
      free(tmp);
   }
   *main = NULL;
}

void
free_masspoint(char ***masspoint)
{
   char **tmp;
   tmp = *masspoint;
   free(tmp);
   *masspoint = NULL;
}

void 
free_plist(struct pid_list **plist)
{
   struct pid_list *tmp; 
   while (*plist){
      tmp = *plist;
      *plist = (*plist)->next; 
      free(tmp);
   }    
}

void
free_all(struct m_m_list **main, struct ch_ch_list **list)
{
   free_m_m(main);
   free_ch_ch(list);
}

void
refresh_info(struct info_ *f)
{
   f->letter = 0;
   f->quotes = 0;
   f->nextsy = 0;
   f->amper = 0;
   f->larrow = 0;
   f->rarrow = 0;
   f->afn = 0;
}

struct info_ * 
creat_info()
{
   struct info_ *tmp; 
   tmp = malloc(sizeof(struct info_));
   refresh_info(tmp);
   return tmp;
}


void
refresh_names(struct file_names *f)
{
   if (f->st_in){
      free(f->st_in);
   }
   if (f->st_out){
      free(f->st_out);
   } 
   f->st_in = NULL;
   f->st_out = NULL;
}

void
refresh(struct info_ *f, struct file_names *fn)
{
   refresh_info(f);
   refresh_names(fn);
}

void
check_info(struct ch_ch_list **list, struct info_ *f,
            struct file_names *fn)
{
   if (f->quotes == 1){
      fprintf(stderr, "Nikon: ERROR >> close quotes\n");
      free_ch_ch(list);
   }
   if (f->amper){
      if (f->nextsy || f->amper > 1 || (f->amper == 1 && !(*list))){
         fprintf(stderr, "Nikon: ERROR >> check &s\n");
         f->amper = 0;
         free_ch_ch(list);
      }
   }
   if ((f->larrow && !(fn->st_out))||
         (f->rarrow && !(fn->st_in))){
      fprintf(stderr, "Nikon: ERROR >> check arrows\n");
      free_ch_ch(list);
      refresh_names(fn);
   }else{
      if (f->larrow > 2 || f->rarrow > 1){
         fprintf(stderr, "nikon: ERROR >> check arrows\n");
	      free_ch_ch(list);
	      refresh_names(fn);
      }
   }
}

int
analize(struct info_ *f, struct file_names *fn, int c)
{
   switch (c){
         case '"':
            f->quotes = !(f->quotes);
            return 1;
         case '&':
            if (!(f->quotes)){
               (f->amper)++;
               f->nextsy = 0;
               return 1;
            }
         case ' ':
            if (!(f->quotes)){
               f->letter = 0;
               return 1;
            }
         case '>':
            if (!(f->quotes)){
               (f->larrow)++;
               init_names(fn, f);
               return 1;
            }
         case '<':
            if (!(f->quotes)){
		         (f->rarrow)++;
               init_names(fn, f); 
               return 1;
            }
         case '|':
            if (!(f->quotes)){
               return 2;
            }               
         default:
            return 0;
   }
}

void 
refr_infini(struct info_ *f)
{
   f->letter = 0;
   f->quotes = 0;
   f->nextsy = 0;
}

/* parsing (init struct ch_ch_list) by info */

struct ch_ch_list *
initialize(struct info_ *f, struct file_names *fn)
{
   int i, c, j = 0;
   struct chunk *last = NULL;
   struct chunk_list *lastch = NULL;
   struct ch_ch_list *lastchch = NULL, *first = NULL;
   printf("Nikon: input >> ");
   for(i = 0; (c = getchar()) != '\n' && c != EOF;i++){
         if (analize(f, fn, c) == 1){
            i--;
         }else{
            if (analize(f, fn ,c) == 2){ 
               j = i = 0;
            }else{
               (f->nextsy)++;
               if (!j){
                  expand_ch_ch(&first, &lastchch);
                  refr_infini(f); 
                  j++;
               }
               if (!(f->letter)){
                  expand_chunklist(&(lastchch->chlist), &lastch);
                  f->letter = 1;
                  i = 0;
               }
               if (!(i <= size_def && (lastch->word) != NULL)){
                  i = 0;
                  expand_chunk(&(lastch->word), &last);
               }
               last->size_mass = i+1;
               (last->mass)[i] = c;
            }
         }
         if (f->afn){
            break;
         }
   }
   f->lastc = c; 
   return first; 
}

void
rm_p_plist(struct pid_list *plist, int pidw)
{
   int fl = 0; 
   while (plist){
      if (plist->pid == pidw){
         fl = 1;
         break;
      }
      plist = plist->next; 
   }
   if (fl == 1){
      plist->pid = 0;
   } 
}

int 
check_plist(struct pid_list *plist)
{
   int count = 0;
   while (plist){
      if (plist->pid != 0){
         count++;
      }
      plist = plist->next; 
   }
   return count;
}

int
cmpcd(char *str)
{
   return (str[0] == 'c' && str[1] == 'd' && str[2] == '\0');
}

void cln_zombies()
{
   while (0<wait4(-1,NULL,WNOHANG,NULL))
      ;
}

void
make_cd(char *adr)
{
   int result; 
   if (adr){
      result = chdir(adr);
      if (result == -1){
         perror(adr);
      }
   }else{
      result = chdir(getenv("HOME"));
	   if (result == -1){
         fprintf(stderr,"Nikon: ERROR >> no adress\n");
      }
   }
}

void
exec_first_in(struct file_names *fn)
{
   int res;
   if (fn->st_in){
      res = open(fn->st_in, O_RDONLY);
      if (res != -1){
         dup2(res, 0);
         close(res);
      }else{
         perror(fn->st_in);
         exit(1);
      }
   }
}

void
exec_last_out(struct file_names *fn, struct info_ *f)
{
   int res;
   if (fn->st_out){
      if (f->larrow == 1){
         res = open(fn->st_out,O_WRONLY|O_CREAT|O_TRUNC,0666);
      }else{
         res = open(fn->st_out,O_WRONLY|O_CREAT|O_APPEND,0666);
      }
      if (res != -1){
         dup2(res, 1);
         close(res);
      }else{
         perror(fn->st_out);
         exit(1);
      }
   }
}

void 
waiting(struct info_ *f, struct pid_list *plist)
{
   int pid;
   if (f->amper == 0){
      while (check_plist(plist)){
         pid = wait(NULL);
         rm_p_plist(plist, pid);
      }
   }
   free_plist(&plist);
}

void
execute_next(struct m_m_list *main, struct info_ *f,
             struct file_names *fn)
{
   int fd[2], pid, i, prevfd = -1;
   char **m_point = NULL;
   struct pid_list *plist = NULL, *plila = NULL; 
   for(i = 0; main; i++){
      pipe(fd);
      pid=fork();
      m_point = main_2mpoint(main->mlist);
      if (pid == 0){
         if (i == 0){
            exec_first_in(fn);
         }else{
            dup2(prevfd, 0);
            close(prevfd);
         }
         if (!(main->next)){
            exec_last_out(fn, f);     
         }else{
            dup2(fd[1], 1);
         }
         close(fd[1]);
         close(fd[0]);
         execvp(m_point[0],m_point);
         perror(m_point[0]);
         fflush(stderr);
         _exit(1);
      }
      expand_plist(&plist, &plila, pid);
      close(fd[1]);
      if (prevfd != -1){
         close(prevfd);
      }
      prevfd = fd[0];
      if (!(main->next)){
         close(prevfd);
      }
      main = main->next;
      free_masspoint(&m_point);      
   }
   waiting(f, plist); 
}

void 
execute_command(struct m_m_list *main, struct info_ *f,
                struct file_names *fn)
{
   char **m_point = NULL;
   m_point = main_2mpoint(main->mlist);
   if (cmpcd(m_point[0])){
      if (main->next){
         fprintf(stderr, "Nikon: >> unnormal use \n");
         free_masspoint(&m_point);
         return;
      }else{
         make_cd(m_point[1]);
      }
   }else{
      free_masspoint(&m_point);
      execute_next(main, f, fn);
   }
} 

struct file_names *
creat_names()
{
   struct file_names *tmp; 
   tmp = malloc(sizeof(struct file_names));
   tmp->st_in = NULL; 
   tmp->st_out = NULL;
   return tmp; 
}

int
main()
{
   struct info_ *inf;
   struct file_names *fn; 
   struct ch_ch_list *ch_ch = NULL;
   struct m_m_list *m_main = NULL;
   inf = creat_info();
   fn = creat_names();
   do{
      ch_ch = initialize(inf, fn);
      check_info(&ch_ch, inf, fn);
      if (ch_ch){
         m_main = ch_ch2m_m(ch_ch);
      }
      if(m_main){ 
         execute_command(m_main, inf, fn);
      }
      cln_zombies();
      free_all(&m_main, &ch_ch);
      refresh(inf, fn);
   } while (inf->lastc != EOF);
   putchar('\n');
   free(inf);
   free(fn);
   return 0;
}
