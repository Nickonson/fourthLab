#include <iostream>
#include <cstdio>
#include <cstdlib>

using namespace std;


/*

    TO do
            > ins, del, ex menu doesnt work fully cuz of IDENTIFIER (right menu)
            >? adding identifier such as: $vars, goto id, $funcs id
            > mechanic of working with nodes key

*/

enum REGIMES{NONE = 0, DECLARING, ATTRIBUTE, CALL};

struct node
{
    int k;
    char id[256];
    int count;
    node *lch;
    node *rch;
};

int STRCMP(const char *s1, const char *s2);

class SplayTree
{
    public:  
    node *RR_ROTATE(node * Node1)
    {
        node *Node2 = Node1->lch;
        Node1->lch = Node2->rch;
        Node2->rch = Node1;
        return Node2;       
    }
    node *LL_ROTATE(node * Node1)
    {
        node *Node2 = Node1->rch;
        Node1->rch = Node2->lch;
        Node2->lch = Node1;
        return Node2;
    }
    node *Splay(char id[256], node *root)
    {
        if(!root)
            return NULL;
        
        node header;
        header.lch = header.rch = NULL;

        node *LeftTreeMax = &header;
        node *RightTreeMin = &header;
        
        while(1)
        {
            if(STRCMP(id, root->id) < 0)            //key < root->k)
            {
                if(!root->lch)
                    break;
                if(STRCMP(id, root->lch->id) < 0)   //key < root->lch->k)
                {
                    root = RR_ROTATE(root);
                    if(!root->lch)
                        break;
                }
                RightTreeMin->lch= root;
                RightTreeMin = RightTreeMin->lch;
                root = root->lch;
                RightTreeMin->lch = NULL;
            }
            else if(STRCMP(id, root->id) > 0)       //key > root->k
            {
                if(!root->rch)
                    break;
                if(STRCMP(id, root->lch->id) > 0)   //key > root->rch->k)
                {
                    root = LL_ROTATE(root);
                    if(!root->rch)
                        break;
                }
                LeftTreeMax->rch= root;
                LeftTreeMax = LeftTreeMax->rch;
                root = root->rch;
                LeftTreeMax->rch = NULL;
            }
            else
                break;
        }
        LeftTreeMax->rch = root->lch;
        RightTreeMin->lch = root->rch;
        root->lch = header.rch;
        root->rch = header.lch;
        return root;
    }
    node *New_Node(char id[256])
    {
        node * p_Node = new node;
        p_Node->count = 1;
        strcpy(p_Node->id, id);
        if(!p_Node)
        {
            printf("\nOut of memory!\n");
            exit(1);
        }
        p_Node->lch = p_Node->rch = NULL;       //p_Node->k = key;
        return p_Node;
    }
    node *Insert(char id[256], node *root)
    {
        static node *p_Node = NULL;
        if(!p_Node)
            p_Node = New_Node(id);
        else
        {
            p_Node->count = 1;
            strcpy(p_Node->id, id);
            //p_Node->k = key;
        }
        if(!root)
        {
            root = p_Node;
            //root->k = 0;
            p_Node = NULL;
            return root;
        }
        //balancing
        root = Splay(id, root);
 
        if(STRCMP(id, root->id) < 0)        //key < root->k)
        {
            p_Node->lch = root->lch;
            p_Node->rch = root;
            root->lch = NULL;
            root = p_Node;
        }
        else if(STRCMP(id, root->id) > 0)   //key > root->k)
        {
            p_Node->rch = root->rch;
            p_Node->lch = root;
            root->rch = NULL;
            root = p_Node;
        }
        else
        {
            root->count++;
            return root;
        }
        p_Node = NULL;
        return root;
    }
    node *Delete(char id[256], node *root)
    {
        node *temp;
        if(!root)
            return NULL;
        
        root = Splay(id, root);
        if(STRCMP(id, root->id) != 0)           // tree is only one node
            return root;
        else
        {
            if(!root->lch)
            {
                temp = root;
                root = root->rch;
            }
            else
            {
                temp = root;
                root = Splay(id, root->lch);
                root->rch = temp->rch;
            }
            free(temp);
            return root;
        }
    }
    node *Search(char id[256], node *root)
    {
        return Splay(id, root);
    }
    void InOrder(node *root)
    {
        if(root)
        {
            InOrder(root->lch);
            cout << "key : " << root->k;

            if(root->lch)
                cout << " | left child: " << root->lch->id << " ( " << root->lch->count << " ) - quantity";
            if(root->rch)
                cout << " | right child: " << root->rch->id << " ( " << root->lch->count << " ) - quantity";
            cout << "\n";
            InOrder(root->rch);
        }
        return;
    }
};

void nodesFromFile(SplayTree *st, node *root, char *rFlName);

void workWithTree(SplayTree *st, node *root);

void addNewElem(char *varName);

bool ifServWord(char *word);

bool ifType(char *word);

void clearFile(char *fl_with_coms, char *newFlName);


void warning256()
{
    printf("out of 256 chars");
}



int main()
{
    
    SplayTree * st = new SplayTree;
    node * root;
    root = NULL;
    
    char inpFl[] = "splay/input.cpp";
    char newFl[] = "splay/input_without_comments.cpp";
    clearFile(inpFl, newFl);
    
    nodesFromFile(st,root, newFl);

    workWithTree(st, root);
    delete(root);
    delete(st);
    return 0;
}

int STRCMP(const char *s1, const char *s2)
{
    int i = 0;
    while(s1[i] != '\0' && s2[i] != '\0' && s1[i] == s2[i])
        i++;
    
    if(s1[i] == '\0' && s2[i] == '\0')
        return 0;
    else if(s1[i] > s2[i])
        return 1;
    return -1;
}

void nodesFromFile(SplayTree *st, node *root, char *rFlName)
{
    FILE *rFl = fopen(rFlName, "r");
    char prev, last;
    bool ifQuotes = false;
    enum REGIMES regime = NONE;
    char tWord[256] = "\0";

    last = getc(rFl);
    
    while (!feof(rFl))
    {
        if(last == '"'){
            last = getc(rFl);
            while(last != '"')
                last = getc(rFl);
            last = getc(rFl);
        }
        else if(last == '\''){
            last = getc(rFl);
            while(last != '\'')
                last = getc(rFl);
            last = getc(rFl);
        }
        else if(!ifQuotes && (last >= 'a' && last <= 'z' && last >= 'A' && last <= 'Z' || last == '_'))
        {
            tWord[0] = '\0';
            int t = 0;
            while(t < 256 && !feof(rFl) && (
                last >= 'a' && last <= 'z' && 
                last >= 'A' && last <= 'Z' &&
                last >= '0' && last <= '9' || last == '_'))
            {
                tWord[t] = last;
                t++;
                last = getc(rFl);
            }
            tWord[t] = '\0';

            if(ifType(tWord))
                if(!STRCMP(tWord, "enum") || !STRCMP(tWord, "union") || !STRCMP(tWord, "struct"))
                {
                    regime = ATTRIBUTE;
                    last = getc(rFl);
                    while(last == ' ')
                        last = getc(rFl);
                    last = getc(rFl);
                    if(last == '_')
                    {
                        t = 0;
                        prev = last;
                        while(!feof(rFl) && (last != ' ' || prev != ')'))
                        {
                            tWord[t] = last;
                            prev = last;
                            last = getc(rFl);
                            t++;
                        }
                        tWord[t] = '\0';
                        if(STRCMP(tWord, "__attribute__(") == 0 && last == ' ')
                        {
                            last = getc(rFl);
                            while(last != '{')
                            {
                                tWord[t] = last;
                                last = getc(rFl);
                                t++;
                            }
                            tWord[t] = '\0';
                        }
                    }
                    else
                    {
                        t = 0;
                        if(last == ' ')
                            while(last == ' ')
                                last = getc(rFl);
                        while(t < 256 && last != '{')
                        {
                            tWord[t] = last;
                            last = getc(rFl);
                            t++;
                        }
                        if(t > 255)warning256();
                        tWord[t] = '\0';
                    }
                    //tword - new declared name
                }
                else
                {
                    regime = DECLARING;
                    if(!STRCMP(tWord, "long") || !STRCMP(tWord, "short"))
                    {
                        while(!feof(rFl) && !(
                            last >= 'a' && last <= 'z' && 
                            last >= 'A' && last <= 'Z' ||
                            last == '_'))
                        {
                            last = getc(rFl);
                        }
                        t = 0;
                        while(t < 256 && !feof(rFl) && (
                            last >= 'a' && last <= 'z' && 
                            last >= 'A' && last <= 'Z' &&
                            last >= '0' && last <= '9' || last == '_'))
                        {
                            tWord[t] = last;
                            last = getc(rFl);
                            t++;
                        }
                        tWord[t] = '\0';
                        if(!STRCMP(tWord, "long ") || !STRCMP(tWord, "short "))
                        {
                            last = getc(rFl);
                            t = 0;
                            while(t < 256 && !feof(rFl) && (
                                last >= 'a' && last <= 'z' && 
                                last >= 'A' && last <= 'Z' &&
                                last >= '0' && last <= '9' || last == '_'))
                            {
                                tWord[t] = last;
                                last = getc(rFl);
                                t++;
                            }
                        }
                    }
                    else
                    {
                        while(!feof(rFl) && !(
                            last >= 'a' && last <= 'z' && 
                            last >= 'A' && last <= 'Z' ||
                            last == '_'))
                        {
                            last = getc(rFl);
                        }
                        t = 0;
                        while(t < 256 && !feof(rFl) && (
                            last >= 'a' && last <= 'z' && 
                            last >= 'A' && last <= 'Z' ||
                            last == '_'))
                        {
                            tWord[t] = last;
                            last = getc(rFl);
                            t++;
                        }
                        tWord[t] = '\0';
                    }
                }
            else if(ifServWord(tWord))
                regime = NONE;
            else
                regime = CALL;

            
            if(regime != NONE)
            {
//                (*st).Insert();
            }
            
        }
        else
            last = getc(rFl);
    }
	

    fclose(rFl);
    return;
}

void workWithTree(SplayTree *st, node *root)
{
    root = NULL;
    (*st).InOrder(root);
    int i, c = 5;
    char id[256];
    
    while(c != 4)
    {
        cout << "1. Insert" << endl;
        cout << "2. Delete" << endl;
        cout << "3. Search" << endl;
        cout << "4. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> c;
        switch (c)
        {
        case 1:
            cout << "Enter value to be inserted: ";
            cin >> i;
            //scanf("%s", identifier);
            //root = st.Insert(root, identifier);
            root = (*st).Insert(id, root);
            cout << "\nAfter Insert: " << i << endl;
            (*st).InOrder(root);
            break;
        case 2:
            cout << "Enter value to be deleted: ";
            cin >> i;
            //root = st.Delete(root, identifier);
            root = (*st).Delete(id, root);
            cout << "\nAfter Delete: " << i << endl;
            (*st).InOrder(root);
            break;
        case 3:
            cout << "Enter value to be searched: ";
            cin >> i;
            //root = (*st).Search(root, identifier);
            root = (*st).Search(id, root);
            cout << "\nAfter Search " << i << endl;
            (*st).InOrder(root);
            break;
        case 4:
            break;
        default:
            cout << "\nInvalid type!\n";
        }
    }
    cout<<"\n";
    return;
}

void addNewElem(char *varName)
{

}

bool ifServWord(char *word)
{
    bool output = false;
    char tempWord[256];

    FILE *srvFl = fopen("splay/service_words.txt", "r");

    while(!feof(srvFl) && !output)
    {
        fscanf(srvFl, "%s", tempWord);
        if(strcmp(word, tempWord) == 0)
            output = true;
    }

    fclose(srvFl);
    return output;
}

bool ifType(char *word)
{
    bool output = false;
    char tempWord[256];
    FILE *tpFl = fopen("splay/data_types.txt", "r");

    while(!feof(tpFl) && !output)
    {
        fscanf(tpFl, "%s", tempWord);
        if(strcmp(word, tempWord) == 0)
            output = true;
    }

    fclose(tpFl);
    return output;
}

void clearFile(char *flWithComs, char *newFlName)
{
    char prev, last;

    FILE *fl_with_coms = fopen(flWithComs, "r");
    FILE *new_file = fopen(newFlName, "w");

    last = getc(fl_with_coms);
    while (!feof(fl_with_coms))
    {
        prev = last;
        last = getc(fl_with_coms);
        if (prev == '\"')
        {
            fputc(prev, new_file);
            do
            {
                if (last == '\\')
                {
                    fputc(last, new_file);
                    last = getc(fl_with_coms);
                    fputc(last, new_file);
                    last = getc(fl_with_coms);
                }
                prev = last;
                last = getc(fl_with_coms);
                fputc(prev, new_file);

            } while (prev != '\"');
        }
        else if (prev == '\'')
        {
            fputc(prev, new_file);
            do
            {
                if (last == '\\')
                {
                    fputc(last, new_file);
                    last = getc(fl_with_coms);
                    fputc(last, new_file);
                    last = getc(fl_with_coms);
                }
                prev = last;
                last = getc(fl_with_coms);
                fputc(prev, new_file);

            } while (prev != '\'');
        }
        else if (prev == '/')
        {
            if (last == '/')
            {
                while (prev != '\n' && !feof(fl_with_coms))
                {
                    prev = last;
                    last = getc(fl_with_coms);
                    if (last == '\n')
                    {
                        
                        if (prev == '\\')
                        {
                            prev = getc(fl_with_coms);
                            last = getc(fl_with_coms);
                        }
                        fputc(' ', new_file);
                        
                        //fputc('\n', new_file);
                    }
                }
            }
            else if (last == '*')
            {
                prev = getc(fl_with_coms);
                last = getc(fl_with_coms);

                while (!(prev == '*' && last == '/') && !feof(fl_with_coms))
                {
                    prev = last;
                    last = getc(fl_with_coms);
                }
                last = getc(fl_with_coms);
            }
            else
                fputc(prev, new_file);
        }
        else
            fputc(prev, new_file);
    }
    fclose(fl_with_coms);
    fclose(new_file);
    return;
}
