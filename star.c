#include "star.h"
#include "commands.h"


// Fonction pour créer un nouveau noeud d'arbre
struct TreeNode *createNode(char* data) {
    // Allouer de la mémoire pour le noeud
    struct TreeNode *newNode = malloc(sizeof(struct TreeNode));
    if(newNode == NULL){
        return NULL;
    }

    // Assigner la valeur au champ de données et initialiser les enfants à NULL
    newNode->data = malloc(sizeof(char)*STRLEN);
    if(newNode->data == NULL){
        free(newNode);
        return NULL;
    }
    memset(newNode->data,'\0',sizeof(char)*STRLEN);
    strcpy(newNode->data,data);
    for (int i = 0; i < MAX_CHILDREN; i++) {
        newNode->children[i] = NULL;
    }
    return newNode;
}

// Fonction pour libérer la mémoire utilisée par un arbre
void freeTree(struct TreeNode *root) {
    // Si le noeud est nul, arrêter la récursion
    if (root == NULL) {
        return;
    }

    // Libérer la mémoire utilisée par les enfants du noeud actuel
    for (int i = 0; i < MAX_CHILDREN; i++) {
        freeTree(root->children[i]);
    }

    free(root->data);
    // Libérer la mémoire utilisée par le noeud actuel
    free(root);
}
int get_length_argument_list(char** res){
    int i = 0;
    while(res[i] != NULL){
        i++;
    }
    return i;
}

// Fonction pour parcourir un arbre en profondeur en utilisant l'algorithme DFS (depth-first search)
void traverseDFS(struct TreeNode *root,char** res) {
    // Si le noeud est nul, arrêter la récursion
    if (root == NULL) {
        return;
    }
    int j = 0; 
    while (root->children[j] != NULL){
        traverseDFS(root->children[j],res);
        j++;
    }
    if (j == 0){
        int k = 0;
        while (res[k] != NULL) {
            k++;
        }
        if (strchr(root->data,'*') == NULL){
            char* d;
            if(root->data[0] == '.' && root->data[1] == '/') {
                d =  strdup(root->data+2);
            }
            else {
                d = strdup(root->data);
            }
            int fd_file = open(root->data,O_RDONLY);
            DIR* dir = opendir(root->data);

            if (fd_file != -1) {
                res[k] = d;
            }
            if ( dir != NULL) {
                res[k] = d;
            }
        close(fd_file);
        closedir(dir);
        }
    }
}

int match(char* dir_name,char* pattern){
    int j = 0;
    // Si le premier caractère du pattern est '*', on commence une boucle qui parcourt les caractères du pattern de la fin vers le début
    if(pattern[0] == '*'){
        // Pour chaque caractère du pattern, de la fin vers le début
        int k=strlen(dir_name);
        for(int i=strlen(pattern);i>0;i--){
            // Si k est inférieur à 0 ou si le caractère du pattern à l'index i ne correspond pas au caractère de dir_name à l'index k, on renvoie 0
            if(k<0 || pattern[i] != dir_name[k]){
                return 0;
            }
            k--;
        }
        // Si on a parcouru tous les caractères du pattern sans trouver de différence, on renvoie 1
        return 1;
    }
    //Sinon on parcourt `a partir du début
    for(int i = 0; i < strlen(pattern) ; i++){
        // Si le caractère du pattern à l'index j est '*', on entre dans une boucle qui parcourt les caractères du pattern à partir de j
        if(pattern[j] == '*'){
            // On avance j tant que le caractère du pattern à l'index j est '* pour eliminer toutes la etoiles
            while(pattern[j] == '*'){
                j++;
            }
            // Si on a atteint la fin du pattern, on renvoie 1
            if(pattern[j] == '\0') return 1;
            while(dir_name[i] != '\0'){
                //Appel recursive pour verifier si le reste du pattern correspond au reste du dir_name
                if(match(dir_name+i , pattern+j)) return 1;
                i++;
            }
        }
        if(pattern[j] == dir_name[i] ){j++;}
        else return 0;
    }
    while(pattern[j] == '*')j++;
    return pattern[j] == '\0';
}

void get_match_dirs(char* dir_name, char* pattern,struct TreeNode* root){
    int i =0;
    DIR* dir;
    struct dirent *ent;
    // Si la longueur de dir_name est égale à 0, on concatène "./" à dir_name
    if(strlen(dir_name) == 0) {
        strcat(dir_name,"./");
    }
    // Si dir est un répertoire valide
    if( (dir = opendir(dir_name)) != NULL ){
        // Tant qu'il y a encore un élément à lire dans le répertoire
        while( (ent = readdir(dir)) != NULL ){
            // Si ent->d_name match le pattern, on entre dans la boucle
            if( match(ent->d_name,pattern)){
                if((ent->d_name)[0]=='.' ) continue;
                if (strcmp(dir_name,".") != 0){ // Si le premier caractère de ent->d_name est '.'(demandé dans sujet), on passe à l'itération suivante de la boucle
                    root->children[i] = createNode(dir_name);  // Si dir_name est différent de ".",  on construit un chemin avec dir_name/ent->d_name
                    if(root->children[i] == NULL) goto error;
                    strcat(root->children[i]->data,"/");
                    strcat(root->children[i]->data,ent -> d_name);
                }
                else{
                    root->children[i] = createNode(ent->d_name); //Sinon on construit un chemin avec ent->d_name
                    if(root->children[i] == NULL) goto error;
                }
                i++;
            }
        }
        closedir(dir);
    }
    error:
        return ;
}

int nbStar(char* path){
    //Compter le nombre d'etoiles dans le path à éliminer
    int cpt = 0;
    int i = 0;
    while(path[i] != '\0'){
        if(path[i] == '*') cpt++;
        i++;
    }
    return cpt;
}

int is_directory(char *path)
{
    struct stat statbuf;
    if (stat(path , &statbuf) != 0)
        return 0; 
    return S_ISDIR(statbuf.st_mode);    
}

void match_path(struct TreeNode* root,int nbstar){
    // Si nbstar est égal à 0, on quitte la fonction / condition d'arret
    if (nbstar == 0){
        return;
    }
    char* cpypath = strdup(root->data);
    // Récupère l'adresse du premier '*' dans cpypath et la stocke dans current_star
    char* current_star = strchr(cpypath,'*');
    //eviter le cas de  a////b///c
    while (current_star[0] != '/' ){
        current_star -- ;
    }
    // Crée une copie de la chaîne qui commence à l'adresse stockée dans current_star et qui se termine au prochain '/' et stocke l'adresse de cette copie dans pattern
    char* pattern = strdup( strtok(current_star,"/") );
    int nb_slash =0;
    while(current_star[0] == '/'){
        current_star[0] = '\0';
        current_star--;
        nb_slash++;
    }
    //Trouver tous les répertoires/fichiers qui match le pattern
    get_match_dirs(cpypath,pattern,root);
    int j =0;
    //Ajouter les noeuds fils au noeud courant si il reste des etoiles on appelle la fait un appel recursif
    while(root->children[j] != NULL){
        if ( (strlen(cpypath)+strlen(pattern)) >= strlen(root->data)){
            j++ ;
            continue;
        }
        strcat(root->children[j]->data,root->data+strlen(cpypath)+strlen(pattern)+nb_slash);
        if (nbstar > 1 ){
            match_path(root->children[j],nbstar-1);
        }
        j++;
    }
    free(cpypath);
    free(pattern);
}


char ** get_paths(char * path)
{ 
    struct TreeNode *root = createNode(path);    
    if(root == NULL) goto error;
    if ( root->data[0] !='/'){
        memmove(root->data+2,root->data,strlen(root->data)+1);
        root->data[0] = '.';
        root->data[1] = '/';
    }
    match_path(root,nbStar(root->data));
    char** res = malloc(MAX_CHILDREN*sizeof(char*)) ;
    if(res == NULL){
        goto error;
    }
    for(int i = 0; i < MAX_CHILDREN; i++){
        res[i] = NULL;
    }
    traverseDFS(root,res);
    freeTree(root);

    return res;
    error:
        freeTree(root);
        return NULL;
}

void get_all_nodes(struct TreeNode *root,char* res[MAX_CHILDREN]) {
    // Si le noeud est nul, arrêter la récursion
    if (root == NULL) {
        return;
    }
    int i = 0;
    while(res[i] != NULL){
        i++;
    }
    char* d;
    if(root->data[0] == '.' && root->data[1] == '/') {
        d =  strdup(root->data+3);
    }
    else {
        d = strdup(root->data + 1 );
    }
    res[i] = d;

    for (int j = 0; j < MAX_CHILDREN; j++) {
        get_all_nodes(root->children[j],res);
    }

}
void get_arborescence(char* dir_name,struct TreeNode* root){
    //Recuperer tous les noeuds de l'arborescence et les stocker dans un arbre 
    int i =0;
    DIR* dir;
    struct dirent *ent;
    if(strlen(dir_name) == 0) {
        strcat(dir_name,"./");
    }
    if( (dir = opendir(dir_name)) != NULL ){        
        while( (ent = readdir(dir)) != NULL ){
                if((ent->d_name)[0]=='.' ) continue;
                if (strcmp(dir_name,".") != 0){                    
                    root->children[i] = createNode(dir_name);
                    if(root->children[i] == NULL) goto error;
                    strcat(root->children[i]->data,"/");
                    strcat(root->children[i]->data,ent -> d_name);
                }
                else{
                    root->children[i] = createNode(ent->d_name);
                    if(root->children[i] == NULL) goto error;
                }
                if (is_directory(root->children[i]->data)){
                    struct stat sb;
                    lstat(root->children[i]->data, &sb);
                    if (S_ISLNK(sb.st_mode)) {
                        continue;
                    }
                    get_arborescence(root->children[i]->data,root->children[i]);
                }
                i++;
            }
        closedir(dir);
    }
    return ;
    error:
        return ;
}

char ** get_paths_dbl_star(char * path)
{ 
    //On trouve tous les noeuds de l'arborescence 
    struct TreeNode *root = createNode("");    
    char** arborescence = malloc(MAX_CHILDREN*sizeof(char*));
    char** res = malloc(MAX_CHILDREN*sizeof(char*));
    if(arborescence == NULL || root == NULL || res == NULL){
        goto error;
    }
    get_arborescence(root->data,root);
    for(int i = 0; i < MAX_CHILDREN; i++){
        arborescence[i] = NULL;
    }
    //Recuperer tous les noeuds de l'arborescence des dossiers courant et les stocker dans un tableau
    get_all_nodes(root,arborescence);
    freeTree(root);
    //On supprime le premier element du tableau qui est le dossier courant
    free(arborescence[0]);
    //On remplace le premier element du tableau par le dossier courant
    arborescence[0] = strdup(".");
    for(int i = 0; i < MAX_CHILDREN; i++){
        res[i] = NULL;
    }
    //On supprime les ** u path si le chemin contient **/a//b/c/*
    if (strlen(strstr(path,"**"))> 2) path += 2;
    //Sinon on supprime les ** de path
    else strcpy(path,"");
    char * path_etoile = malloc(MAX_ARGS_STRLEN*sizeof(char));
    //On trouve tous les chemins qui matchent avec le path en utilisant les noeuds de l'arborescence
    for (int i = 0; i < MAX_CHILDREN; i++){
        if (arborescence[i] == NULL) break;
        memset(path_etoile,'\0',MAX_ARGS_STRLEN*sizeof(char));
        strcat(path_etoile,arborescence[i]);
        strcat(path_etoile,path);
        struct TreeNode *root1 = createNode(path_etoile);    
        if(root1 == NULL) goto error;
        match_path(root1,nbStar(root1->data));
        traverseDFS(root1,res);
        freeTree(root1);
    }
    int len = 0;
    while(arborescence[len] != NULL){
        free(arborescence[len]);
        len++;
    }
    free(arborescence);
    free(path_etoile);
    
    return res;
    error:
        if(res != NULL) free(res);
        if (root != NULL) freeTree(root);
        if (res != NULL){
            int l = 0;
            while(arborescence[l] != NULL){
                free(arborescence[l]);
                l++;
            }
            free(arborescence);
        }
        return NULL;
}

int execStar(char* cmd,int return_value){
    //Debut initialisation
    char** files;
    int ret_val = 1;
    int i = 0;
    char cpycmd[MAX_ARGS_STRLEN] ;
    strcpy(cpycmd,cmd);
    char* argument_list[MAX_ARGS_NUMBER];
    char* res = strtok(cpycmd," ");
    argument_list[i] = res;
    //On recupere tous les arguments de la commande on splitant sur les espaces
    while(res!=NULL){
        i++;
        res = strtok(NULL," ");
        argument_list[i] = res;
    }
    char* argument_list_no_star[MAX_ARGS_NUMBER];
    for(int j = 0; j < MAX_ARGS_NUMBER; j++){
        argument_list_no_star[j] = NULL;
    }
    int index_arg = 0;
    int nb_match = 0;
    //Fin initialisation
    //pour chaque partie de la commande on regarde si elle contient un * ou ** et on remplace par les chemins correspondants
    //Sinon on ajoute la partie de la commande au tableau argument_list_no_star
    for (int j=0 ; j<i; j++){
        if (strchr(argument_list[j],'*') != NULL) {
            if (strstr(argument_list[j],"**") != NULL){
                files = get_paths_dbl_star(argument_list[j]);
            }
            else {files = get_paths(argument_list[j]);}

            if (files == NULL) goto error;
            memmove(argument_list_no_star+index_arg,files, sizeof(char*) * (get_length_argument_list(files)));
            index_arg += get_length_argument_list(files);
            nb_match += get_length_argument_list(files);
            free(files);
            }
        else {
            argument_list_no_star[index_arg] = strdup(argument_list[j]);
            index_arg++;
        }
    }
    char * cmd_sans_star = malloc(MAX_ARGS_STRLEN*sizeof(char));
    if(cmd_sans_star== NULL){
        goto error;
    }
    memset(cmd_sans_star,'\0',MAX_ARGS_STRLEN*sizeof(char));
    //Si il y a aucun chemin qui match on execute la commande sans les * et ** on execute la commande comme elle etait
    if(nb_match == 0){
        ret_val = execute(cmd,return_value,0);
    }
    //Sinon on execute la commande avec tous les chemins qui match on construisant la commande avec tous tout les chemins qui match
    else{
        int e = 0;
        while(argument_list_no_star[e] != NULL){
            strcat(cmd_sans_star,argument_list_no_star[e]);
            strcat(cmd_sans_star," ");
            e++;
        }
        ret_val = execute(cmd_sans_star,return_value,0);
    }

    i =0;
    while(argument_list_no_star[i] != NULL){
        free(argument_list_no_star[i]);
        i++;
    }
    free(cmd_sans_star);
    return ret_val;
    error:
        i =0;
        while(argument_list_no_star[i] != NULL){
            free(argument_list_no_star[i]);
            i++;
        }
        free(cmd_sans_star);
        return -1;
}
