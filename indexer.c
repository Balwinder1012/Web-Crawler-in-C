#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#define null '\0'
#define MAX_DEPTH '3' 
#define MAX_LINK_SIZE 100
#define MAX_LINKS_ALLOWED 20
#define MAX_HASH_SIZE 100
#define MAX_WORD 200


typedef struct lists{
	
	char *keyword;
	char *url;
	int freq;
	struct lists *next;

}theWords;

typedef struct hash{

	theWords *head;
	theWords *last;
}hashTable;



int isValidDir(char *dir){

	struct stat statbuf;
	if ( stat(dir, &statbuf) == -1 ){
		fprintf(stderr, "-----------------\n");
		fprintf(stderr, "Invalid directory\n");
		fprintf(stderr, "-----------------\n");
		return 0;
	}

	//Both check if there's a directory and if it's writable
	if ( !S_ISDIR(statbuf.st_mode) ){
		fprintf(stderr, "-----------------------------------------------------\n");
		fprintf(stderr, "Invalid directory entry. Your input isn't a directory\n");
		fprintf(stderr, "-----------------------------------------------------\n");
		return 0;
	}

	if ( (statbuf.st_mode & S_IWUSR) != S_IWUSR ){
		fprintf(stderr, "------------------------------------------\n");
		fprintf(stderr, "Invalid directory entry. It isn't writable\n");
		fprintf(stderr, "------------------------------------------\n");
		return 0;
	}
	return 1;

	
}

char* readTheFile(char *fileName){
	
    FILE *fp;
    char *buffer;
    long long int length;

    fp = fopen(fileName,"rb");

    if(fp){

        fseek(fp,0,SEEK_END);
        length = ftell(fp);
        fseek(fp,0,SEEK_SET);
        buffer = (char *)malloc(sizeof(char)*(length+1));
        if (buffer)
            fread(buffer,1,length,fp);
        fclose(fp);
        buffer[length]=null;
		

    }
    else
        printf("\n\n######################Error in reading the file %s$\n\n",fileName);

	
    return buffer;

}

int getAllTheFiles(char *dir,char *files[500]){
	
	//char *files[500];
	int counter=0;
	struct dirent *de;  // Pointer for directory entry
 
    // opendir() returns a pointer of DIR type. 
    DIR *dr = opendir(dir);
	
 
    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        printf("Could not open current directory" );
        
    }
 
    // Refer http://pubs.opengroup.org/onlinepubs/7990989775/xsh/readdir.html
    // for readdir()
    while ((de = readdir(dr)) != NULL){
		
		int len = strlen(de->d_name);
		//printf("\n%d ",len);
		if(len >12){
			char *fileName = (char *)malloc(sizeof(char)*len+50);
			sprintf(fileName,"%s",de->d_name);
			files[counter++]=fileName;
			//printf("%s\n", files[counter-1]);
 
		}
		
			
	}
         // printf("%s\n", de->d_name);
 
    closedir(dr);   
	return counter;
	

}
int findOpenAnchorTag(char *ch,int *i){

    int j=*i;

    while(!(ch[j]=='<' && ch[j+1]=='t' && ch[j+2]=='i' && ch[j+3]=='t' && ch[j+4]=='l' && ch[j+5]=='e' && ch[j+6]=='>')){
        if(ch[j]==null){
            *i=j;
            return 0;
        }

        j++;

    }
	j=j+7;
    *i = j;
    return 1;




}
void traverseSpaces(char ch[],int *j){
    while(ch[*j]==' ')
        *j = *j + 1;
}
char *getTheWord(char *ch,int *i){

	int j=*i;
	traverseSpaces(ch,&j);
	
	char *word = (char *)malloc(sizeof(char)*MAX_WORD);
	int counter=0;
	while(ch[j]!='<' && ch[j]!=null && ch[j]!=' ' && ch[j]!='-' && ch[j]!='_' && ch[j]!='/' && ch[j]!=',' && ch[j]!='.' && ch[j]!='|'){
		word[counter++]=ch[j++];
		if(counter==MAX_WORD)
			break;
		
	}
	word[counter]=null;
	*i=j+1;
	
	return word;
	

}
int isThereWord(char *buffer,int *i){
	
	int j=*i;
	traverseSpaces(buffer,&j);

	
	if(buffer[j]==null || buffer[j]=='<' || buffer[j]=='/'){
		*i=j;
		return 0;
	}
	return 1;
	


}
int isWordValid(char *word ){
	if(!strlen(word))
		return 0;
	
	int i=0;
	while(word[i]!=null){
		if((word[i]>='0' && word[i]<='9'))
			return 0;
		i++;
	}
	return 1;


}
int getHashValue(char *word){
	
	int ascii=0,i=0;
	while(word[i]!=null){
		
		ascii += word[i++];
	}
	
	
	return ascii%100;

}

theWords *getNewNode(char *word,char *url){

	theWords *node = (theWords *)malloc(sizeof(theWords));
	node->keyword = word;
	node->freq = 1;
	node->url = url;
	return node;
	

}
theWords *isSameWordOnUrl(char *word,char *url,hashTable ht[]){
	
	int index = getHashValue(word);

	theWords *ptr;
	if((ptr=ht[index].head)==NULL)
		return ptr;

	
	theWords *last = ht[index].last;
	
	
	do{
		
		if(!strcmp(ptr->keyword,word) && !strcmp(ptr->url,url))
			return ptr;
		
			
		if(ptr==last)
			break;
		ptr = ptr->next;
	}while(1);
	
	return ptr=NULL;

}
void insertIntoList(char *word,char *url,theWords ***head,hashTable ht[]){
	

	theWords *ptr;

	
	if(ptr = isSameWordOnUrl(word,url,ht)){

		ptr->freq += 1;
	
		free(word);
		
		return;
	
	}
	
	theWords *node = getNewNode(word,url);
	int index =  getHashValue(word);
	if(ht[index].head==NULL){
		ht[index].head = node;
		ht[index].last=node;
		
		theWords *temp = **head;
		
		if(temp==NULL){
			**head = node;
			node->next=NULL;
		}
		else{
			node->next = **head;
			**head = node;
			ht[index].last=node;
		
		}
		
		if(0)
		while(temp!=NULL){
			printf("list is %s\n",temp->keyword);
			temp=temp->next;
		}
	}
	else{
		
		
		ptr = ht[index].head;
		theWords *last;
		last = ht[index].last;
		while(ptr!=last){
		
			ptr = ptr->next;
		}
		
	
		node->next = ptr->next;
		ptr->next = node;
		ht[index].last = node;
		
		
	
	}
	
	


}

char *getTheUrl(char *buffer){
	int i=0;
	int counter=0;
	while(!(buffer[i]=='U' && buffer[i+1]=='R' && buffer[i+2]=='L')){
		i++;
	}
	i += 6;

	char *url = (char *)malloc(sizeof(char)*200);
	while(buffer[i]!=' ')
		url[counter++]=buffer[i++];
	
	url[counter]=null;
	
	return url;
	

}
int extractTheWords(char *buffer,theWords **head,hashTable ht[],char *urls[]){

    int i=0;
  
	int counter=0;
	static int urlIndexer=0;
	char *url;
	
	url = getTheUrl(buffer);
	urls[urlIndexer++]=url;

        if(findOpenAnchorTag(buffer,&i) ){
			
			while(isThereWord(buffer,&i)){
				
               int j=0;
				
         	   char *word;
			
			
         	   word = getTheWord(buffer,&i);
				
         	   if(isWordValid(word) ){
				 
				  
				   insertIntoList(word,url,&head,ht);
				   
         	       counter++;
				   
					if(counter==MAX_LINKS_ALLOWED){
					
						break;
					}
				
			 		
			

          	  }
              else
                free(word);
							
			
			}
			

        }
	
	//Scanning meta tags now
	
	
	
		

		
    return urlIndexer;

}


void main(int argc,char *argv[]){
	
	hashTable ht[100];
	char *urls[1000];  //this array holds the addresses of the urls so that they could be freed later
	int urlIndexer;
	
	for(int i=0;i<100;i++)
		ht[i].head=NULL;
	
	
	theWords *head = NULL;
	char *files[500];
	
	int n; //n denotes number of files
	
	if(isValidDir(argv[1])){
		n=getAllTheFiles(argv[1],files);
		
		for(int i=0;i<n;i++){
		
			char fileName[100];
			sprintf(fileName,"%s/%s",argv[1],files[i]);
			printf("Reading File %s\n",fileName);
			
			free(files[i]);
			char *buffer;
			buffer = readTheFile(fileName);
			
			urlIndexer = extractTheWords(buffer,&head,ht,urls);
	
			
			
			free(buffer);
	
			
		
		}
		
		if(head==NULL)
			printf("head is null");
		theWords *ptr = head;
		printf("\n");
		
	
		while(ptr!=NULL){
			
			printf("%-90s %-40s %d\n",ptr->url,ptr->keyword,ptr->freq);
			
			theWords *t = ptr;
			ptr=ptr->next;
		
			free(t->keyword);
			free(t);
			
		}
		for(int i=0;i<urlIndexer;i++)
			free(urls[i]);
			
		
		
	
	}
	else{
	
		printf("\ninvalid directory");
	}





}
