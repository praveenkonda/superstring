#include "superstring.h"





static void *allocate_string_memory(FILE *fp) 
{
     int file_size = 0;
     void *string_buffer = NULL;
     if (fp == NULL) {
         if (enable_debug) {            
             printf("input file pointer is null");
         }
         return NULL;
         
     }
     fseek(fp, 0L, SEEK_END);
     file_size = ftell(fp);
     fseek(fp, 0L, SEEK_SET);
     if(enable_debug) {
       printf("size of the file is %d", file_size);
     } 
     string_buffer = malloc(file_size);
     return string_buffer;    
}     

static void *allocate_meta_data_memory(FILE *fp)  
{
    int num_of_strings = 0;
    char word[MAX_LEN_OF_STRING];
    void *meta_data = NULL;      

    while (fgets(word, MAX_LEN_OF_STRING, fp)) {
        num_of_strings++;
    }
   
    meta_data = malloc(num_of_strings*(sizeof(string_node_t)));
    fseek(fp, 0L, SEEK_SET);
    return meta_data;
}        

static void hash_insert_meta_data(string_node_t * string_node) 
{
    int index = 0;
    int hash_index = 1;
    string_node_t *tail; 
    static bool hash_table_init = false;      

    if (!string_node) {
       if (enable_debug) {
           printf("\n hash_insert_meta_data: string_node is null");
       }
       return;
    }
    for (index = 0; index < HASH_KEY_ALPHABET_LEN; index++) {
        hash_index = (hash_index)*(toupper(string_node->word[index]) - 'A');
        index++;
    }
    
     if (!hash_table_init) {
        for (index = 0; index < HASH_BUCKETS; index++) {
            string_hash_node[index].head = NULL;
        }
        hash_table_init = true;
     }
     /* insert nodes into appropriate hash index, key is the first 
      * alphabet of the word. Could be adapted to first two, three etc
      */
      
     string_node->next = NULL;
     string_node->prev = NULL;
     if (hash_index >= HASH_BUCKETS) { 
        printf("hash_index %d out of range", hash_index);
        return;
     }
     tail = string_hash_node[hash_index].tail;               
      if (tail != NULL) {     
          tail->next = string_node;
          string_node->prev = tail;
          string_node->index = string_node->index+1;
          string_hash_node[hash_index].tail = string_node;
      } else {
          string_hash_node[hash_index].tail = string_node;
          string_hash_node[hash_index].head = string_node;
          string_node->index = 1;
      } 
}           
void read_string_and_populate_meta_data(FILE *fp,  void *string_buffer, 
                                        void *meta_data) 
{
    string_node_t *string_node;
    int hash_index = 0;     
    int index = 0;     
    int word_len = 0;
    while (fgets(string_buffer, MAX_LEN_OF_STRING, fp)) {
         string_node = (string_node_t *) meta_data;
         string_node->word = string_buffer;
         word_len = strlen(string_node->word);
         if (word_len > 0) {
            string_node->word[strlen(string_node->word) -1] = '\0';
         }
         if (enable_debug) {
            printf("\n  read string is %s", string_node->word);
         }
         hash_insert_meta_data(string_node);
         meta_data = (string_node_t *)meta_data+ 1;
         string_buffer = string_buffer + strlen(string_buffer)+1;
     }
     for (hash_index = 0; hash_index < HASH_BUCKETS; hash_index++) {
          string_node = string_hash_node[hash_index].head;  
          while(string_node != NULL) {
             if (enable_debug) {
                 printf("\n bucket %d :: index %d :: word %s",
                hash_index, string_node->index, string_node->word);
             }
             string_node = string_node->next;
          }
     }
}

int get_substring_count(char * word, char *original_word, 
                     sub_string_node_t **substring_words)
{
    int index = 0;
    int substring_count = 0;
    string_node_t *compare_node;
    int substring2_count = 0;
    int new_substring_count = 0;
    bool do_break = false;      
    int hash_index = 1;      
    char *substring2 = NULL;      
    char substring1[MAX_LEN_OF_STRING];      
    sub_string_node_t *local_sub_string_words = NULL;       
    string_node_t *tail;
 
    if (!word || !original_word) {
       if (enable_debug) {
           printf("\n word or original_word are null");
       }
       return 0;
    } 
 
    if (enable_debug) {
        printf("\n substring_count word %s:: %s", word, original_word );      
    }
     
    if (strlen(word) == 0) {
       return 0;
    } 
     
    while (index < HASH_KEY_ALPHABET_LEN) {
        hash_index = (hash_index)*(toupper(word[index]) - 'A');
        index++;
    }

       
     if (hash_index >= HASH_BUCKETS) { 
        if (enable_debug)
            printf("\n hash_index %d out of range", hash_index);
        return 0;
    }
 
    index =0;
     
    while (index < (strlen(word))) {
        memcpy(substring1, word, index+1);
        substring1[index+1] =  '\0';
        substring2 = &word[index+1];
        if (enable_debug) {
            printf ("\n substring1 :: %s,  substring2 :: %s ",
                        substring1,substring2);
        }
        compare_node = string_hash_node[hash_index].head;
        tail = string_hash_node[hash_index].tail;
        while(compare_node != NULL && (strcmp(compare_node->word, 
             original_word))) {                 
                  /* since the strings are sorted, compare only with 
                     strings ahead of it in the list */
            if (strcmp(compare_node->word, substring1) == 0) {
                if (enable_debug) {
                    printf("\n found a matching substring %s",
                      substring1);
                }
                if (strlen(substring2) > 0) {
                    substring2_count = get_substring_count(substring2,
                           original_word,&local_sub_string_words);
                    if (substring2_count > 0) {
                           new_substring_count = substring2_count+1;
                    } else {
                        new_substring_count = 0;
                    }
                }else {
                    if (enable_debug) {
                        printf("\n initializing substring count "
                                 "substring1 %s", substring1);
                    }
                    new_substring_count = 1;
                    do_break = true;
                }   
                if ((new_substring_count > 0)) { 
                    if (sub_string_node_index < MAX_NUM_OF_SUBSTRINGS) {  
                        sub_string_node_buffer[sub_string_node_index].node=
                                               compare_node;
                        sub_string_node_buffer[sub_string_node_index].next 
                                     =  local_sub_string_words;  
                        local_sub_string_words =
                             &sub_string_node_buffer[sub_string_node_index];
                        sub_string_node_index++;                                                                  
                    }
                }
                if (do_break) {
                    do_break = false;
                    break;
                }
            }
            compare_node = compare_node->next;
       }
       index++;
       if (enable_debug) {
           printf("substring_count %d new_substring_count %d",
           substring_count, new_substring_count);        
       }
       if (new_substring_count > substring_count) {
           substring_count = new_substring_count;
           *substring_words = local_sub_string_words; 
       }
    
    }
    return substring_count;
} 
                       
void sort_super_string_list(string_node_t *super_string_list,
           int max_sub_string_count)  {
      string_node_t *super_string[max_sub_string_count+1];
      string_node_t *super_node = super_string_list;
      int index =0;
      int i = 0;
      sub_string_node_t *sub_string_node;
      string_node_t *next_super_node;

      for (i=0; i <=max_sub_string_count; i++) {
         super_string[i] = NULL;
      }
      while(super_node) {
         next_super_node = super_node->next_super_node;
         if (super_node->num_of_substrings <= max_sub_string_count) {       
             index = super_node->num_of_substrings;  
            super_node->next_super_node = super_string[index];            
            super_string[index] = super_node;                            
         }
         super_node = next_super_node;
     }
     
     printf ("\n\nThe maximum number of substrings in a string is %d",
               max_sub_string_count);
     for (index=max_sub_string_count; 
            index > max_sub_string_count-MAX_LEVELS_TO_DISPLAY; index--) {
           printf("\n\nString with %d substrings", index);
           super_node = super_string[index];
           while(super_node) {
              printf("\n%s", super_node->word);
              sub_string_node = super_node->substring_nodes;
              while(sub_string_node) {
                  if (sub_string_node->node &&  sub_string_node->node->word) {
                       printf("::%s", sub_string_node->node->word);
                  }
                   sub_string_node = sub_string_node->next;
              }
              super_node = super_node->next_super_node;
          }
      }
}   
         
     
void populate_substring_counts(int *max_sub_string_count)
                                
{
   int hash_index = 0;
   string_node_t *string_node;
   int index = 0;

   sub_string_node_t *sub_string_words = NULL;
   super_string_list = NULL;
    
   *max_sub_string_count = 0;
   
   while (hash_index < HASH_BUCKETS) {      
        if (string_hash_node[hash_index].head == NULL) {
            hash_index++;
            continue;  
        }
        string_node = string_hash_node[hash_index].tail;
        while(string_node != NULL) {
           sub_string_words = NULL;
           string_node->num_of_substrings = get_substring_count(
                     string_node->word, string_node->word, &sub_string_words);
           string_node->substring_nodes = sub_string_words;
           if (string_node->num_of_substrings > 0) {
               if (enable_debug) {
                  printf("\n bucket %d , word %s, substring_count %d",
                   hash_index, string_node->word, 
                   string_node->num_of_substrings);
               }
               string_node->next_super_node = super_string_list;
               super_string_list = string_node;
               if (string_node->num_of_substrings > *max_sub_string_count) {
                   *max_sub_string_count = string_node->num_of_substrings;
               } 
           }
           
           string_node = string_node->prev;
        }
        hash_index++;
   }
}			

int main(int argc, char  **argv) 
{
    char ch, file_name[MAX_FILE_LENGTH+1];
    FILE *fp;
    int len = 0;   
    void *string_buffer = NULL; /*stores the strings*/
    void *meta_data = NULL; /* stores meta data for string , like next string
                             index in the bucket*/
    int max_sub_string_count = 0;
    char debug[] = "-enable_debug";
    enable_debug = true;
    enable_debug = false;
    
    if (argc ==2) {
       if (!strcmp(debug, (char *) argv[1]))
           enable_debug = true;
    }
    printf("Enter the name of the input file \n");
    fgets(file_name, MAX_FILE_LENGTH, stdin); 
    /* Removing the new line character*/  
    file_name[strlen(file_name) -1] = '\0';
    fp = fopen(file_name,"r"); 
    if( fp == NULL ) {
        perror("Error while opening the file.\n");   
        exit(EXIT_FAILURE);
    }
    string_buffer = allocate_string_memory(fp);
    meta_data = allocate_meta_data_memory(fp); 
    read_string_and_populate_meta_data(fp, string_buffer, meta_data);
    populate_substring_counts(&max_sub_string_count);
    sort_super_string_list(super_string_list, max_sub_string_count);    
    free(meta_data);
    free(string_buffer);
    return 0;
}  
          
    
   

