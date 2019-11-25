#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int check(char *buf)
{
    int i=0,count=0,mid,sum=0;
    char tmp[4];

    while(1)
    {
        if(buf[i]>='0' && buf[i]<='9')
        {
            if(count>=3)
                return -1;
            tmp[count] = buf[i];
            count++;
        }
        else if(buf[i]=='.')
        {
            if(sum>=4)
                return -2;
            mid = atoi(buf);
            if(mid<0 || mid>255)
                return -3;
            count = 0;
            sum++;
            for(int j=0;j<3;j++)
                tmp[j] = '\0';
        }
        else if(buf[i]=='\0')
        {
            mid = atoi(buf);
            if(mid<0 || mid>255)
                return -4;
            if(++sum==4)
                return 1;
            return -5;
        }
        else
            return -6;
        i++;
    }
}

void main()
{
    FILE *fp = fopen("./input.txt","r");

    fseek(fp,0,SEEK_END);
    int length = ftell(fp);
    fseek(fp,0,SEEK_SET);

    char *buf,*sub_buf;
    buf = (char*)malloc(length+8);
    sub_buf = (char*)malloc(128);
    int a=fread(buf,1,length+8,fp);
    buf[a] = EOF;

    int i = 0, j = 0;

    while(1)
    {
        if(buf[j]!='\r' && buf[j]!=EOF)
        {
            if(i>=127)
            {
                printf("too long\n");
                return ;
            }
            sub_buf[i++] = buf[j++];
        }
        else
        {
            sub_buf[i] = '\0';
            printf("%d\n",check(sub_buf));
            if((check(sub_buf))!=1)
                printf("%s is not a correct ip.\n",sub_buf);
            memset(sub_buf,0,128);
            if(buf[j]==EOF)
                break;
            i=0;
            j+=2;
        }
    }

    free(buf);
    free(sub_buf);
}
