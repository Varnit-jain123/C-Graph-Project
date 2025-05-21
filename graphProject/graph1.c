#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<tm_avl_tree.h>
#include<tm_pair.h>
#include<tm_pqueue.h>
typedef struct __city
{
int code;
char name[51];
} City;

typedef struct __city_header
{
int lastGeneratedCode;
int recordCount;
} CityHeader;

//global data structures
AVLTree *cities;
AVLTree *citiesByName;
CityHeader cityHeader;
AVLTree *graph;

int cityCodeComparator(void *left,void *right)
{
City *leftCity=(City *)left;
City *rightCity=(City *)right;
return leftCity->code-rightCity->code;
}

int cityNameComparator(void *left,void *right)
{
City *leftCity=(City *)left;
City *rightCity=(City *)right;
return strcasecmp(leftCity->name,rightCity->name); //strcasecmp for incase sensitive comparison
}

int graphVertexComparator(void *left,void *right)
{
City *leftCity;
City *rightCity;
Pair *leftPair=(Pair*)left;
Pair *rightPair=(Pair*)right;
leftCity=(City *)leftPair->first;
rightCity=(City *)rightPair->first;
return strcasecmp(leftCity->name,rightCity->name); //strcasecmp for incase sensitive comparison
} 

int adjacentVertexComparator(void *left,void *right)
{
City *leftCity;
City *rightCity;
Pair *leftPair=(Pair*)left;
Pair *rightPair=(Pair*)right;
leftCity=(City *)leftPair->first;
rightCity=(City *)rightPair->first;
return strcasecmp(leftCity->name,rightCity->name); //strcasecmp for incase sensitive comparison
}

void populateDataStructures(int *success)
{
FILE *cityFile;
FILE *graphFile;
City *city;
City *advCity;
Pair *p1,*p2;
AVLTree *advTree;
int *edgeWeight;
int acode,weight;
City c;
char m;
int code;
int succ;
if(success) *success=false;
printf("Please wait, loading Data...\n");
cities=createAVLTree(&succ,cityCodeComparator);
if(!succ)
{
printf("unable to load data, low memory issue\n");
return;
}
citiesByName=createAVLTree(&succ,cityNameComparator);
if(!succ)
{
printf("unable to load data, low memory issue\n");
destroyAVLTree(cities);
return;
}

graph=createAVLTree(&succ,graphVertexComparator);
if(!succ)
{
printf("Unable to load data, low memory issue\n");
destroyAVLTree(cities);
destroyAVLTree(citiesByName);
return;
}

cityFile=fopen("city.dat","rb");
if (cityFile != NULL) 
{
fread(&cityHeader,sizeof(CityHeader),1,cityFile);
if(!(feof(cityFile)))
{
while(1)
{
fread(&c,sizeof(City),1,cityFile);
if(feof(cityFile)) break;
city=(City *)malloc(sizeof(City));
//checking if city is null is pending 
city->code=c.code;
strcpy(city->name,c.name);
insertIntoAVLTree(cities,(void *)city,&succ);
insertIntoAVLTree(citiesByName,(void *)city,&succ);
}
fclose(cityFile);
if(getSizeOfAVLTree(cities)>0)
{
graphFile=fopen("graph.dat","r");
if(graphFile!=NULL)
{
while(1)
{
code=0;
m=fgetc(graphFile);
if(feof(graphFile)) break;
code=m-48;
while(1)
{
m=fgetc(graphFile);
if(m==',') break;
code=(code*10)+(m-48);
}
c.code=code;
city=(City*)getFromAVLTree(cities,(void *)&c,&succ);
p1=(Pair *)malloc(sizeof(Pair));
// pending success check
p1->first=(void *)city;
advTree=createAVLTree(&succ,adjacentVertexComparator);
// pending success check
p1->second=(void *)advTree;
while(1)
{
acode=0;
while(1)
{
m=fgetc(graphFile);
if(m==',') break;
acode=(acode*10)+(m-48);
}
weight=0;
while(1)
{
m=fgetc(graphFile);
if(m==',' || m=='#') break;
weight=(weight*10)+(m-48);
}
c.code=acode;
advCity=(City *)getFromAVLTree(cities,(void *)&c,&succ);
p2=(Pair *)malloc(sizeof(Pair));
p2->first=(void *)advCity;
edgeWeight=(int *)malloc(sizeof(int));
*edgeWeight=weight;
p2->second=(void *)edgeWeight;
insertIntoAVLTree(advTree,p2,&succ); 
// success part pending
if(m=='#') break;
}   // all adjacent cities elements reading path complete
insertIntoAVLTree(graph,p1,&succ);
    // success part pending
}  //graphFile reading content loops ends
fclose(graphFile);
}  //graphFile!=NULL if parts ends
}
} 
} //loading data from data file ends here
if(success) *success=true;
if(getSizeOfAVLTree(cities)==0)
{
cityHeader.lastGeneratedCode=0;
cityHeader.recordCount=0;
}
}

void releaseDataStructures()
{
// releasing city type structures is pending
destroyAVLTree(cities);
destroyAVLTree(citiesByName);
}

void addCity()
{
City c;
City *city;
FILE* file;
char m;
char name[52];
int succ;
printf("City (Add Module) \n");
printf("Enter name of city to add : ");
fgets(name,52,stdin);
if(name[strlen(name)-1]=='\n') name[strlen(name) - 1]='\0';
else while(getchar()!='\n');
strcpy(c.name,name);
city=(City *)getFromAVLTree(citiesByName,(void*)&c,&succ);
if(succ)
{
printf("%s, exists\n",name);
return;
}
printf("Save (Y/N) : ");
m=getchar();
while(getchar()!='\n');
if(m!='y' && m!='Y') 
{
printf("%s not saved\n",name);
return;
}
file=fopen("city.dat","rb+");
if(file==NULL)
{
file=fopen("city.dat","wb+");
fwrite(&cityHeader,sizeof(CityHeader),1,file);
}
else
{
fseek(file,0,SEEK_END);
}
cityHeader.recordCount++;
cityHeader.lastGeneratedCode++;
c.code=cityHeader.lastGeneratedCode;
fwrite(&c,sizeof(City),1,file);
fseek(file,0,SEEK_SET);
fwrite(&cityHeader,sizeof(CityHeader),1,file);
fclose(file);
city=(City*)malloc(sizeof(City));
// if unable to allocate memory part pending
city->code=c.code;
strcpy(city->name,c.name);
insertIntoAVLTree(cities,(void*)city,&succ);
//checking of succ pending
insertIntoAVLTree(citiesByName,(void*)city,&succ);
//checking of succ pending
printf("%s added, press enter to continue.....",c.name);
while(getchar()!='\n');
}

void searchCity()
{
AVLTreeInOrderIterator avlTreeInOrderIterator; 
City* city;
City c;
int succ;
char name[52];
Pair graphP;
Pair *graphPair;
AVLTree *advTree;
Pair *advPair;
City *advCity;
int* weight;
printf("City (search module)\n");
printf("Enter name of city to search : ");
fgets(name,52,stdin);
if(name[strlen(name)-1]=='\n') name[strlen(name) - 1]='\0';
else while(getchar()!='\n');
strcpy(c.name,name);
city=(City*)getFromAVLTree(citiesByName,(void*)&c,&succ);
if(!succ) 
{
printf("%s does not exists \n",name);
}
else
{
printf("%s exists\n",city->name);
graphP.first=(void *)city;
graphPair=(Pair *)getFromAVLTree(graph,(void *)&graphP,&succ);
if(succ)
{
printf("City %s, is directly connected to following cities \n",city->name);
advTree=(AVLTree *)graphPair->second;
avlTreeInOrderIterator=getAVLTreeInOrderIterator(advTree,&succ);
while(hasNextInOrderElementInAVLTree(&avlTreeInOrderIterator))
{
advPair=(Pair *)getNextInOrderElementFromAVLTree(&avlTreeInOrderIterator,&succ);
advCity=(City *)advPair->first;
weight=(int*)advPair->second;
printf("City %s, Distance %d\n",advCity->name,*weight);
}
}
}
printf("press enter to continue....");
while(getchar()!='\n');
}

void drawLine(char chr,int size)
{
int e;
for(e=1;e<=size;e++) printf("%c",chr);
printf("\n");
}

void displayCities()
{
int sno;
int displayHeader;
AVLTreeInOrderIterator avlTreeInOrderIterator;
City *city;
int succ;
int pageSize;
int sz=getSizeOfAVLTree(cities);
printf("Cities (Display Module)\n");
if(sz==0)
{
printf("City (Display Module)\n");
printf("No cities exists, press Enter to continue....");
while(getchar()!='\n');
return;
}
displayHeader=1;
pageSize=5;
sno=0;
avlTreeInOrderIterator=getAVLTreeInOrderIterator(citiesByName,&succ);
while(hasNextInOrderElementInAVLTree(&avlTreeInOrderIterator))
{
if(displayHeader)
{
drawLine('-',70);
printf("Cities (Display Module)\n");
drawLine('-',70);
printf("     S.no. City\n");
drawLine('-',70);
displayHeader=0;
}	
sno++;
city=(City*)getNextInOrderElementFromAVLTree(&avlTreeInOrderIterator,&succ);
printf("%10d %-50s\n",sno,city->name);
if(sno%pageSize==0 || sno==sz)
{
drawLine('-',70);
displayHeader=1;
printf("Press Enter to Continue....");
while(getchar()!='\n');  
}
}
}

void editCity()
{
City* city;
City* city2;
City c;
char m;
int succ,position;
FILE *cityFile;
CityHeader cheader;
char name[52],newName[52];
printf("City (Edit Modlue)\n");
printf("enter name of city to edit : ");
fgets(name,52,stdin);
if(name[strlen(name)-1]=='\n') name[strlen(name) - 1]='\0';
else while(getchar()!='\n');
strcpy(c.name,name);
city=(City *)getFromAVLTree(citiesByName,(void *)&c,&succ);
if(!succ)
{
printf("city %s, does not exists, press enter to continue....",name);
while(getchar()!='\n');
return;
}
printf("City : %s\n",city->name);
printf("Edit (Y/N) : ");
m=getchar();
while(getchar()!='\n');
if(m!='y' && m!='Y')
{
printf("City %s, not updated, press enter to continue....",name);
while(getchar()!='\n');
return;
}
printf("Enter new name of city : ");
fgets(newName,52,stdin);
if(newName[strlen(newName)-1]=='\n') newName[strlen(newName) - 1]='\0';
else while(getchar()!='\n');
if(strcmp(name,newName)!=0)
{
strcpy(c.name,newName);
city2=(City *)getFromAVLTree(citiesByName,(void *)&c,&succ);
if(succ && city->code!=city2->code) //very imp concept
{
printf("City : %s exists, press enter to continue....",newName);
while(getchar()!='\n');
return;
}
}
printf("Update (Y/N) : ");
m=getchar();
while(getchar()!='\n');
if(m!='y' && m!='Y')
{
printf("City : %s not updated, press enter to continue....",name);
while(getchar()!='\n');
return;
}
cityFile=fopen("city.dat","rb+");
fread(&cheader,sizeof(CityHeader),1,cityFile);
while(1)
{
position=ftell(cityFile);
fread(&c,sizeof(City),1,cityFile);
if(feof(cityFile)) break;
if(strcmp(city->name,c.name)==0)
{
fseek(cityFile,position,SEEK_SET);
c.code=city->code;
strcpy(c.name,newName);
fwrite(&c,sizeof(City),1,cityFile);
break;
}
} 
fclose(cityFile);
strcpy(c.name,city->name);
c.code=city->code;
removeFromAVLTree(citiesByName,(void *)&c,&succ);
// the following removal is not required, as code has not changed and we are using the same data structures
//removeFromAVLTree(cities,(void *)&c,&succ);
strcpy(city->name,newName);
insertIntoAVLTree(cities,(void*)city,&succ);
insertIntoAVLTree(citiesByName,(void*)city,&succ);
printf("City %s updated to city %s\n",name,newName);
printf("press enter to continue....");
while(getchar()!='\n');
return;
}

void removeCity()
{
City* city;
City c;
char m;
int succ;
FILE *cityFile,*tmpFile;
CityHeader cheader;
char name[52];
printf("City (Delete Modlue)\n");
printf("enter name of city to delete : ");
fgets(name,52,stdin);
if(name[strlen(name)-1]=='\n') name[strlen(name) - 1]='\0';
else while(getchar()!='\n');
strcpy(c.name,name);
city=(City *)getFromAVLTree(citiesByName,(void *)&c,&succ);
if(!succ)
{
printf("city %s, does not exists, press enter to continue....",name);
while(getchar()!='\n');
return;
}
printf("City : %s\n",city->name);
printf("Delete (Y/N) : ");
m=getchar();
while(getchar()!='\n');
if(m!='y' && m!='Y')
{
printf("City %s, not removed, press enter to continue....",name);
while(getchar()!='\n');
return;
}

cityFile=fopen("city.dat","rb");
tmpFile=fopen("tmp.tmp","wb");
fread(&cheader,sizeof(CityHeader),1,cityFile);
cityHeader.recordCount--;
fwrite(&cityHeader,sizeof(CityHeader),1,tmpFile);
while(1)
{
fread(&c,sizeof(City),1,cityFile);
if(feof(cityFile)) break;
if(strcmp(city->name,c.name)!=0)
{
fwrite(&c,sizeof(City),1,tmpFile);
}
} 
fclose(cityFile);
fclose(tmpFile);
cityFile=fopen("city.dat","wb");
tmpFile=fopen("tmp.tmp","rb");
fread(&cheader,sizeof(CityHeader),1,tmpFile);
fwrite(&cheader,sizeof(CityHeader),1,cityFile);
while(1)
{
fread(&c,sizeof(City),1,tmpFile);
if(feof(tmpFile)) break;
fwrite(&c,sizeof(City),1,cityFile);
}
fclose(cityFile);
fclose(tmpFile);
tmpFile=fopen("tmp.tmp","wb");
fclose(tmpFile);
c.code=city->code;
strcpy(c.name,city->name);
removeFromAVLTree(citiesByName,(void *)&c,&succ);
removeFromAVLTree(cities,(void *)&c,&succ);
printf("City %s is removed, press enter to continue....",name);
while(getchar()!='\n');
return;
}

void addAdjacentVertex()
{
int cityCode,advCode;
City *city;
City* advCity;
char name[52];
char advName[52];
char m;
Pair *graphPair;
Pair *advPair;
City c;
City advC;
int succ;
Pair gPair;
Pair aPair;
int shouldAppend;
FILE *graphFile;
int weight;
int *wgt;
AVLTree *advTree;
Pair *gp,*advp;
City *gct,*advc;
int *ewgt;
AVLTree *advt;
AVLTreeInOrderIterator graphAvlTreeInOrderIterator,advAvlTreeInOrderIterator;
printf("Enter city against which you want to add adjacent cities : ");
fgets(name,52,stdin);
if(name[strlen(name)-1]=='\n') name[strlen(name) - 1]='\0';
else while(getchar()!='\n');
strcpy(c.name,name);
city=(City *)getFromAVLTree(citiesByName,(void *)&c,&succ);
if(!succ)
{
printf("City %s does not exists \n",name);
return;
}
cityCode=city->code;
gPair.first=(void *)city; 
graphPair=(Pair*)getFromAVLTree(graph,(void *)&gPair,&succ);
shouldAppend=0;
if(!succ)
{
graphPair=NULL;
shouldAppend=1;
}
while(1)
{
printf("Enter city adjacent to %s : ",name);
fgets(advName,52,stdin);
if(advName[strlen(advName)-1]=='\n') advName[strlen(advName) - 1]='\0';
else while(getchar()!='\n');
strcpy(advC.name,advName);
advCity=(City *)getFromAVLTree(citiesByName,(void *)&advC,&succ);
if(!succ)
{
printf("City %s does not exists \n",advName);
printf("Want to add another city as city adjacent to %s (Y/N) : ",name);
m=getchar();
while(getchar()!='\n');
if(m!='y' && m!='Y') break;
continue;
}
advCode=advCity->code;
if(graphPair!=NULL) 
{
aPair.first=(void *)advCity;
getFromAVLTree((AVLTree*)graphPair->second,&aPair,&succ);
if(succ)
{
printf("City %s exists as adjacent to city %s\n",advName,name);
printf("Want to add another city as city adjacent to %s (Y/N) : ",name);
m=getchar();
while(getchar()!='\n');
if(m!='y' && m!='Y') break;
continue;
}
}
printf("Enter weight of edge form %s to %s : ",name,advName);
scanf("%d",&weight);
while(getchar()!='\n');
if(weight<=0)
{
printf("Invalid Weight\n");
printf("Want to add another city as city adjacent to %s (Y/N) : ",name);
m=getchar();
while(getchar()!='\n');
if(m!='y' && m!='Y') break;
continue;
}
printf("Save (Y/N) : ");
m=getchar();
while(getchar()!='\n');
if(m=='Y' || m=='y')
{
if(graphPair==NULL) 
{
graphPair=(Pair*)malloc(sizeof(Pair));
graphPair->first=(void *)city;
advTree=createAVLTree(&succ,adjacentVertexComparator);
// success check pending
graphPair->second=(void *)advTree;
advPair=(Pair *)malloc(sizeof(Pair));
advPair->first=(void *)advCity;
wgt=(int *)malloc(sizeof(int));
*wgt=weight;
advPair->second=(void *)wgt;
insertIntoAVLTree(advTree,(void *)advPair,&succ);
// succ and malloc check pending
insertIntoAVLTree(graph,graphPair,&succ);
// succ part pending
graphFile=fopen("graph.dat","a");
fprintf(graphFile,"%d,%d,%d#",cityCode,advCode,weight);
fclose(graphFile);
}
else if(shouldAppend==1)
{
graphFile=fopen("graph.dat","r+");
fseek(graphFile,-1,SEEK_END);
fprintf(graphFile,",%d,%d#",advCode,weight);
fclose(graphFile);
advPair=(Pair *)malloc(sizeof(Pair));
advPair->first=(void *)advCity;
wgt=(int *)malloc(sizeof(int));
*wgt=weight;
advPair->second=(void *)wgt;
insertIntoAVLTree(advTree,(void *)advPair,&succ);
}
else
{
advPair=(Pair *)malloc(sizeof(Pair));
advPair->first=(void *)advCity;
wgt=(int *)malloc(sizeof(int));
*wgt=weight;
advPair->second=(void *)wgt;
advTree=(AVLTree *)graphPair->second;
insertIntoAVLTree(advTree,(void *)advPair,&succ);
// graph updated , file yet to be updated
graphFile=fopen("graph.dat","w");
graphAvlTreeInOrderIterator=getAVLTreeInOrderIterator(graph,&succ);
while(hasNextInOrderElementInAVLTree(&graphAvlTreeInOrderIterator))
{
gp=(Pair *)getNextInOrderElementFromAVLTree(&graphAvlTreeInOrderIterator,&succ);
gct=(City *)gp->first;
advt=(AVLTree *)gp->second;
fprintf(graphFile,"%d,",gct->code);
advAvlTreeInOrderIterator=getAVLTreeInOrderIterator(advt,&succ);
while(hasNextInOrderElementInAVLTree(&advAvlTreeInOrderIterator))
{
advp=(Pair *)getNextInOrderElementFromAVLTree(&advAvlTreeInOrderIterator,&succ);
advc=(City *)advp->first;
ewgt=(int *)advp->second;
if(hasNextInOrderElementInAVLTree(&advAvlTreeInOrderIterator)) fprintf(graphFile,"%d,%d,",advc->code,*ewgt);
else fprintf(graphFile,"%d,%d#",advc->code,*ewgt);
}
}
fclose(graphFile);
}
} //save part ends
printf("Add more city as adjacent vertex to %s (Y/N) :",name);
m=getchar();
while(getchar()!='\n');
if(m!='y' && m!='Y') break;
}
}

void searchRoute()
{
int distanceComparator(void *left,void *right)
{
Pair *leftPair,*rightPair;
int *leftDistance,*rightDistance;
leftPair=(Pair *)left;
rightPair=(Pair *)right;
leftDistance=(int *)leftPair->second;
rightDistance=(int *)rightPair->second;
return (*leftDistance)-(*rightDistance);
}
typedef struct __path
{
City *city;
City *arrivedFromCity;
int totalDistanceFromStartingSource;
} Path;
int pathComparator(void *left,void *right)
{
Path *leftPath,*rightPath;
leftPath=(Path *)left;
rightPath=(Path *)right;
return leftPath->city->code-rightPath->city->code;
}
Path *pathStructurePointer;
Path pathStructure;
Stack *stack;
AVLTree *pathTree;
char sourceCityName[52],targetCityName[52];
City *sourceCity,*targetCity;
City c;
int succ,routeExists,sum,found,i;
PQueue *pqueue;
Pair *cityTotalDistancePair;
int *totalDistanceFromSourceCity;
City *tmpCity;
City *city;
Pair *graphPair;
Pair *tmpPair;
Pair graphP;
AVLTree *advTree;
AVLTreeInOrderIterator avlTreeInOrderIterator;
Pair *advPair;
int *edgeWeight;
City *advCity;
int *td; 

pathTree=createAVLTree(&succ,pathComparator);
if(!succ)
{
printf("Unable to allocate enough space\n");
return;
}
pqueue=createPQueue(distanceComparator,&succ);
if(!succ) 
{
destroyAVLTree(pathTree);
printf("Unable to allocate enough memory\n");
return ;
}
printf("Enter Source city : ");
fgets(sourceCityName,52,stdin);
if(sourceCityName[strlen(sourceCityName)-1]=='\n') sourceCityName[strlen(sourceCityName) - 1]='\0';
else while(getchar()!='\n');
strcpy(c.name,sourceCityName);
sourceCity=(City*)getFromAVLTree(citiesByName,(void *)&c,&succ);
if(!succ)
{
printf("%s does not exists\n",sourceCityName);
destroyAVLTree(pathTree);
destroyPQueue(pqueue);
return;
}
printf("Enter Destination city : ");
fgets(targetCityName,52,stdin);
if(targetCityName[strlen(targetCityName)-1]=='\n') targetCityName[strlen(targetCityName) - 1]='\0';
else while(getchar()!='\n');
strcpy(c.name,targetCityName);
targetCity=(City*)getFromAVLTree(citiesByName,(void *)&c,&succ);
if(!succ)
{
printf("%s does not exists\n",targetCityName);
destroyAVLTree(pathTree);
destroyPQueue(pqueue);
return;
}
cityTotalDistancePair=(Pair *)malloc(sizeof(Pair));
//null check pending
cityTotalDistancePair->first=(void *)sourceCity;
totalDistanceFromSourceCity=(int *)malloc(sizeof(int));
//null check pending
*totalDistanceFromSourceCity=0;
cityTotalDistancePair->second=(void *)totalDistanceFromSourceCity;
addToPQueue(pqueue,(void*)cityTotalDistancePair,&succ);

//add source city to path and set total distance travelled as zero
pathStructurePointer=(Path *)malloc(sizeof(Path));
pathStructurePointer->city=sourceCity;
pathStructurePointer->totalDistanceFromStartingSource=0;
pathStructurePointer->arrivedFromCity=NULL;
insertIntoAVLTree(pathTree,(void *)pathStructurePointer,&succ);
// what if insertion fails ???
routeExists=0;
while(getSizeOfPQueue(pqueue)>0)
{
cityTotalDistancePair=(Pair *)removeFromPQueue(pqueue,&succ);
city=(City *)cityTotalDistancePair->first;
if(city==targetCity)
{
routeExists=1;
break;
}
totalDistanceFromSourceCity=(int *)cityTotalDistancePair->second;
graphP.first=(void *)city;
graphPair=(Pair *)getFromAVLTree(graph,(void *)&graphP,&succ);
if(!succ) continue;
advTree=(AVLTree *)graphPair->second;
if(getSizeOfAVLTree(advTree)==0) continue;
avlTreeInOrderIterator=getAVLTreeInOrderIterator(advTree,&succ);
while(hasNextInOrderElementInAVLTree(&avlTreeInOrderIterator))
{
advPair=(Pair *)getNextInOrderElementFromAVLTree(&avlTreeInOrderIterator,&succ);
advCity=(City *)advPair->first;
edgeWeight=(int *)advPair->second;
sum=(*totalDistanceFromSourceCity)+(*edgeWeight);
pathStructure.city=advCity;
pathStructurePointer=(Path *)getFromAVLTree(pathTree,(void *)&pathStructure,&succ);
if(succ)
{
if(sum>(pathStructurePointer->totalDistanceFromStartingSource)) continue;
pathStructurePointer->arrivedFromCity=city;
pathStructurePointer->totalDistanceFromStartingSource=sum;
found=false;
for(i=0;i<getSizeOfPQueue(pqueue);i++)
{
tmpPair=(Pair *)getElementFromPQueue(pqueue,i,&succ);
tmpCity=(City *)tmpPair->first;
if(tmpCity->code==advCity->code)
{
found=true;
break;
}
}
if(found)
{
*((int *)tmpPair->second)=sum;
updateElementInPQueue(pqueue,i,(void*)tmpPair,&succ);
continue;
}
cityTotalDistancePair=(Pair *)malloc(sizeof(Pair));
cityTotalDistancePair->first=(void *)advCity;
td=(int *)malloc(sizeof(int));
*td=sum;
cityTotalDistancePair->second=(void *)td;
addToPQueue(pqueue,(void *)cityTotalDistancePair,&succ);
}
else
{
cityTotalDistancePair=(Pair *)malloc(sizeof(Pair));
cityTotalDistancePair->first=(void *)advCity;
td=(int *)malloc(sizeof(int));
*td=sum;
cityTotalDistancePair->second=(void *)td;
addToPQueue(pqueue,(void *)cityTotalDistancePair,&succ);
pathStructurePointer=(Path *)malloc(sizeof(Path));
pathStructurePointer->city=advCity;
pathStructurePointer->arrivedFromCity=city;
pathStructurePointer->totalDistanceFromStartingSource=sum;
insertIntoAVLTree(pathTree,(void *)pathStructurePointer,&succ);
}
} //traversing adjacent vertices loop ends
} //PQueue loops ends
if(routeExists==0)
{
printf("No route exists from %s to %s \n",sourceCity->name,targetCity->name);
//lot of code to release memory
return;
}
stack=createStack(&succ);
// succ false part pending
pathStructure.city=targetCity;
while(true)
{
pathStructurePointer=(Path *)getFromAVLTree(pathTree,(void *)&pathStructure,&succ);
pushOnStack(stack,(void *)pathStructurePointer,&succ);
if(pathStructurePointer->city==sourceCity) break;
pathStructure.city=pathStructurePointer->arrivedFromCity;
}
while(!isStackEmpty(stack))
{
pathStructurePointer=(Path *)popFromStack(stack,&succ);
printf("%s (%d)\n",pathStructurePointer->city->name,pathStructurePointer->totalDistanceFromStartingSource);
}

// lot of memory release left
}


int mainMenu()
{
int ch;
while(1)
{
printf("1.City Master\n");
printf("2.Get Route\n");
printf("3.Exit\n");
printf("enter your choice : ");
scanf("%d",&ch);
while(getchar()!='\n');
if(ch<1 || ch>3) 
{
printf("Invalid input\n");
continue;
}
else
{
return ch;
}
}
}

void cityMenu()
{
int ch;
while(1)
{
printf("City Master \n");
printf("1. ADD\n");
printf("2. EDIT\n");
printf("3. DELETE\n");
printf("4. SEARCH\n");
printf("5. LIST\n");
printf("6. ADD ADJACENT VERTEX\n");
printf("7. EXIT\n");
printf("Enter your choice : ");
scanf("%d",&ch);
while(getchar()!='\n');
if(ch<1 || ch>7) 
{
printf("Invalid input\n");
continue;
}
if(ch==1) addCity();
else if(ch==2) editCity();
else if(ch==3) removeCity();
else if(ch==4) searchCity();
else if(ch==5) displayCities();
else if(ch==6) addAdjacentVertex();
else if(ch==7) return;
}
}

int main()
{
int ch,succ;
populateDataStructures(&succ);
if(!succ) return 0;
while(1)
{
ch=mainMenu();
if(ch==1)
{
cityMenu();
}
if(ch==2)
{
searchRoute();
}
if (ch==3) break;
}
return 0;
}
