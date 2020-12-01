# if!defined(_M_PROCESSING_LINE_H)
# define _M_PROCESSING_LINE_H
# include<cstdio>

void print_line(size_t len,int total_len)
{
    printf("\r");
    printf("[%d|%d]:",len,total_len);
    for(int i=0;i<len;i++)
        printf("#");
}

# endif