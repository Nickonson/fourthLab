#include <iostream>
#include <cstdio>
#include <cstdlib>

using namespace std;

/*
    TO do
        > adding : goto id
*/

struct node
{
    int k;
    char id[256];
    int count;
    node *lch;
    node *rch;
};

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
            if(strcmp(id, root->id) < 0)
            {
                if(!root->lch)
                    break;
                if(strcmp(id, root->lch->id) < 0)
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
            else if(strcmp(id, root->id) > 0)       //key > root->k
            {
                if(!root->rch)
                    break;
                if(strcmp(id, root->rch->id) > 0)   //key > root->rch->k)
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
        p_Node->lch = p_Node->rch = NULL;
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
        }
        if(!root)
        {
            root = p_Node;
            p_Node = NULL;
            return root;
        }
        //balancing
        root = Splay(id, root);
 
        if(strcmp(id, root->id) < 0)        //key < root->k)
        {
            p_Node->lch = root->lch;
            p_Node->rch = root;
            root->lch = NULL;
            root = p_Node;
        }
        else if(strcmp(id, root->id) > 0)   //key > root->k)
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
        if(strcmp(id, root->id) != 0)           // tree is only one node
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
            cout << "id : " << root->id << " ( " << root->count << " ) ";

            if(root->lch)
                cout << " | left child: " << root->lch->id << " ( " << root->lch->count << " ) ";
            if(root->rch)
                cout << " | right child: " << root->rch->id << " ( " << root->rch->count << " ) ";
            cout << "\n";
            InOrder(root->lch);
            InOrder(root->rch);
        }
        return;
    }
};

void nodesFromFile(SplayTree *st, node **p_root, char *rFlName);

void workWithTree(SplayTree *st, node **p_root);

bool ifServWord(char *word);

void clearFile(char *fl_with_coms, char *newFlName);

int main()
{
    printf("_____________________________________________________________________________________________\n");
    SplayTree * st = new SplayTree;
    node * root = new node;
    root = NULL;
    char inpFl[] = "splay/input.cpp";
    char newFl[] = "splay/input_without_comments.cpp";
    
    clearFile(inpFl, newFl);
    nodesFromFile(st, &root, newFl);
    workWithTree(st, &root);

    delete(root);
    delete(st);
    return 0;
}

void nodesFromFile(SplayTree *st, node **p_root, char *rFlName)
{
    node * root = *p_root;
    FILE *rFl = fopen(rFlName, "r");
    char prev, last;
    bool ifQuotes = false;
    char tWord[256] = "\0";
    int i = 0;
    last = getc(rFl);
    while (!feof(rFl))
    {
        if(last == '\"' || last == '\'')
            ifQuotes = !ifQuotes;
        if(!ifQuotes)
        {
            if(last >= 'a' && last <= 'z' || 
                last >= 'A' && last <= 'Z' || last == '_')
            {
                i = 0;
                while(!feof(rFl) && (last >= 'a' && last <= 'z' || 
                    last >= 'A' && last <= 'Z' || last == '_' 
                    || last >= '0' && last <= '9'))
                {
                    tWord[i] = last;
                    i++;
                    prev = last;
                    last = getc(rFl);
                }
                tWord[i] = '\0';
                if(strcmp(tWord, "include") == 0)
                {
                    while(!feof(rFl) && last != '\n')
                    {
                        prev = last;
                        last = getc(rFl);
                    }
                }else if(strcmp(tWord, "enum") == 0)
                {
                    int cnt = 0;
                    while(!feof(rFl) && cnt < 2)
                    {
                        if(last == ')')
                            cnt++;
                        prev = last;
                        last = getc(rFl);
                    }
                }
                else if(!ifServWord(tWord))
                {
                    root = (*st).Insert(tWord, root);
                    cout << "\nAfter Insert: " << tWord << endl;
                    (*st).InOrder(root);
                }
            }
        }
        prev = last;
        last = getc(rFl);    
    }
    *p_root = root;
    fclose(rFl);
    return;
}

void workWithTree(SplayTree *st, node **p_root)
{
    node * root = *p_root;
    printf("_____________________________________________________________________________________________\n");
    (*st).InOrder(root);
    int c = 5;
    char id[256];
    
    while(c != 3)
    {
        //cout << "1. Insert" << endl;
        cout << "1. Delete" << endl;
        cout << "2. Search" << endl;
        cout << "3. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> c;
        switch (c)
        {
        /*
        case 1:
            cout << "Enter value to be inserted: ";
            scanf("%s", id);
            root = (*st).Insert(id, root);
            cout << "\nAfter Insert: " << id << endl;
            (*st).InOrder(root);
            break;
        */
        case 1:
            cout << "Enter value to be deleted: ";
            scanf("%s", id);
            root = (*st).Delete(id, root);
            cout << "\nAfter Delete: " << id << endl;
            (*st).InOrder(root);
            break;
        case 2:
            cout << "Enter value to be searched: ";
            scanf("%s", id);
            root = (*st).Search(id, root);
            cout << "\nAfter Search " << id << endl;
            (*st).InOrder(root);
            break;
        case 3:
            break;
        default:
            cout << "\nInvalid type!\n";
        }
    }
    cout<<"\n";
    return;
}

bool ifServWord(char *word)
{
    bool output = false;
    char tempWord[256];
    FILE *srvFl = fopen("splay/service_words.txt", "r");
    fseek(srvFl, 0, SEEK_SET);
    while(!feof(srvFl) && !output)
    {
        fscanf(srvFl, "%s", tempWord);
        if(strcmp(word, tempWord) == 0)
            output = true;            
    }
    fclose(srvFl);
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
