#include <iostream>
#include <cstdio>
#include <cstdlib>

using namespace std;

struct node
{
   int k;
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
    node *New_Node(int key)
    {
        node *p_Node = new node;
        if(!p_Node)
        {
            printf("\nOut of memory!\n");
            exit(1);
        }
        p_Node->k = key;
        p_Node->lch = p_Node->rch = NULL;
        return p_Node;
    }
    node *Insert(int key, node *root)
    {
        static node *p_Node = NULL;
        if(!p_Node)
            p_Node = New_Node(key);
        else
            p_Node->k = key;
        
        if(!root)
        {
            root = p_Node;
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

int main()
{
    SplayTree st;
    node *root;
    root = NULL;
    st.InOrder(root);
    int i, c;

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
            root = st.Insert(i, root);
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
    return 0;
}