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

class SplayTree
{
    public:  
    node *RR_ROTATE(node *Node1)
    {
        node *Node2 = Node1->lch;
        Node1->lch = Node2->rch;
        Node2->rch = Node1;
        return Node2;       
    }
    node *LL_ROTATE(node *Node1)
    {
        node *Node2 = Node1->rch;
        Node1->rch = Node2->lch;
        Node2->lch = Node1;
        return Node2;
    }
    node *Splay(int key, node *root)
    {
        if(!root)
            return NULL;
        
        node header;
        header.lch = header.rch = NULL;

        node *LeftTreeMax = &header;
        node *RightTreeMin = &header;
        
        while(1)
        {
            if(key < root->k)
            {
                if(!root->lch)
                    break;
                if(key < root->lch->k)
                {
                    root = RR_ROTATE(root);
                    if(!root->lch)
                        break;
                }
                RightTreeMin = root;
                root = root->lch;
                RightTreeMin->lch = NULL;
            }
            else if(key > root->k)
            {
                if(!root->rch)
                    break;
                if(key > root->rch->k)
                {
                    root = LL_ROTATE(root);
                    if(!root->rch)
                        break;
                }
                LeftTreeMax = root;
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
    node *New_Node(char identifier[256])
    {
        node *p_Node = new node;
        p_Node->count = 1;
        strcpy(p_Node->id, identifier);
        if(!p_Node)
        {
            printf("\nOut of memory!\n");
            exit(1);
        }
        p_Node->k = 0;
        p_Node->lch = p_Node->rch = NULL;
        return p_Node;
    }
    node *Insert(int key, node *root, char identifier[256])
    {
        static node *p_Node = NULL;
        if(!p_Node)
            p_Node = New_Node(identifier);
        else
            p_Node->k = key;
        
        if(!root)
        {
            root = p_Node;
            //k == 0 as default
            p_Node = NULL;
            return root;
        }
        root = Splay(key, root);
        if(key < root->k)
        {
            p_Node->lch = root->lch;
            p_Node->rch = root;
            root->lch = NULL;
            root = p_Node;
        }
        else if(key > root->k)
        {
            p_Node->rch = root->rch;
            p_Node->lch = root;
            root->rch = NULL;
            root = p_Node;
        }
        else
            return root;
        p_Node = NULL;
        return root;
    }
    node *Delete(int key, node *root)
    {
        node *temp;
        if(!root)
            return NULL;
        
        root = Splay(key, root);
        if(key != root->k) // tree is only one node
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
                root = Splay(key, root->lch);
                root->rch = temp->rch;
            }
            free(temp);
            return root;
        }
    }
    node *Search(int key, node *root)
    {
        return Splay(key, root);
    }
    void InOrder(node *root)
    {
        if(root)
        {
            InOrder(root->lch);
            cout << "key : " << root->k;

            if(root->lch)
                cout << " | left child: " << root->lch->k;
            if(root->rch)
                cout << " | right child: " << root->rch->k;
            cout << "\n";
            InOrder(root->rch);
        }
    }
};

void workWithTree();

void addNewElem(char *varName);

bool ifServWord(char *word);

bool ifType(char *word);

void clearFile(char *newFlName, FILE *file_with_comments);

int STRCMP(const char *s1, const char *s2);

void warning256()
{
    printf("fock out of 256 chars");
}

int main()
{
    SplayTree sp;
    node *root;
    root = NULL;

    char newName[] = "splay/input_without_comments.cpp";
    FILE *rFl = fopen("splay/input.cpp", "r");
    clearFile(newName, rFl);
    fclose(rFl);

    rFl = fopen(newName, "r");

    char prev, last;
    last = getc(rFl);

    char tWord[256] = "\0";
    bool ifQuotes = false;
    enum REGIMES regime = NONE;
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
                sp.Insert(, root, tWord);
            }
            
        }
        else
            last = getc(rFl);
    }
	

    fclose(rFl);
    
    return 0;
}

void workWithTree()
{
    SplayTree st;
    node *root;
    root = NULL;
    st.InOrder(root);
    int i, c;
    char identifier[256];
    scanf("%s", identifier);
    printf("%s\n", identifier);
    scanf("%s", identifier);
    printf("%s\n", identifier);
    
    while(1)
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
            scanf("%s", identifier);
            root = st.Insert(i, root, identifier);
            cout << "\nAfter Insert: " << i << endl;
            st.InOrder(root);
            break;
        case 2:
            cout << "Enter value to be deleted: ";
            cin >> i;
            root = st.Delete(i, root);
            cout << "\nAfter Delete: " << i << endl;
            st.InOrder(root);
            break;
        case 3:
            cout << "Enter value to be searched: ";
            cin >> i;
            root = st.Search(i, root);
            cout << "\nAfter Search " << i << endl;
            st.InOrder(root);
            break;
        case 4:
            exit(1);
        default:
            cout << "\nInvalid type!\n";
        }
    }
    
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

void clearFile(char *newFlName, FILE *file_with_comments)
{
    char prev, last;

    FILE *new_file = fopen(newFlName, "w");

    last = getc(file_with_comments);
    while (!feof(file_with_comments))
    {
        prev = last;
        last = getc(file_with_comments);
        if (prev == '\"')
        {
            fputc(prev, new_file);
            do
            {
                if (last == '\\')
                {
                    fputc(last, new_file);
                    last = getc(file_with_comments);
                    fputc(last, new_file);
                    last = getc(file_with_comments);
                }
                prev = last;
                last = getc(file_with_comments);
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
                    last = getc(file_with_comments);
                    fputc(last, new_file);
                    last = getc(file_with_comments);
                }
                prev = last;
                last = getc(file_with_comments);
                fputc(prev, new_file);

            } while (prev != '\'');
        }
        else if (prev == '/')
        {
            if (last == '/')
            {
                while (prev != '\n' && !feof(file_with_comments))
                {
                    prev = last;
                    last = getc(file_with_comments);
                    if (last == '\n')
                    {
                        
                        if (prev == '\\')
                        {
                            prev = getc(file_with_comments);
                            last = getc(file_with_comments);
                        }
                        fputc('\n', new_file);
                    }
                }
            }
            else if (last == '*')
            {
                prev = getc(file_with_comments);
                last = getc(file_with_comments);

                while (!(prev == '*' && last == '/') && !feof(file_with_comments))
                {
                    prev = last;
                    last = getc(file_with_comments);
                }
                last = getc(file_with_comments);
            }
            else
                fputc(prev, new_file);
        }
        else
            fputc(prev, new_file);
    }
    fclose(file_with_comments);
    fclose(new_file);
}

int STRCMP(const char *s1, const char *s2)
{
    int i = 0;
    while(s1[i] != '\0' && s2[i] != '\0' && s1[i] == s2[i])
        i++;
    
    if(s1[i] != '\0' || s2[i] != '\0')
        return 0;
    else if(s1[i] > s2[i])
        return 1;
    else 
        return -1;
}