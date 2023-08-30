/*
Code by 
Govinda Rohith Y
CS21BTECH11062
Assignment-9 
Implementation of B-Trees
*/
// 1.Each node contains the Age of a faculty. 
// 2.You should implement  
// A.Insert
// B.Search
// C.Delete 
// 3.Take the value of n as five
//Working for N>=3 and N is ODD
#include<iostream>
#include<stdlib.h>
using namespace std;
int N=5; //max no of keys per node only ODD values >=3 are allowed
class Node
{
    //class to store each node in a tree
    public:
    int no;
    // current no of keys in Node
    int *keys; //array of keys
    Node **child; //array of child nodes
    Node *parent; //parent NODE
};  
class Root
{
    //class to store Root node
    public :
    Node *start;
};
Root act(Root r,Node *temp,int age,Node *left,Node *right);
void printer(Node *temp)
{
    //function to traverse the tree
    int i;
    if(temp!=NULL) 
    {
    for(i=0;i<temp->no;i++)
    {
        if(temp->child[i]!=NULL)
        {
            printer(temp->child[i]);
        }
        cout<<temp->keys[i]<<" ";
    }
    if(temp->child[i]!=NULL)
    {
        printer(temp->child[i]);
    }
    }
    else
    {
        //case when tree is empty
        cout<<"Empty tree try to insert some data";
    }
}
Node * creator()
{
    //function to crate a new node and returns it
    Node *entry=new Node();
    int *arr,i;
    arr=(int *)malloc(sizeof(int)*N);
    Node **child;
    child=(Node **)malloc(sizeof(Node *)*(N+1));
    entry->keys=arr;
    entry->child=child;
    for(i=0;i<N;i++)
    {
        entry->keys[i]=0;
        entry->child[i]=NULL;
    }
    entry->child[i]=NULL;
    return entry;
}
void destroyer(Node *temp)
{
    //function to free allocated memory for each node
    free(temp->keys);
    free(temp->child);
    delete temp;
}
void free_mem(Node *temp)
{
    int i;
    if(temp!=NULL)
    {
    free(temp->keys);
    for(i=0;i<temp->no;i++)
    {
        if(temp->child[i]!=NULL)
        {
            free_mem(temp->child[i]);
        }
    }
    if(temp->child[i]!=NULL)
    {
        free_mem(temp->child[i]);
    }
    free(temp->child);
    delete temp;
    }
}
Root ins_spilt(Root r,Node *temp)
{
    //function to split node according to alogorithm
    //0 1       2         3 4  
    //left taken by temp and right taken by entry
    Node *entry,*parent=temp->parent;
    int i,fel=temp->keys[(N-1)/2];
    entry=creator();
    for(i=0;i<(N-1)/2;i++)
    {
        entry->keys[i]=temp->keys[(N+1)/2+i];
        entry->child[i]=temp->child[(N+1)/2+i];
    }
    entry->child[i]=temp->child[(N+1)/2+i];
    entry->no=(N-1)/2;
    for(i=(N-1)/2+1;i<N;i++)
    {
        temp->child[i]=NULL;
        temp->keys[i]=0;
    }
    temp->child[i]=NULL;
    temp->keys[(N-1)/2]=0;
    temp->no=(N-1)/2;
    entry->parent=parent; //mistake here
    temp->parent=parent;
    for(i=0;i<=entry->no;i++)
    {
        if(entry->child[i]!=NULL) entry->child[i]->parent=entry;
        else break;
    }
    return act(r,parent,fel,temp,entry);
}
Root act(Root r,Node *temp,int age,Node *left,Node *right)
{
    //function to seperates to in each case for insertion 
    int i,j;
    //temp is a node which have to insert age
    //left is left tree of age 
    if(temp==NULL)
    {
        Node *entry;
        entry=creator();
        entry->no=1;
        entry->keys[0]=age;
        entry->child[0]=left;
        entry->child[1]=right;
        if(left!=NULL)entry->child[0]->parent=entry;
        if(right!=NULL)entry->child[1]->parent=entry;
        r.start=entry;
        return r;
    }
    else
    {
    if(age<=temp->keys[0])
    {
        for(j=temp->no;j>0;j=j-1)
        {
            temp->keys[j]=temp->keys[j-1];
            temp->child[j+1]=temp->child[j];
        }
        temp->child[j]=left;
        temp->child[j+1]=right;
        temp->keys[j]=age;
        if(left!=NULL)temp->child[temp->no]->parent=temp;
        if(right!=NULL)temp->child[temp->no+1]->parent=temp;
    }
    else if(age>=temp->keys[temp->no-1])
    {
        temp->keys[temp->no]=age;
        temp->child[temp->no]=left;
        temp->child[temp->no+1]=right;
        if(left!=NULL)temp->child[temp->no]->parent=temp;
        if(right!=NULL)temp->child[temp->no+1]->parent=temp;
    }
    else
    {
    for(i=0;i<temp->no-1;i++)
    {
        if(age>=temp->keys[i]&&age<=temp->keys[i+1])
        {
            for(j=temp->no;j>i+1;j=j-1)
            {
                temp->keys[j]=temp->keys[j-1];
                temp->child[j+1]=temp->child[j];
            }
            temp->child[j]=left;
            temp->child[j+1]=right;
            temp->keys[j]=age;
            break;
        }
    }
    }
    temp->no++;
    if(temp->no==N) return ins_spilt(r,temp);
    return r;
    }
}
Root insert_a(Root r,int age)
{
    //function to find to node for which the given age needs to be inserting
    Node *temp=r.start;
    int i;
    if(temp==NULL)
    {
        Node *entry;
        entry=creator();
        entry->keys[0]=age;
        r.start=entry;
        entry->no=1;
        return r;
    }
    //finds node in which the age should be inserted
    while (temp!=NULL)
    {
        if(age<temp->keys[0])
        {
            if(temp->child[0]!=NULL) temp=temp->child[0];
            else break;
        }
        if(age>temp->keys[temp->no-1])
        {
            if(temp->child[temp->no]!=NULL) temp=temp->child[temp->no];
            else break;
        }
        for(i=0;i<temp->no-1;i++)
        {
            if(age>=temp->keys[i]&&age<=temp->keys[i+1])
            {    
                if(temp->child[i+1]!=NULL) temp=temp->child[i+1];
                else 
                {
                    return act(r,temp,age,NULL,NULL);
                }
            }
        }         
    }
    cout<<"***  Inserted Successfully ***"<<endl;
    return act(r,temp,age,NULL,NULL);
}
bool search_b(Root r,int age)
{
    //function returns true if there exists a node with given age
    //else returns false
    Node *temp=r.start;
    int i;
    if(temp==NULL)
    {
        cout<<"Empty tree try to insert some data"<<endl;
        return false;
    }
    while(temp!=NULL)
    {
        if(age<temp->keys[0]) temp=temp->child[0];
        if(age>temp->keys[temp->no-1]) temp=temp->child[temp->no];
        else
        {
            for(i=0;i<temp->no;i++)
            {
                if(age==temp->keys[i])
                {
                    cout<<"Found entry with age  "<<age<<endl;
                    return true;
                } 
                if(age>temp->keys[i]&&age<temp->keys[i+1])
                {
                    temp=temp->child[i+1];
                    break;
                }
            }
        }
    }
    cout<<"Not found entry with given age"<<endl;
    return false;
}
Node * insucc(Node *temp,int index)
{
    //returns NULL if no inorder successor is present
    //else returns its inorder successor
    //index represents the index of age in temp node
    Node *req=NULL;
    temp=temp->child[index+1];
    while(temp!=NULL)
    {
        req=temp;
        temp=temp->child[0];
    }
    return req;
}
Node * inpre(Node *temp,int index)
{
    //returns NULL if no inorder predecessor is present
    //else returns its inorder predecessor
    //index represents the index of age in temp node
    Node *req=NULL;
    temp=temp->child[index];
    while(temp!=NULL)
    {
        req=temp;
        temp=temp->child[temp->no];
    }
    return req;
}
Node * left_founder(Node *temp)
{
    //returns left sibling of a given node temp
    int i;
    Node *parent=temp->parent;
    if(parent==NULL) return NULL;
    for(i=0;i<=parent->no;i++)
    {
        if(parent->child[i]==temp) break;
    }
    if(i==0) return NULL;
    else return parent->child[i-1];
}
Node * right_founder(Node *temp)
{
    //returns left sibling of a given node temp
    int i;
    Node *parent=temp->parent;
    if(parent==NULL) return NULL;
    for(i=0;i<=parent->no;i++)
    {
        if(parent->child[i]==temp) break;
    }
    if(i==parent->no)   return NULL;
    else return parent->child[i+1];
}
Root merge(Root r,Node *left,Node *temp,Node *right)
{
    //function to merge the nodes with less than min number of keys
    //untill the B-Trees properties satisfies
    //index=-1 just merge no need for delete
    //any other delete element in that index of temp
    //     parent
    // left and right  and merge untill proper trees arrives
    //      parent
    //  fir        sec  --> fir parent sec 
    Node *parent=temp->parent;
    int i; 
    int conc=0,iso;
    if(left!=NULL)
    {
        for(i=0;i<parent->no;i++)
        {
            if(parent->child[i]==left&&parent->child[i+1]==temp) break;
        }
        conc=i;
        iso=parent->keys[conc];
        for(i=conc;i<parent->no-1;i++)
        {
            parent->keys[i]=parent->keys[i+1];
            parent->child[i+1]=parent->child[i+2];
        }
        parent->keys[i]=0; 
        parent->child[i+1]=NULL;
        parent->no=parent->no-1;
        //parent assignment done here
        left->keys[left->no]=iso;
        left->no++;
        for(i=0;i<temp->no;i++)
        {
            left->keys[i+left->no]=temp->keys[i];
            left->child[i+left->no]=temp->child[i];
            if(left->child[i+left->no]!=NULL)left->child[i+left->no]->parent=left;
        }
        left->child[i+left->no]=temp->child[i];
        if(left->child[i+left->no]!=NULL)left->child[i+left->no]->parent=left;
        left->no=left->no+temp->no;
        //left totally assigned complete!!!
        destroyer(temp);
        if(parent->no==0&&parent->parent==NULL)
        {
            r.start=left;
            left->parent=NULL;
            destroyer(parent);
            return r;
        } 
    }
    else if(right!=NULL)
    {
        return merge(r,temp,right,right_founder(right));
    }
    else
    {
        //both left and right are NULL
        for(i=0;i<=parent->no;i++)
        {
            if(parent->child[i]==temp) break;
        }
        conc=i;
        temp->keys[temp->no]=parent->keys[i];
        temp->no++;
        for(i=conc;i<parent->no-1;i++)
        {
            parent->keys[i]=parent->keys[i+1];
            parent->child[i+1]=parent->child[i+2];
        }
        parent->no=parent->no-1;
        if(parent->no==0)
        {
            destroyer(parent);
            r.start=temp;
            temp->parent=NULL;
            return r;
        }
    }
    if(parent->no<(N-1)/2)
    {
        int nleft=0,nright=0;
        left=left_founder(parent),right=right_founder(parent);
        temp=parent;
        parent=temp->parent;
        if(parent!=NULL)
        {
            for(i=0;i<=parent->no;i++)
            {
                if(parent->child[i]==temp) break;
            }
            conc=i-1;
        }
        if(left!=NULL) nleft=left->no;
        if(right!=NULL) nright=right->no;
        if(temp->no>=(N-1)/2||temp==r.start) return r;
        else if(nleft>=(N+1)/2)
        {
            //borrow left sibling case
            //parent   0 1 2 3 
            //child   A B C D E
            //if temp=D then conc->2
            for(i=temp->no;i>0;i=i-1)
            {
                temp->keys[i]=temp->keys[i-1];
                temp->child[i+1]=temp->child[i];
            }
            temp->child[i+1]=temp->child[i];
            temp->keys[0]=parent->keys[conc];
            temp->child[0]=left->child[nleft];  
            if(temp->child[0]!=NULL) temp->child[0]->parent=temp;
            temp->no++; //temp node assign complete
            parent->keys[conc]=left->keys[nleft-1]; //parent assign complete
            left->keys[nleft-1]=0;
            left->child[nleft]=NULL;
            left->no=left->no-1; //left assignment complete
            return r;
        }
        else if(nright>=(N+1)/2)
        {
            temp->keys[temp->no]=parent->keys[conc+1];
            temp->no++;
            temp->child[temp->no]=right->child[0];
            if(temp->child[temp->no]!=NULL)temp->child[temp->no]->parent=temp;
            //temp assignment complete
            parent->keys[conc+1]=right->keys[0]; //parent assign comp
            for(i=0;i<nright;i++)
            {
                right->keys[i]=right->keys[i+1];
                right->child[i]=right->child[i+1];
            }
            right->child[i]=right->child[i+1];
            right->keys[nright-1]=0;
            right->child[nright]=NULL;
            right->no=right->no-1; //right assignment complete
            return r;
        }
        else return merge(r,left,temp,right);
    } 
    return r;
}
Root case_sep(Root r,Node *temp,int index)
{
    //function to seperate into cases for deletion according to algorithm
    //assume leaf only comes here
    //pairin to be deleted number is greater then parent[pairin]
    int nright=0,nleft=0,i,pairin=0;
    Node *right=NULL,*left=NULL,*parent=temp->parent;
    if(parent==NULL)
    {
        for(i=0;i<=temp->no;i++)
        {
            if(temp->child[i]!=NULL) break;
        }
        if(i==temp->no+1)
        {
            for(i=index;i<temp->no-1;i++)
            {
                temp->keys[i]=temp->keys[i+1];
            }
            temp->keys[i]=0;
            temp->no=temp->no-1;
            if(temp->no==0)
            {
                r.start=NULL;
                destroyer(temp);
            }
            return r;
        }
        Node *temp2;
        while(temp->child[index]!=NULL||temp->child[index+1]!=NULL)
        {
            if(temp->child[index+1]!=NULL)
            {
                temp2=insucc(temp,index);
                i=temp->keys[index];
                temp->keys[index]=temp2->keys[0];
                temp2->keys[0]=i;
                index=0;
                temp=temp2;
            }
            else
            {
                temp=inpre(temp,index);
                i=temp->keys[index];
                temp->keys[index]=temp2->keys[temp2->no-1];
                temp2->keys[temp2->no-1]=i;
                index=temp2->no-1;
                temp=temp2;
            }
        }
        return case_sep(r,temp,index);
    }
    for(i=0;i<parent->no;i++)
    {
        if(temp==parent->child[i]) break;
    }
    if(i==0)
    {
        left=NULL;
        pairin=-1;
        right=parent->child[1];
    }
    else if(i==parent->no)
    {
        right=NULL;
        left=parent->child[parent->no-1]; //
    }
    else
    {
        pairin=i-1;
        left=parent->child[i-1];
        right=parent->child[i+1];
    }
    if(left!=NULL) nleft=left->no;
    if(right!=NULL) nright=right->no;
    if(temp->child[index]!=NULL||temp->child[index+1]!=NULL)
    {
        //test for more than one swappings
        //internal node case
        Node *temp2;
        while(temp->child[index]!=NULL||temp->child[index+1]!=NULL)
        {
            if(temp->child[index+1]!=NULL)
            {
                temp2=insucc(temp,index);
                i=temp->keys[index];
                temp->keys[index]=temp2->keys[0];
                temp2->keys[0]=i;
                index=0;
                temp=temp2;
            }
            else
            {
                temp=inpre(temp,index);
                i=temp->keys[index];
                temp->keys[index]=temp2->keys[temp2->no-1];
                temp2->keys[temp2->no-1]=i;
                index=temp2->no-1;
                temp=temp2;
            }
        }
        return case_sep(r,temp,index);
    }
    else if (temp->no>=(N+1)/2)
    {
        //leaf with enough keys
        for(i=index;i<temp->no-1;i++)
        {
            temp->keys[i]=temp->keys[i+1];
        }
        temp->no=temp->no-1;
        return r;
    }
    else if(nleft>=(N+1)/2)
    {
        //left sibling have more 
        for(i=temp->no;i>0;i=i-1)
        {
            temp->keys[i]=temp->keys[i-1];
            temp->child[i+1]=temp->child[i];
        }
        temp->no++;
        temp->keys[0]=parent->keys[pairin];
        temp->child[0]=left->child[left->no];
        if(temp->child[0]!=NULL)temp->child[0]->parent=temp;
        parent->keys[pairin]=left->keys[left->no-1];
        left->keys[left->no-1]=0;
        left->child[left->no]=NULL;
        left->no=left->no-1;
        return case_sep(r,temp,index+1);
    }
    else if(nright>=(N+1)/2)
    {
        //right sibling have more
        temp->keys[temp->no]=parent->keys[pairin+1];
        temp->no++;
        temp->child[temp->no]=right->child[0];
        if(temp->child[temp->no]!=NULL) temp->child[temp->no]->parent=temp;
        parent->keys[pairin+1]=right->keys[0];
        for(i=0;i<right->no;i++)
        {
            right->keys[i]=right->keys[i+1];
            right->child[i]=right->child[i+1];
        }
        right->child[i+1]=NULL;
        right->no=right->no-1;
        right->keys[right->no]=0;
        return case_sep(r,temp,index);
    }
    else if(nleft<=(N-1)/2&&nright<=(N-1)/2)
    {
        for(i=index;i<temp->no-1;i++)
        {
            temp->keys[i]=temp->keys[i+1];
            temp->child[i]=temp->child[i+1];
        }
        temp->child[i]=temp->child[i+1];
        temp->keys[i]=0;
        temp->child[i+1]=NULL;
        temp->no=temp->no-1; //deletes req node
        return merge(r,left_founder(temp),temp,right_founder(temp));
    }
    else return r;
    return r;
}
Root delete_c(Root r,int age)
{
    //Find node for which age is to be deleted 
    Node *temp=r.start;
    int i;
    if(temp==NULL)
    {
        cout<<"Empty tree try to insert some data"<<endl;
        return r;
    }
    while(temp!=NULL)
    {
        if(age<temp->keys[0]) temp=temp->child[0];
        if(age>temp->keys[temp->no-1]) temp=temp->child[temp->no];
        else
        {
            for(i=0;i<temp->no;i++)
            {
                if(age==temp->keys[i])
                {
                    cout<<"*** Deleted Successfully ***"<<endl;
                    return case_sep(r,temp,i);
                } 
                if(age>temp->keys[i]&&age<temp->keys[i+1])
                {
                    temp=temp->child[i+1];
                    break;
                }
            }
        }
    }
    cout<<"Not found entry with given age"<<endl;
    return r;
}
int main()
{
    if(N<3||N%2==0)
    {
        cout<<"Value of N should be ODD and greater than or equal to 3"<<endl;
        return 0;
    }
    Root r; //declare a root node with NULL
    r.start=NULL;
    r=insert_a(r,1);
    r=insert_a(r,12);
    r=insert_a(r,8);
    r=insert_a(r,2);
    r=insert_a(r,25);       
    r=insert_a(r,6);
    r=insert_a(r,14);
    r=insert_a(r,28); 
    r=insert_a(r,17);
    r=insert_a(r,7);
    r=insert_a(r,52);
    r=insert_a(r,16);
    r=insert_a(r,48);
    r=insert_a(r,68);
    r=insert_a(r,3);
    r=insert_a(r,26);
    r=insert_a(r,29);
    r=insert_a(r,53);
    r=insert_a(r,55);
    r=insert_a(r,45); 
    r=insert_a(r,10);
    r=insert_a(r,11);
    search_b(r,17);
    r=delete_c(r,1);
    r=delete_c(r,12); 
    r=delete_c(r,8); 
    r=delete_c(r,2); 
    r=delete_c(r,25);       
    r=delete_c(r,6); 
    r=delete_c(r,14); 
    r=delete_c(r,28); 
    r=delete_c(r,17);
    r=delete_c(r,7);
    r=delete_c(r,52);
    r=delete_c(r,16);
    r=delete_c(r,48);
    r=delete_c(r,68);  
    r=delete_c(r,3);
    r=delete_c(r,26);
    r=delete_c(r,29);
    r=delete_c(r,53);
    r=delete_c(r,55);
    r=delete_c(r,45); 
    r=delete_c(r,10);
    r=delete_c(r,11);
    printer(r.start);
    cout<<endl;
    free_mem(r.start);    
    return 0;
}