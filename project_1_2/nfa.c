#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//  The item of Transition Function List.
typedef struct func{
    int this_q;
    int next_q;
    struct func *next_func;
} func;

//  NFA container.
struct {
    int Q;          //  States.
    char *Alphabet; //  Accepted characters.
    int q0;         //  Start state.
    func **Func;    //  Transition functions.
    int *F;         //  End states.

    int Alphabet_size;
    int F_size;
} n;

//  Input sentence buffer.
struct {
    int pos;
    char buf[128];
} input;

//  Context of NFA.
typedef struct {
    int bracket_depth;  //  The number of pairs of brackets.
    int *end_now;       //  The end states by now.
    int or_start;       //  Back to here when meeting operation "or(|)".
    int star_start;     //  ...... operation "star(*)".

    int end_num;        //  The number of end states by now.
} context_;

int map_c_to_i(char c)
{
    if(c>=33 && c<=176)
        return c-33;
    else 
        return -1;
}

//  When q != -1, add a single new end state q to current context. \
        Otherwise, add all of context adder's end states to current states.
void add_F(context_ *context,int q,context_ *adder)
{
    if(q!=-1)
    {
        int *tmp = (int*)malloc(sizeof(int)*(context->end_num+1));
        memcpy(tmp,context->end_now,sizeof(int)*context->end_num);
        tmp[context->end_num] = q;
        free(context->end_now);
        context->end_now = tmp;
        context->end_num ++;
    }
    else 
    {
        int *tmp = (int*)malloc(sizeof(int)*(context->end_num + adder->end_num));
        memcpy(tmp,context->end_now,context->end_num*sizeof(int));
        memcpy(tmp+context->end_num,adder->end_now,adder->end_num*sizeof(int));
        free(context->end_now);
        context->end_now = tmp;
        context->end_num += adder->end_num;
    }
}

//  Delete the giving context's end states.
void del_F(context_ *context)
{
    //  Can't free context->end_now here but in add_F function.
    context->end_num = 0;
}

int is_letter(char c)
{
    if((c>=33 && c<=176) && (c!='(' && c!=')' && c!='|' && c!='*'))
        return 1;
    else 
        return 0;
}

//  When this_q != -1, add a new transition function which goes \
        from this_q to next_q after receiving c.    \
        Otherwise, add context->end_num functions which go from \
        the end states of *context to next_q after receiving c.
void add_func(char c,int this_q,int next_q,context_ *context)
{
    if(this_q!=-1)
    {
        func *tmp = (func*)malloc(sizeof(func));
        tmp->this_q = this_q;
        tmp->next_q = next_q;
        tmp->next_func = n.Func[map_c_to_i(c)];
        n.Func[map_c_to_i(c)] = tmp;
    }
    else 
        for(int i=0;i<context->end_num;i++)
            add_func(c,context->end_now[i],next_q,NULL);
}

//  Use this recursive function to create NFA.
context_ go(context_ context)
{
    while(1)
    {
        if(input.buf[input.pos]=='\0')
        {
            return context;
        }
        else if(is_letter(input.buf[input.pos]))
        {
            add_func('#',-1,n.Q,&context);      //    #: empty character；
            context.star_start = n.Q;
            add_func(input.buf[input.pos],n.Q,n.Q+1,NULL);
            n.Q ++;
            del_F(&context);
            add_F(&context,n.Q,NULL);
            n.Q ++;
        }
        else if(input.buf[input.pos]=='(')
        {
            input.pos ++;
            //  There may produce a negligible state sometimes, but it guarantees \
                    safety and has no influence on correctness.
            if(context.end_num>1)
            {
                add_func('#',-1,n.Q,&context);
                del_F(&context);
                add_F(&context,n.Q,NULL);
                n.Q ++;
            }
            context.star_start = context.end_now[0];
            context.bracket_depth ++;

            int *tmp = (int*)malloc(sizeof(int));
            *tmp = context.end_now[0];
            context_ rev_context = {context.bracket_depth,tmp,context.end_now[0],n.Q,1};
            rev_context = go(rev_context);
            free(context.end_now);
            context.end_now = rev_context.end_now;
            context.end_num = rev_context.end_num;
            context.bracket_depth = rev_context.bracket_depth;
        }
        else if(input.buf[input.pos]==')')
        {
            context.bracket_depth --;
            return context;
        }
        else if(input.buf[input.pos]=='*')
        {
            add_func('#',-1,context.star_start,&context);
            add_F(&context,context.star_start,NULL);
        }
        else if(input.buf[input.pos]=='|')
        {
            input.pos ++;
            int *tmp = (int*)malloc(sizeof(int));
            *tmp = context.or_start;
            context_ rev_context = {context.bracket_depth,tmp,context.or_start,n.Q,1};
            rev_context = go(rev_context);
            add_F(&context,-1,&rev_context);
            context.bracket_depth = rev_context.bracket_depth;
            free(rev_context.end_now);
            return context;
        }
        else 
        {
            printf("Unrecognized character!\n");
            exit(0);
        }
        input.pos ++;
    }
}

//  Initialize the process.
int initialize(context_ *context)
{
    //  Initialize the context of NFA.
    context->bracket_depth = 0;
    context->end_now = (int*)malloc(sizeof(int));
    context->end_now[0] = 0;
    context->or_start = 0;
    context->star_start = n.Q;
    context->end_num = 1;

    //  Initialize the input buffer.
    FILE *fp = fopen("./input.txt","r");
    fgets(input.buf,127,fp);
    input.pos = 0;

    //  Initialize the NFA container.
    n.Alphabet_size = 0;
    n.q0 = 0;
    n.Q = 1;
    n.Func = (func**)malloc(144*sizeof(func*));
    n.F_size = 1;
    n.F = (int*)malloc(sizeof(int));
    n.F[0] = n.q0;
    int j=0;
    for(;j<144;j++)
        n.Func[j] = NULL;

    //  Count the number of characters.
    int i=0;
    char c;
    while((c=input.buf[i])!='\0')
    {
        if(c=='|' || c=='*' || c=='(' || c==')' || c=='#') ;
        else 
        {
            if(c>=33 && c<=176)
            {
                for(j=0;j<n.Alphabet_size;j++)
                {
                    if(input.buf[i]==n.Alphabet[j])
                        break;
                }
                if(j==n.Alphabet_size)
                {
                    char *tmp = (char*)malloc(n.Alphabet_size+1);
                    memcpy(tmp,n.Alphabet,n.Alphabet_size);
                    tmp[n.Alphabet_size] = input.buf[i];
                    n.Alphabet_size++;
                    free(n.Alphabet);
                    n.Alphabet = tmp;
                }
            }
            else 
            {
                printf("Unrecognized character!\n");
                return 0;
            }
        }
        i++;
    }

    return 1;
}

//  Create the NFA.
int create_nfa(context_ *context)
{
    *context = go(*context);
    if(context->bracket_depth != 0)
    {
        printf("The number of \'(\' and \')\' should be equal.\n");
        return 0;
    }
    n.F = (int*)malloc(sizeof(int)*context->end_num);
    n.F_size = context->end_num;
    for(int i=0;i<n.F_size;i++)
        n.F[i] = context->end_now[i];
    free(context->end_now);
    return 1;
}

//  Before: Functions are organized by charaters;   \
    After : Functions are organized by states.
void reshape_func()
{
    func **f_tmp = (func**)malloc(n.Q*sizeof(func*));
    int i = 0;
    func *p,*q;
    for(;i<144;i++)
    {
        p = n.Func[i];
        while(p)
        {
            q = p;
            p = p->next_func;
            q->next_func = f_tmp[q->this_q];
            f_tmp[q->this_q] = q;
            q->this_q = i+33;
        }
    }
    free(n.Func);
    n.Func = f_tmp;
}

//  Print the NFA.
void print_nfa()
{
    int i;
    printf("-------------NFA--------------\n");
    printf("Alphabet:\n\t");
    for(i=0;i<n.Alphabet_size;i++)
        printf("%c ",n.Alphabet[i]);
    printf("#");
    printf("\nq0:\n\t%d",n.q0);
    printf("\nQ:\n\t%d\n",n.Q);
    printf("Transition:\n");
    reshape_func();
    func *p,*q;
    for(i=0;i<n.Q;i++)
    {
        p = n.Func[i];
        while(p)
        {
            printf("\t(%d , %c) -> %d\n",i,p->this_q,p->next_q);
            p = p->next_func;
        }
    }
    printf("F:\n\t");
    for(i=0;i<n.F_size;i++)
        printf("%d ",n.F[i]);
    printf("\n");
    printf("------------------------------\n\n");
}

//  Decide the next step when receiving input[pos] at current state \
            according to the created NFA. \
        Parameter depth means the number of states that input[pos] has passed. \
        Returning 1 means success, while 0 means failure. \
        It is a recursive function.
int recognize_string(char *input,int state,int pos,int depth)
{
    //  Which means it got into a dead circle.
    if(depth>n.Q)
        return 0;
    int i;
    char c = input[pos];
    if(c=='\0')
    {
        for(i=0;i<n.F_size;i++)
        {
            if(state==n.F[i])
                return 1;
        }
        func *p = n.Func[state];
        while(p)
        {
            if(p->this_q=='#')
            {
                if(recognize_string(input,p->next_q,pos,depth+1))
                    return 1;
            }
            p = p->next_func;
        }
        return 0;
    }

    //  Without this, it may get into a dead circle.
    for(i=0;i<n.Alphabet_size;i++)
        if(c==n.Alphabet[i])
            break;
    if(i==n.Alphabet_size)
        return 0;

    func *p = n.Func[state];
    while(p)
    {
        if(p->this_q=='#')
        {
            if(recognize_string(input,p->next_q,pos,depth+1))
                return 1;
        }
        else if(p->this_q==c)
        {
            if(recognize_string(input,p->next_q,pos+1,1))
                return 1;
        }
        p = p->next_func;
    }
    return 0;
}

//  Prepare and recognize the input sentence.
void recognize()
{
    char c;
    char buf[128];
    while(1)
    {
        printf("Do you want to recognize a sentence? (y/n)");
        scanf("%c",&c);
        switch(c)
        {
            case 'n':
                return ;
            case 'y':
                // fflush(stdin);
                while((c=getchar())!='\n') ;
                printf("Input a sentence(no more than 128 bytes included \'\\0\'):\n");
                gets(buf);
                if(recognize_string(buf,n.q0,0,1))
                    printf("It is accepted by the above NFA.\n");
                else 
                    printf("It can't be recognized by the above NFA.\n");
                break;
            default:
                while((c=getchar())!='\n') ;
                printf("Please input \'y\' or \'n\'.\n");
                break;
        }
    }
}

//  Free the memory.
void free_memory()
{
    func *p,*q;
    for(int i=0;i<n.Q;i++)
    {
        p = n.Func[i];
        while(p)
        {
            q = p;
            p = p->next_func;
            free(q);
        }
    }
    free(n.Func);
    free(n.F);
    free(n.Alphabet);
}


void main()
{
    context_ context;

    if(!initialize(&context))
        return ;
    if(!create_nfa(&context))
        return ;

    print_nfa();

    recognize();

    free_memory();
}